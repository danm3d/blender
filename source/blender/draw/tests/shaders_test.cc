/* Apache License, Version 2.0 */

#include "testing/testing.h"

#include "intern/draw_manager_testing.h"

#include "GPU_context.h"
#include "GPU_init_exit.h"
#include "GPU_shader.h"

#include "GHOST_C-api.h"

#include "engines/eevee/eevee_private.h"
#include "engines/gpencil/gpencil_engine.h"
#include "engines/overlay/overlay_private.h"
#include "engines/workbench/workbench_private.h"

/* Base class for draw test cases. It will setup and tear down the GPU part around each test. */
class DrawTest : public ::testing::Test {
 private:
  GHOST_SystemHandle ghost_system;
  GHOST_ContextHandle ghost_context;
  GPUContext *context;

  void SetUp() override
  {
    GHOST_GLSettings glSettings = {0};
    ghost_system = GHOST_CreateSystem();
    ghost_context = GHOST_CreateOpenGLContext(ghost_system, glSettings);
    context = GPU_context_create(0);
    GPU_init();
    DRW_draw_state_init_gtests(GPU_SHADER_CFG_DEFAULT);
  }

  void TearDown() override
  {
    GPU_exit();
    GPU_context_discard(context);
    GHOST_DisposeOpenGLContext(ghost_system, ghost_context);
    GHOST_DisposeSystem(ghost_system);
  }
};

