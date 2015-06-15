
#include <nova.hpp>

#include <asset.hpp>
#include <intrin.hpp>
#include <math.hpp>

namespace nova {
	math::Vec3 get_particle_pos(Particle * particle, float t, float dt) {
		float axis_offset = intrin::sin(t + particle->axis.x) * math::PI * 1.25f;
		// float axis_offset = (math::simplex_noise(particle->axis.x * 0.25f, t * 0.25f) * 2.0f - 1.0f) * math::PI * 1.25f;
		math::Vec3 rotated_axis = (math::rotate_around_y(axis_offset) * math::vec4(particle->axis, 1.0f)).xyz;
		math::Vec3 pos = (math::rotate_around_axis(rotated_axis, t) * math::vec4(particle->position, 1.0f)).xyz;
		pos *= 1.0f + math::simplex_noise(rotated_axis.x, t) * 0.08f;
		return pos;
	}

	void tick(GameState * game_state) {
		if(!game_state->started) {
			game_state->started = true;

			uint32_t sphere_vert_id = gl::compile_shader_from_source(STAR_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t sphere_frag_id = gl::compile_shader_from_source(STAR_FRAG_SRC, GL_FRAGMENT_SHADER);
			// uint32_t sphere_frag_id = gl::compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->sphere_program_id = gl::link_shader_program(sphere_vert_id, sphere_frag_id);

			char file_path[256];
			std::strcpy(file_path, sys::exe_path_str);
			std::strcat(file_path, "../dat/sphere.obj");
			asset::Model model = asset::load_obj_from_file(file_path);
			game_state->sphere_vertex_buffer = gl::create_vertex_buffer(model.vert_data, model.vert_data_length, 3, GL_STATIC_DRAW);

			uint32_t quad_vert_id = gl::compile_shader_from_source(BASIC_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t quad_frag_id = gl::compile_shader_from_source(BASIC_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->quad_program_id = gl::link_shader_program(quad_vert_id, quad_frag_id);

			game_state->quad_vertex_buffer = gl::create_vertex_buffer(asset::quad_verts, ARRAY_COUNT(asset::quad_verts), 3, GL_STATIC_DRAW);

			uint32_t particle_vert_id = gl::compile_shader_from_source(PARTICLE_VERT_SRC, GL_VERTEX_SHADER);
			uint32_t particle_frag_id = gl::compile_shader_from_source(PARTICLE_FRAG_SRC, GL_FRAGMENT_SHADER);
			game_state->particle_program_id = gl::link_shader_program(particle_vert_id, particle_frag_id);

			// game_state->particle_array_length = 65536;
			game_state->particle_array_length = 4096;
			game_state->particle_array = new Particle[game_state->particle_array_length];
			game_state->particle_vert_length = 12;
			game_state->particle_vert_array_length = game_state->particle_array_length * game_state->particle_vert_length;
			game_state->particle_vert_array = new float[game_state->particle_vert_array_length];
			for(uint32_t i = 0; i < game_state->particle_array_length; i++) {
				Particle * particle = game_state->particle_array + i;
				particle->axis = math::normalize(math::rand_vec3() * 2.0f - 1.0f);	
				particle->position = math::normalize((math::rotate_around_y(90.0f + intrin::sin(particle->axis.x)) * math::vec4(particle->axis, 1.0f)).xyz);
				math::Vec3 pos = get_particle_pos(particle, 0.0f, 0.0f);

				uint32_t v_idx = i * game_state->particle_vert_length;

				game_state->particle_vert_array[v_idx++] = pos.v[0];
				game_state->particle_vert_array[v_idx++] = pos.v[1];
				game_state->particle_vert_array[v_idx++] = pos.v[2];

				game_state->particle_vert_array[v_idx++] = pos.v[0];
				game_state->particle_vert_array[v_idx++] = pos.v[1];
				game_state->particle_vert_array[v_idx++] = pos.v[2];

				game_state->particle_vert_array[v_idx++] = pos.v[0];
				game_state->particle_vert_array[v_idx++] = pos.v[1];
				game_state->particle_vert_array[v_idx++] = pos.v[2];

				game_state->particle_vert_array[v_idx++] = pos.v[0];
				game_state->particle_vert_array[v_idx++] = pos.v[1];
				game_state->particle_vert_array[v_idx++] = pos.v[2];
			}

			game_state->particle_vertex_buffer = gl::create_vertex_buffer(game_state->particle_vert_array, game_state->particle_vert_array_length, 3, GL_DYNAMIC_DRAW);
			
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

		if(!game_state->running_particle_sim) {
			game_state->camera_pos += game_state->delta_time * 0.2f;
		}

		math::Vec3 initial_camera_pos = math::normalize(math::vec3(0.0f, 1.0f, 1.0f)) * 5.0f;
		// math::Vec3 initial_camera_pos = math::vec3(0.0f, 0.0f, 3.0f);
		math::Vec3 camera_pos = (math::rotate_around_y(game_state->camera_pos) * math::vec4(initial_camera_pos, 1.0f)).xyz;
		game_state->view_matrix = math::look_at(camera_pos, math::vec3(0.0f), math::VEC3_UP);
		math::Mat4 view_projection_matrix = game_state->projection_matrix * game_state->view_matrix;

		glUseProgram(game_state->particle_program_id); {
			if(game_state->running_particle_sim) {
				static float m_t = 0.0f;
				m_t = math::lerp(m_t, math::min(math::length(game_state->mouse_delta) * 0.002f, 0.04f), game_state->delta_time * 8.0f);

				static float t = 0.0f;
				t += m_t;
				t += game_state->delta_time * 0.2f;
				// t += game_state->delta_time;
				float dt = game_state->delta_time * 56.0f;

				for(uint32_t i = 0; i < game_state->particle_array_length; i++) {
					Particle * particle = game_state->particle_array + i;
					math::Vec3 pos = get_particle_pos(particle, t, dt);

					uint32_t v_idx = i * game_state->particle_vert_length;

					game_state->particle_vert_array[v_idx +  0] = math::lerp(game_state->particle_vert_array[v_idx +  0], game_state->particle_vert_array[v_idx +  3], dt);
					game_state->particle_vert_array[v_idx +  1] = math::lerp(game_state->particle_vert_array[v_idx +  1], game_state->particle_vert_array[v_idx +  4], dt);
					game_state->particle_vert_array[v_idx +  2] = math::lerp(game_state->particle_vert_array[v_idx +  2], game_state->particle_vert_array[v_idx +  5], dt);

					game_state->particle_vert_array[v_idx +  3] = math::lerp(game_state->particle_vert_array[v_idx +  3], game_state->particle_vert_array[v_idx +  9], dt);
					game_state->particle_vert_array[v_idx +  4] = math::lerp(game_state->particle_vert_array[v_idx +  4], game_state->particle_vert_array[v_idx + 10], dt);
					game_state->particle_vert_array[v_idx +  5] = math::lerp(game_state->particle_vert_array[v_idx +  5], game_state->particle_vert_array[v_idx + 11], dt);

					game_state->particle_vert_array[v_idx +  6] = game_state->particle_vert_array[v_idx +  3];
					game_state->particle_vert_array[v_idx +  7] = game_state->particle_vert_array[v_idx +  4];
					game_state->particle_vert_array[v_idx +  8] = game_state->particle_vert_array[v_idx +  5];

					game_state->particle_vert_array[v_idx +  9] = pos.v[0];
					game_state->particle_vert_array[v_idx + 10] = pos.v[1];
					game_state->particle_vert_array[v_idx + 11] = pos.v[2];
				}				
			}

			uint32_t xform_id = glGetUniformLocation(game_state->particle_program_id, "xform");
			glUniformMatrix4fv(xform_id, 1, GL_FALSE, view_projection_matrix.v);
			
			glBindBuffer(GL_ARRAY_BUFFER, game_state->particle_vertex_buffer.id);
			glBufferSubData(GL_ARRAY_BUFFER, 0, game_state->particle_vertex_buffer.size_in_bytes, game_state->particle_vert_array);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

			glDrawArrays(GL_LINES, 0, game_state->particle_vertex_buffer.vert_count);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(game_state->quad_program_id); {
			math::Mat4 inverse_view_projection = math::inverse(view_projection_matrix); 

			math::Vec2 screen_pos = math::vec2(game_state->mouse_pos.x / game_state->back_buffer_width, game_state->mouse_pos.y / game_state->back_buffer_height) * 2.0f - 1.0f;
			math::Vec4 image_point = inverse_view_projection * math::vec4(screen_pos, 0.0f, 1.0f);
			// std::printf("LOG: (%f, %f)\n", screen_pos.x, screen_pos.y);

			math::Ray camera_ray = math::ray(camera_pos, (image_point.xyz / image_point.w) - camera_pos);

			math::Vec3 pos = math::vec3(0.0f);
			float t = math::ray_plane_intersection(camera_ray, math::vec3(0.0f, 1.0f, 0.0f), 0.0f);
			if(t > 0.0f) {
				pos = camera_ray.o + camera_ray.d * t;
			}

			math::Mat4 world_matrix = math::translate(pos) * math::rotate_around_x(math::PI * 0.5f) * math::scale(0.2f);
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

		glUseProgram(game_state->sphere_program_id); {
			math::Mat4 world_matrix = math::rotate_around_y(game_state->total_time * 0.08f);
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

			// glDrawArrays(GL_TRIANGLES, 0, game_state->sphere_vertex_buffer.vert_count);
		}
	}
}