#include "app.h"
#include "example.h"

#include "example_skybox.h"
#include "example_particle.h"
#include "example_matrices.h"
#include "example_billboard.h"
#include "example_light_maps.h"
#include "example_billboard2.h"
#include "example_shadow_maps.h"
#include "example_motion_blur.h"
#include "example_colored_cube.h"
#include "example_red_triangle.h"
#include "example_vbo_indexing.h"
#include "example_model_loading.h"
#include "example_textured_cube.h"
#include "example_basic_shading.h"
#include "example_ant_tweek_bar.h"
#include "example_normal_mapping.h"
#include "example_defered_shading.h"
#include "example_render_to_texture.h"
#include "example_basic_tessellation.h"
#include "example_keyboard_and_mouse.h"
#include "example_skeletal_animation_with_assimp.h"
#include "example_particle_system_using_transform_feedback.h"

App app;

int main() {
	if (!app.Initialize()) {
		return -1;
	}
	Example* example = new Example_SkyBox;

	app.SetExample(example);

	app.EnterMainLoop();

	delete example;
	app.Destroy();

	return 0;
}