TEST_F(DrawTest, workbench_glsl_shaders)
{
  workbench_shader_library_ensure();
  DRW_draw_state_init_gtests(GPU_SHADER_CFG_DEFAULT);

  const int MAX_WPD = 6;
  WORKBENCH_PrivateData wpds[MAX_WPD];

  wpds[0].sh_cfg = GPU_SHADER_CFG_DEFAULT;
  wpds[0].shading.light = V3D_LIGHTING_FLAT;
  wpds[1].sh_cfg = GPU_SHADER_CFG_DEFAULT;
  wpds[1].shading.light = V3D_LIGHTING_MATCAP;
  wpds[2].sh_cfg = GPU_SHADER_CFG_DEFAULT;
  wpds[2].shading.light = V3D_LIGHTING_STUDIO;
  wpds[3].sh_cfg = GPU_SHADER_CFG_CLIPPED;
  wpds[3].shading.light = V3D_LIGHTING_FLAT;
  wpds[4].sh_cfg = GPU_SHADER_CFG_CLIPPED;
  wpds[4].shading.light = V3D_LIGHTING_MATCAP;
  wpds[5].sh_cfg = GPU_SHADER_CFG_CLIPPED;
  wpds[5].shading.light = V3D_LIGHTING_STUDIO;

  for (int wpd_index = 0; wpd_index < MAX_WPD; wpd_index++) {
    WORKBENCH_PrivateData *wpd = &wpds[wpd_index];
    EXPECT_NE(workbench_shader_opaque_get(wpd, WORKBENCH_DATATYPE_MESH), nullptr);
    EXPECT_NE(workbench_shader_opaque_get(wpd, WORKBENCH_DATATYPE_HAIR), nullptr);
    EXPECT_NE(workbench_shader_opaque_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD), nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_MESH, false), nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_MESH, true), nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_HAIR, false), nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_HAIR, true), nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD, false),
              nullptr);
    EXPECT_NE(workbench_shader_opaque_image_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD, true),
              nullptr);
    EXPECT_NE(workbench_shader_composite_get(wpd), nullptr);
    EXPECT_NE(workbench_shader_merge_infront_get(wpd), nullptr);

    EXPECT_NE(workbench_shader_transparent_get(wpd, WORKBENCH_DATATYPE_MESH), nullptr);
    EXPECT_NE(workbench_shader_transparent_get(wpd, WORKBENCH_DATATYPE_HAIR), nullptr);
    EXPECT_NE(workbench_shader_transparent_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD), nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_MESH, false),
              nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_MESH, true), nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_HAIR, false),
              nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_HAIR, true), nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD, false),
              nullptr);
    EXPECT_NE(workbench_shader_transparent_image_get(wpd, WORKBENCH_DATATYPE_POINTCLOUD, true),
              nullptr);
    EXPECT_NE(workbench_shader_transparent_resolve_get(wpd), nullptr);
  }

  EXPECT_NE(workbench_shader_shadow_pass_get(false), nullptr);
  EXPECT_NE(workbench_shader_shadow_pass_get(true), nullptr);
  EXPECT_NE(workbench_shader_shadow_fail_get(false, false), nullptr);
  EXPECT_NE(workbench_shader_shadow_fail_get(false, true), nullptr);
  EXPECT_NE(workbench_shader_shadow_fail_get(true, false), nullptr);
  EXPECT_NE(workbench_shader_shadow_fail_get(true, true), nullptr);

  /* NOTE: workbench_shader_cavity_get(false, false) isn't a valid option. */
  EXPECT_NE(workbench_shader_cavity_get(false, true), nullptr);
  EXPECT_NE(workbench_shader_cavity_get(true, false), nullptr);
  EXPECT_NE(workbench_shader_cavity_get(true, true), nullptr);
  EXPECT_NE(workbench_shader_outline_get(), nullptr);

  EXPECT_NE(workbench_shader_antialiasing_accumulation_get(), nullptr);
  EXPECT_NE(workbench_shader_antialiasing_get(0), nullptr);
  EXPECT_NE(workbench_shader_antialiasing_get(1), nullptr);
  EXPECT_NE(workbench_shader_antialiasing_get(2), nullptr);

  EXPECT_NE(workbench_shader_volume_get(false, false, false, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, false, false, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, false, true, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, false, true, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, true, false, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, true, false, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, true, true, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(false, true, true, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, false, false, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, false, false, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, false, true, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, false, true, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, true, false, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, true, false, true), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, true, true, false), nullptr);
  EXPECT_NE(workbench_shader_volume_get(true, true, true, true), nullptr);

  GPUShader *dof_prepare_sh;
  GPUShader *dof_downsample_sh;
  GPUShader *dof_blur1_sh;
  GPUShader *dof_blur2_sh;
  GPUShader *dof_resolve_sh;
  workbench_shader_depth_of_field_get(
      &dof_prepare_sh, &dof_downsample_sh, &dof_blur1_sh, &dof_blur2_sh, &dof_resolve_sh);
  EXPECT_NE(dof_prepare_sh, nullptr);
  EXPECT_NE(dof_downsample_sh, nullptr);
  EXPECT_NE(dof_blur1_sh, nullptr);
  EXPECT_NE(dof_blur2_sh, nullptr);
  EXPECT_NE(dof_resolve_sh, nullptr);

  workbench_shader_free();
}

TEST_F(DrawTest, gpencil_glsl_shaders)
{
  EXPECT_NE(GPENCIL_shader_antialiasing(0), nullptr);
  EXPECT_NE(GPENCIL_shader_antialiasing(1), nullptr);
  EXPECT_NE(GPENCIL_shader_antialiasing(2), nullptr);

  EXPECT_NE(GPENCIL_shader_geometry_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_layer_blend_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_mask_invert_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_depth_merge_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_blur_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_colorize_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_composite_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_transform_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_glow_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_pixelize_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_rim_get(), nullptr);
  EXPECT_NE(GPENCIL_shader_fx_shadow_get(), nullptr);

  GPENCIL_shader_free();
}

