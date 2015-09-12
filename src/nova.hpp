
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

		uint32_t quad_program_id;
		gl::VertexBuffer quad_vertex_buffer;

		gl::FrameBuffer msaa_frame_buffer;
		gl::FrameBuffer resolve_frame_buffer;

		uint32_t threshold_program_id;
		gl::FrameBuffer threshold_frame_buffer;

		uint32_t blur_program_id;
		gl::FrameBuffer blur_frame_buffers[2];

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

		bool key_space_pressed;
		bool key_rgt_mouse_pressed;
		bool key_lft_mouse_down;
		bool key_rgt_mouse_down;

		bool started;
	};
}

#endif