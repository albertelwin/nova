
#ifndef NAMESPACE_NOVA_INCLUDED
#define NAMESPACE_NOVA_INCLUDED

#include <math.hpp>

namespace nova {
	struct Particle {
		math::Vec3 position;
		math::Vec3 axis;
	};

	struct GameState {
		uint32_t sphere_program_id;
		gl::VertexBuffer sphere_vertex_buffer;

		uint32_t quad_program_id;
		gl::VertexBuffer quad_vertex_buffer;

		uint32_t particle_program_id;
		gl::VertexBuffer particle_vertex_buffer;
		uint32_t particle_vert_length;
		float * particle_vert_array;
		uint32_t particle_vert_array_length;

		Particle * particle_array;
		uint32_t particle_array_length;

		float back_buffer_width;
		float back_buffer_height;

		bool running_particle_sim;
		float camera_pos;

		math::Mat4 view_matrix;
		math::Mat4 projection_matrix;

		float total_time;
		float delta_time;

		math::Vec2 mouse_pos;
		math::Vec2 mouse_delta;

		bool key_space_pressed;

		bool started;
	};
}

#endif