TEST_F(DrawTest, overlay_glsl_shaders)
{
  for (int i = 0; i < 2; i++) {
    eGPUShaderConfig sh_cfg = i == 0 ? GPU_SHADER_CFG_DEFAULT : GPU_SHADER_CFG_CLIPPED;
    DRW_draw_state_init_gtests(sh_cfg);
    EXPECT_NE(OVERLAY_shader_antialiasing(), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_degrees_of_freedom_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_degrees_of_freedom_solid(), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_envelope(false), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_envelope(true), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_shape(false), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_shape(true), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_shape_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_sphere(false), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_sphere(true), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_stick(), nullptr);
    EXPECT_NE(OVERLAY_shader_armature_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_background(), nullptr);
    EXPECT_NE(OVERLAY_shader_clipbound(), nullptr);
    EXPECT_NE(OVERLAY_shader_depth_only(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_curve_handle(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_curve_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_curve_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_gpencil_guide_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_gpencil_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_gpencil_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_lattice_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_lattice_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_analysis(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_edge(false), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_edge(true), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_face(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_facedot(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_normal(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_skin_root(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_mesh_vert(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_particle_strand(), nullptr);
    EXPECT_NE(OVERLAY_shader_edit_particle_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_extra(false), nullptr);
    EXPECT_NE(OVERLAY_shader_extra(true), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_groundline(), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_wire(false, false), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_wire(false, true), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_wire(true, false), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_wire(true, true), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_loose_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_facing(), nullptr);
    EXPECT_NE(OVERLAY_shader_gpencil_canvas(), nullptr);
    EXPECT_NE(OVERLAY_shader_grid(), nullptr);
    EXPECT_NE(OVERLAY_shader_image(), nullptr);
    EXPECT_NE(OVERLAY_shader_motion_path_line(), nullptr);
    EXPECT_NE(OVERLAY_shader_motion_path_vert(), nullptr);
    EXPECT_NE(OVERLAY_shader_uniform_color(), nullptr);
    EXPECT_NE(OVERLAY_shader_outline_prepass(false), nullptr);
    EXPECT_NE(OVERLAY_shader_outline_prepass(true), nullptr);
    EXPECT_NE(OVERLAY_shader_outline_prepass_gpencil(), nullptr);
    EXPECT_NE(OVERLAY_shader_outline_prepass_pointcloud(), nullptr);
    EXPECT_NE(OVERLAY_shader_extra_grid(), nullptr);
    EXPECT_NE(OVERLAY_shader_outline_detect(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_face(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_point(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_texture(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_vertcol(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_weight(), nullptr);
    EXPECT_NE(OVERLAY_shader_paint_wire(), nullptr);
    EXPECT_NE(OVERLAY_shader_particle_dot(), nullptr);
    EXPECT_NE(OVERLAY_shader_particle_shape(), nullptr);
    EXPECT_NE(OVERLAY_shader_sculpt_mask(), nullptr);
    EXPECT_NE(OVERLAY_shader_volume_velocity(false), nullptr);
    EXPECT_NE(OVERLAY_shader_volume_velocity(true), nullptr);
    EXPECT_NE(OVERLAY_shader_wireframe(false), nullptr);
    EXPECT_NE(OVERLAY_shader_wireframe(true), nullptr);
    EXPECT_NE(OVERLAY_shader_wireframe_select(), nullptr);
    EXPECT_NE(OVERLAY_shader_xray_fade(), nullptr);
  }

  OVERLAY_shader_free();
}

TEST_F(DrawTest, eevee_glsl_shaders_static)
{
  EEVEE_shaders_lightprobe_shaders_init();
  EEVEE_shaders_material_shaders_init();

  EXPECT_NE(EEVEE_shaders_bloom_blit_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_blit_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_downsample_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_downsample_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_upsample_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_upsample_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_resolve_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_bloom_resolve_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_downsample_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_downsample_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_scatter_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_scatter_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_resolve_get(false), nullptr);
  EXPECT_NE(EEVEE_shaders_depth_of_field_resolve_get(true), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_filter_glossy_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_filter_diffuse_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_filter_visibility_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_grid_fill_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_planar_downsample_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_studiolight_probe_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_studiolight_background_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_cube_display_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_grid_display_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_probe_planar_display_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_update_noise_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_velocity_resolve_sh_get(), nullptr);
  EXPECT_NE(EEVEE_shaders_taa_resolve_sh_get(EFFECT_TAA), nullptr);
  EXPECT_NE(EEVEE_shaders_taa_resolve_sh_get(EFFECT_TAA_REPROJECT), nullptr);

  EEVEE_shaders_free();
}