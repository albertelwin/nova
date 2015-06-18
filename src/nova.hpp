
#ifndef NAMESPACE_NOVA_INCLUDED
#define NAMESPACE_NOVA_INCLUDED

#include <math.hpp>

namespace nova {
	struct Particle {
		math::Vec3 position;
		math::Vec3 velocity;
		float mass;
	};

	struct ParticleArray {
		Particle * v;
		uint32_t length;

		gl::VertexBuffer vertex_buffer;
		float * verts;
		uint32_t verts_length;
	};

	struct GameState {
		uint32_t sphere_program_id;
		gl::VertexBuffer sphere_vertex_buffer;

		uint32_t quad_program_id;
		gl::VertexBuffer quad_vertex_buffer;

		uint32_t particle_program_id;
		uint32_t particle_vert_length;

		ParticleArray cloud_particles;
		ParticleArray disc_particles;

		float back_buffer_width;
		float back_buffer_height;

		float protostar_mass;

		bool running_particle_sim;
		float camera_pos;

		math::Mat4 view_matrix;
		math::Mat4 projection_matrix;

		float total_time;
		float delta_time;

		math::Vec2 mouse_pos;
		math::Vec2 mouse_delta;

		bool key_space_pressed;
		bool key_mouse_down;

		bool started;
	};
}

#endif