
#include <nova.hpp>

#include <asset.hpp>
#include <intrin.hpp>
#include <math.hpp>

namespace nova {
	math::Vec3 get_particle_pos(Particle * particle, float t, float dt) {
		float axis_offset = intrin::sin(t + particle->velocity.x) * math::PI * 1.25f;
		// float axis_offset = (math::simplex_noise(particle->velocity.x * 0.25f, t * 0.25f) * 2.0f - 1.0f) * math::PI * 1.25f;
		math::Vec3 rotated_axis = (math::rotate_around_y(axis_offset) * math::vec4(particle->velocity, 1.0f)).xyz;
		math::Vec3 pos = (math::rotate_around_axis(rotated_axis, t) * math::vec4(particle->position, 1.0f)).xyz;
		pos *= 1.0f + math::simplex_noise(rotated_axis.x, t) * 0.08f;
		return pos;
	}

	void tick(GameState * game_state) {
		float g_constant = 0.01f;

		float protostar_initial_mass = 76.0f;
		float protostar_jeans_mass = protostar_initial_mass + 240.0f;
		float protostar_collapsed_mass = 160000.0f;

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

			uint32_t quad_vert_id = gl::compile_shader_from_source(BASIC_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t quad_frag_id = gl::compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->quad_program_id = gl::link_shader_program(quad_vert_id, quad_frag_id);

			game_state->quad_vertex_buffer = gl::create_vertex_buffer(asset::quad_verts, ARRAY_COUNT(asset::quad_verts), 3, GL_STATIC_DRAW);

			uint32_t particle_vert_id = gl::compile_shader_from_source(PARTICLE_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t particle_frag_id = gl::compile_shader_from_source(PARTICLE_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->particle_program_id = gl::link_shader_program(particle_vert_id, particle_frag_id);
			
			game_state->particle_vert_length = 4;

			game_state->cloud_particles.length = 0;
			game_state->cloud_particles.v = new Particle[game_state->cloud_particles.length];
			game_state->cloud_particles.verts_length = game_state->cloud_particles.length * game_state->particle_vert_length * 3;
			game_state->cloud_particles.verts = new float[game_state->cloud_particles.verts_length];
			for(uint32_t i = 0; i < game_state->cloud_particles.length; i++) {
				Particle * particle = game_state->cloud_particles.v + i;
				particle->velocity = math::normalize(math::rand_vec3() * 2.0f - 1.0f);
				particle->position = math::normalize((math::rotate_around_y(90.0f + intrin::sin(particle->velocity.x)) * math::vec4(particle->velocity, 1.0f)).xyz);
				particle->mass = 1.0f;

				math::Vec3 pos = get_particle_pos(particle, 0.0f, 0.0f);

				uint32_t v_idx = i * (game_state->particle_vert_length * 3);
				for(uint32_t k = 0; k < game_state->particle_vert_length; k++) {
					game_state->cloud_particles.verts[v_idx++] = pos.v[0];
					game_state->cloud_particles.verts[v_idx++] = pos.v[1];
					game_state->cloud_particles.verts[v_idx++] = pos.v[2];					
				}
			}

			game_state->cloud_particles.vertex_buffer = gl::create_vertex_buffer(game_state->cloud_particles.verts, game_state->cloud_particles.verts_length, 3, GL_DYNAMIC_DRAW);

			game_state->disc_particles.length = 65536;
			game_state->disc_particles.v = new Particle[game_state->disc_particles.length];
			game_state->disc_particles.verts_length = game_state->disc_particles.length * game_state->particle_vert_length * 3;
			game_state->disc_particles.verts = new float[game_state->disc_particles.verts_length];
			for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
				float d = math::rand_float();
				d = d * d;
				d *= 4.0f;
				
				//TODO: Better initial position, pick a random point a circumference then move it in/out
				math::Vec2 pos_2d = math::rand_sample_in_circle() * d;
				pos_2d += math::normalize(pos_2d) * 0.2f;
				math::Vec3 pos = math::vec3(pos_2d.x, 0.0f, pos_2d.y);

				// float vel_mag = (4.0f / math::length_squared(pos)) * 0.2f;
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

			game_state->disc_particles.vertex_buffer = gl::create_vertex_buffer(game_state->disc_particles.verts, game_state->disc_particles.verts_length, 3, GL_DYNAMIC_DRAW);

			game_state->protostar_mass = protostar_initial_mass;

			game_state->running_particle_sim = true;
			game_state->camera_pos = 0.0f;

			game_state->view_matrix = math::MAT4_IDENTITY;
			float aspect_ratio = game_state->back_buffer_width / game_state->back_buffer_height;
			game_state->projection_matrix = math::perspective_projection(aspect_ratio, 60.0f, 0.3f, 1000.0f);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(game_state->key_space_pressed) {
			game_state->running_particle_sim = !game_state->running_particle_sim;
		}

		game_state->camera_pos += game_state->delta_time * 0.04f;
		// if(!game_state->running_particle_sim) {
		// 	game_state->camera_pos += game_state->delta_time * 0.2f;
		// }

		math::Vec3 initial_camera_pos = math::normalize(math::vec3(0.0f, 1.0f, 1.0f)) * 3.0f;
		math::Vec3 camera_pos = (math::rotate_around_y(game_state->camera_pos) * math::vec4(initial_camera_pos, 1.0f)).xyz;
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

		glUseProgram(game_state->particle_program_id); {
			float dt = game_state->delta_time * 56.0f * 0.5f;
			dt = math::min(dt, 1.0f);

			if(game_state->running_particle_sim) {
				static float m_t = 0.0f;
				m_t = math::lerp(m_t, math::min(math::length(game_state->mouse_delta) * 0.002f, 0.04f), game_state->delta_time * 8.0f);

				static float t = 0.0f;
				t += m_t;
				t += game_state->delta_time * 0.2f;
				// t += game_state->delta_time;

				for(uint32_t i = 0; i < game_state->cloud_particles.length; i++) {
					Particle * particle = game_state->cloud_particles.v + i;
					math::Vec3 pos = get_particle_pos(particle, t, dt);

					uint32_t v_idx = i * (game_state->particle_vert_length * 3);

					game_state->cloud_particles.verts[v_idx +  0] = math::lerp(game_state->cloud_particles.verts[v_idx +  0], game_state->cloud_particles.verts[v_idx +  3], dt);
					game_state->cloud_particles.verts[v_idx +  1] = math::lerp(game_state->cloud_particles.verts[v_idx +  1], game_state->cloud_particles.verts[v_idx +  4], dt);
					game_state->cloud_particles.verts[v_idx +  2] = math::lerp(game_state->cloud_particles.verts[v_idx +  2], game_state->cloud_particles.verts[v_idx +  5], dt);

					game_state->cloud_particles.verts[v_idx +  3] = math::lerp(game_state->cloud_particles.verts[v_idx +  3], game_state->cloud_particles.verts[v_idx +  9], dt);
					game_state->cloud_particles.verts[v_idx +  4] = math::lerp(game_state->cloud_particles.verts[v_idx +  4], game_state->cloud_particles.verts[v_idx + 10], dt);
					game_state->cloud_particles.verts[v_idx +  5] = math::lerp(game_state->cloud_particles.verts[v_idx +  5], game_state->cloud_particles.verts[v_idx + 11], dt);

					game_state->cloud_particles.verts[v_idx +  6] = game_state->cloud_particles.verts[v_idx +  3];
					game_state->cloud_particles.verts[v_idx +  7] = game_state->cloud_particles.verts[v_idx +  4];
					game_state->cloud_particles.verts[v_idx +  8] = game_state->cloud_particles.verts[v_idx +  5];

					game_state->cloud_particles.verts[v_idx +  9] = pos.v[0];
					game_state->cloud_particles.verts[v_idx + 10] = pos.v[1];
					game_state->cloud_particles.verts[v_idx + 11] = pos.v[2];
				}				
			}

			uint32_t xform_id = glGetUniformLocation(game_state->particle_program_id, "xform");
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, view_projection_matrix.v);
			
			glBindBuffer(GL_ARRAY_BUFFER, game_state->cloud_particles.vertex_buffer.id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, game_state->cloud_particles.vertex_buffer.size_in_bytes, game_state->cloud_particles.verts);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_LINES, 0, game_state->cloud_particles.vertex_buffer.vert_count);

			{
				float protostar_mass = game_state->protostar_mass;

				float touch_gravity_mod = game_state->key_mouse_down ? 1.0f : 0.0f;
				float physics_delta_time = game_state->delta_time * 0.04f;

				for(uint32_t i = 0; i < game_state->disc_particles.length; i++) {
					Particle * particle = game_state->disc_particles.v + i;

					math::Vec3 acceleration = math::vec3(0.0f);

					math::Vec3 centre_dir = -particle->position;
					float centre_r = math::length_squared(centre_dir);
					if(centre_r < 0.002f && particle->mass > 0.0f) {
						game_state->protostar_mass += 0.08f;
						particle->mass = 0.0f;
					}
					else {
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

					math::Vec3 pos = particle->position;

					uint32_t v_idx = i * (game_state->particle_vert_length * 3);

					game_state->disc_particles.verts[v_idx +  0] = math::lerp(game_state->disc_particles.verts[v_idx +  0], game_state->disc_particles.verts[v_idx +  3], dt);
					game_state->disc_particles.verts[v_idx +  1] = math::lerp(game_state->disc_particles.verts[v_idx +  1], game_state->disc_particles.verts[v_idx +  4], dt);
					game_state->disc_particles.verts[v_idx +  2] = math::lerp(game_state->disc_particles.verts[v_idx +  2], game_state->disc_particles.verts[v_idx +  5], dt);

					game_state->disc_particles.verts[v_idx +  3] = math::lerp(game_state->disc_particles.verts[v_idx +  3], game_state->disc_particles.verts[v_idx +  9], dt);
					game_state->disc_particles.verts[v_idx +  4] = math::lerp(game_state->disc_particles.verts[v_idx +  4], game_state->disc_particles.verts[v_idx + 10], dt);
					game_state->disc_particles.verts[v_idx +  5] = math::lerp(game_state->disc_particles.verts[v_idx +  5], game_state->disc_particles.verts[v_idx + 11], dt);

					game_state->disc_particles.verts[v_idx +  6] = game_state->disc_particles.verts[v_idx +  3];
					game_state->disc_particles.verts[v_idx +  7] = game_state->disc_particles.verts[v_idx +  4];
					game_state->disc_particles.verts[v_idx +  8] = game_state->disc_particles.verts[v_idx +  5];

					game_state->disc_particles.verts[v_idx +  9] = pos.v[0];
					game_state->disc_particles.verts[v_idx + 10] = pos.v[1];
					game_state->disc_particles.verts[v_idx + 11] = pos.v[2];
				}

				if(game_state->protostar_mass > protostar_jeans_mass && protostar_mass < protostar_jeans_mass) {
					game_state->protostar_mass = protostar_collapsed_mass;
				}	
			}

			glBindBuffer(GL_ARRAY_BUFFER, game_state->disc_particles.vertex_buffer.id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, game_state->disc_particles.vertex_buffer.size_in_bytes, game_state->disc_particles.verts);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_LINES, 0, game_state->disc_particles.vertex_buffer.vert_count);
		}

		glUseProgram(game_state->sphere_program_id); {
			// math::Mat4 world_matrix = math::rotate_around_y(game_state->total_time * 0.08f);
			math::Mat4 world_matrix = math::scale(0.05f);
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

		glUseProgram(game_state->quad_program_id); {
			math::Mat4 world_matrix = math::translate(touch_pos) * math::rotate_around_x(math::PI * 0.5f) * math::scale(0.25f);
			math::Mat4 world_view_projection_matrix = view_projection_matrix * world_matrix;

			uint32_t xform_id = glGetUniformLocation(game_state->quad_program_id, "xform");
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, world_view_projection_matrix.v);

			uint32_t color_id = glGetUniformLocation(game_state->quad_program_id, "color");
			glUniform3f(color_id, 1.0f, 1.0f, 1.0f);

			glBindBuffer(GL_ARRAY_BUFFER, game_state->quad_vertex_buffer.id);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_TRIANGLES, 0, game_state->quad_vertex_buffer.vert_count);
		}

		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
	}
}