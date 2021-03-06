
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

	struct Camera {
		float angle;
		float dist;
		float timer;
	};

	struct GameState {
		uint32_t sphere_program_id;
		gl::VertexBuffer sphere_vertex_buffer;

		uint32_t disc_program_id;
		gl::VertexBuffer disc_vertex_buffer;

		uint32_t particle_program_id;
		uint32_t particle_vert_length;
		ParticleArray disc_particles;

		uint32_t back_buffer_width;
		uint32_t back_buffer_height;

		float protostar_mass;
		float protostar_size;
		uint32_t particles_consumed;
		float particles_consumed_threshold;
		float particle_mass_consumed;

		Camera camera;
		Camera initial_camera;

		math::Mat4 view_matrix;
		math::Mat4 projection_matrix;

		float total_time;
		float delta_time;

		math::Vec2 mouse_pos;
		math::Vec2 mouse_delta;

		bool8_t left_mouse_key_down;
		bool8_t right_mouse_key_down;

		bool started;
	};
}

#endif