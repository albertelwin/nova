
#include <nova.hpp>

#include <asset.hpp>
#include <intrin.hpp>
#include <math.hpp>

namespace {
	float const g_constant = 0.01f;

	float const protostar_initial_mass = 76.0f;
	float const protostar_initial_size = 0.05f;
	float const protostar_jeans_mass = protostar_initial_mass + 240.0f;
	float const protostar_critial_mass = protostar_initial_mass + 240.0f;
	float const protostar_collapsed_mass = 160000.0f;
}

namespace nova {
	void initialize_disc_particles(GameState * game_state) {
		for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
			float d = math::rand_float();
			d *= 4.0f;
			
			math::Vec2 pos_2d = math::rand_sample_in_circle() * d;
			pos_2d += math::normalize(pos_2d) * 0.2f;
			math::Vec3 pos = math::vec3(pos_2d.x, 0.0f, pos_2d.y);

			float vel_mag = math::sqrt((g_constant * protostar_initial_mass) / math::length(pos)) * (1.0f - math::simplex_noise(pos_2d.x, pos_2d.y) * 0.1f);

			Particle * particle = game_state->disc_particles.v + i;
			particle->position = pos;
			particle->velocity = math::normalize((math::rotate_around_y(67.5f) * math::vec4(pos, 1.0f)).xyz) * vel_mag;
			particle->mass = 1.0f;

			uint32_t v_idx = i * (game_state->particle_vert_length * 3);
			for(uint32_t k = 0; k < game_state->particle_vert_length; k++) {
				game_state->disc_particles.verts[v_idx++] = pos.v[0];
				game_state->disc_particles.verts[v_idx++] = pos.v[1];
				game_state->disc_particles.verts[v_idx++] = pos.v[2];
			}
		}
	}

	void tick(GameState * game_state) {
		if(!game_state->started) {
			game_state->started = true;

			uint32_t sphere_vert_id = gl::compile_shader_from_source(STAR_VERT_SRC, GL_VERTEX_SHADER);
			// uint32_t sphere_frag_id = gl::compile_shader_from_source(STAR_FRAG_SRC, GL_FRAGMENT_SHADER);
			uint32_t sphere_frag_id = gl::compile_shader_from_source(PARTICLE_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->sphere_program_id = gl::link_shader_program(sphere_vert_id, sphere_frag_id);

			// char file_path[256];
			// std::strcpy(file_path, sys::get_exe_path());
			// //TODO: Package up assets on someway, contents of /dat is just copied into bin folder atm
			// std::strcat(file_path, "sphere.obj");
			// asset::Model model = asset::load_obj_from_file(file_path);
			// game_state->sphere_vertex_buffer = gl::create_vertex_buffer(model.vert_data, model.vert_data_length, 3, GL_STATIC_DRAW);
			game_state->sphere_vertex_buffer = gl::create_vertex_buffer(asset::icosphere_verts, ARRAY_COUNT(asset::icosphere_verts), 3, GL_STATIC_DRAW);

			uint32_t disc_vert_id = gl::compile_shader_from_source(BASIC_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t disc_frag_id = gl::compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->disc_program_id = gl::link_shader_program(disc_vert_id, disc_frag_id);

			game_state->disc_vertex_buffer = gl::create_vertex_buffer(asset::disc_verts, ARRAY_COUNT(asset::disc_verts), 3, GL_STATIC_DRAW);

			uint32_t particle_vert_id = gl::compile_shader_from_source(PARTICLE_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t particle_frag_id = gl::compile_shader_from_source(PARTICLE_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->particle_program_id = gl::link_shader_program(particle_vert_id, particle_frag_id);
			
			game_state->particle_vert_length = 4;

			game_state->disc_particles.length = 65536;
			game_state->disc_particles.v = new Particle[game_state->disc_particles.length];
			game_state->disc_particles.verts_length = game_state->disc_particles.length * game_state->particle_vert_length * 3;
			game_state->disc_particles.verts = new float[game_state->disc_particles.verts_length];
			initialize_disc_particles(game_state);

			game_state->disc_particles.vertex_buffer = gl::create_vertex_buffer(game_state->disc_particles.verts, game_state->disc_particles.verts_length, 3, GL_DYNAMIC_DRAW);

			uint32_t quad_vert_id = gl::compile_shader_from_source(SCREEN_QUAD_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t quad_frag_id = gl::compile_shader_from_source(TEXTURE_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->quad_program_id = gl::link_shader_program(quad_vert_id, quad_frag_id);
			game_state->quad_vertex_buffer = gl::create_vertex_buffer(asset::quad_verts, ARRAY_COUNT(asset::quad_verts), 3, GL_STATIC_DRAW);

			game_state->msaa_frame_buffer = gl::create_msaa_frame_buffer(game_state->back_buffer_width, game_state->back_buffer_height, 8);
			game_state->resolve_frame_buffer = gl::create_frame_buffer(game_state->back_buffer_width, game_state->back_buffer_height);

			game_state->threshold_program_id = gl::link_shader_program(quad_vert_id, gl::compile_shader_from_source(THRESHOLD_TEXTURE_FRAG_SRC, GL_FRAGMENT_SHADER));
			game_state->threshold_frame_buffer = gl::create_frame_buffer(game_state->back_buffer_width, game_state->back_buffer_height, GL_LINEAR, GL_LINEAR);

			game_state->blur_program_id = gl::link_shader_program(quad_vert_id, gl::compile_shader_from_source(BLUR_TEXTURE_FRAG_SRC, GL_FRAGMENT_SHADER));
			for(uint32_t i = 0; i < ARRAY_COUNT(game_state->blur_frame_buffers); i++) {
				game_state->blur_frame_buffers[i] = gl::create_frame_buffer(game_state->back_buffer_width, game_state->back_buffer_height / 32, GL_LINEAR, GL_LINEAR);
			}

			game_state->protostar_mass = protostar_initial_mass;
			game_state->protostar_size = protostar_initial_size;
			game_state->particles_consumed = 0;
			// game_state->particles_consumed_threshold = 0.04f;
			// game_state->particles_consumed_threshold = 0.02f;
			game_state->particles_consumed_threshold = 1.0f;
			game_state->particle_mass_consumed = (65536.0f / (float)game_state->disc_particles.length) * 0.08f;

			game_state->initial_camera = {};
			game_state->initial_camera.angle = 0.0f;
			game_state->initial_camera.dist = 3.0f;
			game_state->initial_camera.timer = 1.0f;
			game_state->camera = game_state->initial_camera;

			game_state->view_matrix = math::MAT4_IDENTITY;
			float aspect_ratio = (float)game_state->back_buffer_width / (float)game_state->back_buffer_height;
			game_state->projection_matrix = math::perspective_projection(aspect_ratio, 60.0f, 0.03f, 1000.0f);

			glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		}

		game_state->camera.angle += game_state->delta_time * 0.02f;
		if(game_state->camera.timer < 1.0f) {
			game_state->camera.timer += game_state->delta_time * 0.0002f;
			game_state->camera.dist = math::lerp(game_state->camera.dist, game_state->initial_camera.dist, game_state->camera.timer); 
		}

		float camera_dist = game_state->camera.dist;
		float camera_offset = (intrin::sin(game_state->camera.angle * 4.0f) * 0.5f + 0.5f) * math::PI * 0.0625f;
		// float camera_offset = 0.0f;

		math::Vec3 initial_camera_pos = math::normalize(math::vec3(0.0f, 1.0f, 1.0f)) * camera_dist;
		math::Vec3 camera_pos = (math::rotate_around_y(game_state->camera.angle) * math::rotate_around_x(camera_offset) * math::vec4(initial_camera_pos, 1.0f)).xyz;
		game_state->view_matrix = math::look_at(camera_pos, math::vec3(0.0f), math::VEC3_UP);
		math::Mat4 view_projection_matrix = game_state->projection_matrix * game_state->view_matrix;

		math::Mat4 inverse_view_projection = math::inverse(view_projection_matrix);

		math::Vec2 screen_pos = math::vec2(game_state->mouse_pos.x / game_state->back_buffer_width, (game_state->back_buffer_height - game_state->mouse_pos.y) / game_state->back_buffer_height) * 2.0f - 1.0f;
		math::Vec4 image_point = inverse_view_projection * math::vec4(screen_pos, 0.0f, 1.0f);

		math::Ray camera_ray = {};
		camera_ray.o = camera_pos;
		camera_ray.d = normalize((image_point.xyz / image_point.w) - camera_pos);

		math::Vec3 touch_pos = math::vec3(0.0f);
		float camera_ray_t = math::ray_plane_intersection(camera_ray, math::vec3(0.0f, 1.0f, 0.0f), 0.0f);
		if(camera_ray_t > 0.0f) {
			touch_pos = camera_ray.o + camera_ray.d * camera_ray_t;
		}

		float physics_delta_time = game_state->delta_time * 0.04f;
		if(game_state->key_rgt_mouse_down) {
			physics_delta_time *= 4.0f;	
		}

		float protostar_size = game_state->protostar_size;

		float percentage_particles_consumed = (float)game_state->particles_consumed / (float)game_state->disc_particles.length;
		if(percentage_particles_consumed < game_state->particles_consumed_threshold) {
			float protostar_mass = game_state->protostar_mass;
			float protostar_delta_mass = 0.0f;

			float touch_gravity_mod = game_state->key_lft_mouse_down ? 1.0f : 0.0f;

			for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
				Particle * particle = game_state->disc_particles.v + i;

				math::Vec3 acceleration = math::vec3(0.0f);

				math::Vec3 centre_dir = -particle->position;
				float centre_r = math::length_squared(centre_dir);
				if(centre_r < 0.002f && particle->mass > 0.0f) {
					game_state->protostar_mass += game_state->particle_mass_consumed;
					particle->mass = 0.0f;
					protostar_delta_mass += 1.0f;
					game_state->particles_consumed++;
				}
				if(centre_r > 0.0f) {
					float centre_r_sqrt = math::sqrt(centre_r);
					float centre_gravity = math::min(g_constant * (protostar_mass / centre_r), protostar_mass);
					acceleration += (centre_dir / centre_r_sqrt) * centre_gravity;				
				}

				math::Vec3 touch_dir = touch_pos - particle->position;
				float touch_r = math::length_squared(touch_dir);
				if(touch_r > 0.0f) {
					float touch_gravity = math::min(g_constant * (32.0f / touch_r), 64.0f);
					acceleration -= (touch_dir / math::sqrt(touch_r)) * touch_gravity * touch_gravity_mod;
				}

				if(particle->mass > 0.0f || protostar_mass > protostar_jeans_mass) {
					particle->velocity += acceleration * physics_delta_time;
					particle->position += particle->velocity * physics_delta_time;
				}
			}

			if(game_state->protostar_mass > protostar_jeans_mass && protostar_mass < protostar_jeans_mass) {
				game_state->protostar_mass = protostar_collapsed_mass;
			}

			// static float max_delta_mass = 0.0f;
			// if(protostar_delta_mass > max_delta_mass) {
			// 	max_delta_mass = protostar_delta_mass;
			// 	std::printf("LOG: %f\n", max_delta_mass);
			// }
		}
		else {
			static float expanding_timer = 0.0f;
			if(expanding_timer < 1.0f) {
				if(expanding_timer == 0.0f) {
					game_state->protostar_mass = 8000.0f;
				}

				expanding_timer += physics_delta_time * 2.4f;

				float remapped_percentage = (percentage_particles_consumed - game_state->particles_consumed_threshold) / (1.0f - game_state->particles_consumed_threshold);

				float animation_delta_time = physics_delta_time * 16.0f;
				game_state->protostar_mass = math::lerp(game_state->protostar_mass, 80000.0f, animation_delta_time);
				game_state->protostar_size = math::lerp(protostar_initial_size, 1.0f, math::sqr(remapped_percentage));

				float r_sqr = math::sqr(game_state->protostar_size);
				float m = game_state->protostar_mass;

				for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
					Particle * particle = game_state->disc_particles.v + i;

					math::Vec3 dir = -particle->position;
					float dist_sqr = math::length_squared(dir);
					if(dist_sqr <= r_sqr && particle->mass > 0.0f) {
						particle->mass = 0.0f;
						game_state->particles_consumed++;
					}
					else if(dist_sqr > r_sqr) {
						float dist = math::sqrt(dist_sqr);
						float g = math::min(g_constant * (m / dist_sqr), m);
						math::Vec3 a = (dir / dist) * g;

						particle->velocity += a * physics_delta_time;
						particle->position += particle->velocity * physics_delta_time;
					}
				}
			}
			else {
				game_state->camera.timer = 0.0f;
				game_state->camera.dist = game_state->initial_camera.dist * (protostar_initial_size / game_state->protostar_size);
				game_state->protostar_size = protostar_initial_size;
				game_state->protostar_mass = protostar_initial_mass;

				initialize_disc_particles(game_state);
				game_state->particles_consumed = 0;
				expanding_timer = 0.0f;
			}
		}

		float line_delta_time = math::min(game_state->delta_time * 56.0f * 0.5f, 1.0f);
		for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
			Particle * particle = game_state->disc_particles.v + i;
			math::Vec3 pos = particle->position;

			uint32_t v_idx = i * (game_state->particle_vert_length * 3);

			game_state->disc_particles.verts[v_idx +  0] = math::lerp(game_state->disc_particles.verts[v_idx +  0], game_state->disc_particles.verts[v_idx +  3], line_delta_time);
			game_state->disc_particles.verts[v_idx +  1] = math::lerp(game_state->disc_particles.verts[v_idx +  1], game_state->disc_particles.verts[v_idx +  4], line_delta_time);
			game_state->disc_particles.verts[v_idx +  2] = math::lerp(game_state->disc_particles.verts[v_idx +  2], game_state->disc_particles.verts[v_idx +  5], line_delta_time);

			game_state->disc_particles.verts[v_idx +  3] = math::lerp(game_state->disc_particles.verts[v_idx +  3], game_state->disc_particles.verts[v_idx +  9], line_delta_time);
			game_state->disc_particles.verts[v_idx +  4] = math::lerp(game_state->disc_particles.verts[v_idx +  4], game_state->disc_particles.verts[v_idx + 10], line_delta_time);
			game_state->disc_particles.verts[v_idx +  5] = math::lerp(game_state->disc_particles.verts[v_idx +  5], game_state->disc_particles.verts[v_idx + 11], line_delta_time);

			game_state->disc_particles.verts[v_idx +  6] = game_state->disc_particles.verts[v_idx +  3];
			game_state->disc_particles.verts[v_idx +  7] = game_state->disc_particles.verts[v_idx +  4];
			game_state->disc_particles.verts[v_idx +  8] = game_state->disc_particles.verts[v_idx +  5];

			game_state->disc_particles.verts[v_idx +  9] = pos.v[0];
			game_state->disc_particles.verts[v_idx + 10] = pos.v[1];
			game_state->disc_particles.verts[v_idx + 11] = pos.v[2];
		}

		glBindFramebuffer(GL_FRAMEBUFFER, game_state->msaa_frame_buffer.id);
		glViewport(0, 0, game_state->msaa_frame_buffer.width, game_state->msaa_frame_buffer.height);
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glViewport(0, 0, game_state->back_buffer_width, game_state->back_buffer_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(game_state->particle_program_id); {
			uint32_t xform_id = glGetUniformLocation(game_state->particle_program_id, "xform");
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, view_projection_matrix.v);

			glBindBuffer(GL_ARRAY_BUFFER, game_state->disc_particles.vertex_buffer.id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, game_state->disc_particles.vertex_buffer.size_in_bytes, game_state->disc_particles.verts);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_LINES, 0, game_state->disc_particles.vertex_buffer.vert_count);
		}

		glUseProgram(game_state->sphere_program_id); {
			// math::Mat4 world_matrix = math::rotate_around_y(game_state->total_time * 0.08f);
			math::Mat4 world_matrix = math::scale(protostar_size);
			math::Mat4 world_view_projection_matrix = view_projection_matrix * world_matrix;

			uint32_t time_id = glGetUniformLocation(game_state->sphere_program_id, "time");
			uint32_t world_matrix_id = glGetUniformLocation(game_state->sphere_program_id, "world_matrix");
			uint32_t world_view_projection_matrix_id = glGetUniformLocation(game_state->sphere_program_id, "world_view_projection_matrix");

			glUniform1f(time_id, game_state->total_time);
			glUniformMatrix4fv(world_matrix_id, 1, GL_FALSE, world_matrix.v);
			glUniformMatrix4fv(world_view_projection_matrix_id, 1, GL_FALSE, world_view_projection_matrix.v);

			glBindBuffer(GL_ARRAY_BUFFER, game_state->sphere_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_TRIANGLES, 0, game_state->sphere_vertex_buffer.vert_count);
		}

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

		glUseProgram(game_state->disc_program_id); {
			math::Mat4 world_matrix = math::translate(touch_pos) * math::rotate_around_x(math::PI * 0.5f) * math::scale(0.025f * (camera_dist / game_state->initial_camera.dist));
			math::Mat4 world_view_projection_matrix = view_projection_matrix * world_matrix;

			uint32_t xform_id = glGetUniformLocation(game_state->disc_program_id, "xform");
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, world_view_projection_matrix.v);

			uint32_t color_id = glGetUniformLocation(game_state->disc_program_id, "color");
			glUniform3f(color_id, 1.0f, 1.0f, 1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, game_state->disc_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_TRIANGLES, 0, game_state->disc_vertex_buffer.vert_count);
		}

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		gl::resolve_msaa_frame_buffer(game_state->msaa_frame_buffer, game_state->resolve_frame_buffer);

		glBindFramebuffer(GL_FRAMEBUFFER, game_state->threshold_frame_buffer.id);
		glViewport(0, 0, game_state->threshold_frame_buffer.width, game_state->threshold_frame_buffer.height);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(game_state->threshold_program_id); {
			glBindBuffer(GL_ARRAY_BUFFER, game_state->quad_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, game_state->resolve_frame_buffer.texture_id);
			glUniform1i(glGetUniformLocation(game_state->threshold_program_id, "tex_0"), 0);

			glUniform1f(glGetUniformLocation(game_state->threshold_program_id, "bias"), -0.6f);
			glUniform1f(glGetUniformLocation(game_state->threshold_program_id, "scale"), 3.0f);

			glDrawArrays(GL_TRIANGLES, 0, game_state->quad_vertex_buffer.vert_count);
		}

		uint32_t blur_tex_id = game_state->threshold_frame_buffer.texture_id;
		glUseProgram(game_state->blur_program_id); {
			glBindBuffer(GL_ARRAY_BUFFER, game_state->quad_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		for(uint32_t i = 0; i < 2; i++) {
				uint32_t idx = i % 2;

				glBindFramebuffer(GL_FRAMEBUFFER, game_state->blur_frame_buffers[idx].id);
				glViewport(0, 0, game_state->blur_frame_buffers[idx].width, game_state->blur_frame_buffers[idx].height);
				glClear(GL_COLOR_BUFFER_BIT);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, blur_tex_id);
				glUniform1i(glGetUniformLocation(game_state->threshold_program_id, "tex_0"), 0);

				glUniform1f(glGetUniformLocation(game_state->blur_program_id, "tex_height_r"), 1.0f / (float)game_state->blur_frame_buffers[idx].height);

				glDrawArrays(GL_TRIANGLES, 0, game_state->quad_vertex_buffer.vert_count);

				blur_tex_id = game_state->blur_frame_buffers[idx].texture_id; 
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, game_state->back_buffer_width, game_state->back_buffer_height);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(game_state->quad_program_id); {
			glBindBuffer(GL_ARRAY_BUFFER, game_state->quad_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, game_state->resolve_frame_buffer.texture_id);
			glUniform1i(glGetUniformLocation(game_state->quad_program_id, "tex_0"), 0);

			glDrawArrays(GL_TRIANGLES, 0, game_state->quad_vertex_buffer.vert_count);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);

		glUseProgram(game_state->quad_program_id); {
			glBindBuffer(GL_ARRAY_BUFFER, game_state->quad_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blur_tex_id);
			// glBindTexture(GL_TEXTURE_2D, game_state->threshold_frame_buffer.texture_id);
			glUniform1i(glGetUniformLocation(game_state->quad_program_id, "tex_0"), 0);

			glDrawArrays(GL_TRIANGLES, 0, game_state->quad_vertex_buffer.vert_count);
		}

		glDisable(GL_BLEND);
	}
}