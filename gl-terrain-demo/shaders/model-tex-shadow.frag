#version 330 core

const int CSM_LEVELS = 4;

/* Inputs */
in vec4 vs_pos;
in vec3 vs_normal;
in vec2 vs_uv;
flat in int vs_mat_idx;
flat in int vs_sampler_index;
in vec4 vs_shadow_map_uv[CSM_LEVELS];
in float vs_dist_from_camera;
in float vs_visibility;
in vec4 vs_clip_pos;
in vec4 vs_prev_clip_pos;

/* Uniforms */
uniform mat4 ViewInv;

uniform vec4 LightPosition;
uniform float LightAttenuation;
uniform vec3 LightDiffuse;
uniform vec3 LightAmbient;
uniform vec3 LightSpecular;

uniform vec3 MaterialDiffuse[8];
uniform vec3 MaterialAmbient[8];
uniform vec3 MaterialSpecular[8];
uniform float MaterialShininess[8];

uniform sampler2D TexDiffuse[4];

uniform sampler2DShadow ShadowMap[CSM_LEVELS];
uniform float ShadowCSMEndClipSpace[CSM_LEVELS];
uniform float ShadowMapSize[CSM_LEVELS];
uniform int ShadowCSMLevels;
uniform int ShadowPFC;
const float ShadowAcneBias = 0.002;

uniform float NearPlane;
uniform float FarClipPlane;
uniform float FarRenderPlane;

uniform vec3 SkyColor;

/* Outputs */
out vec4 fs_color;
out vec4 fs_motion_vector;

float sample_shadow_map(int level, vec3 shadow_coords)
{
   /* Can't use non-uniform expressions with sampler arrays :-( */
   if (level == 0)
      return texture(ShadowMap[0], shadow_coords);
   else if (level == 1)
      return texture(ShadowMap[1], shadow_coords);
   else if (level == 2)
      return texture(ShadowMap[2], shadow_coords);

   return texture(ShadowMap[3], shadow_coords);
}

float compute_shadow_factor(float dp)
{
   for (int level = 0; level < ShadowCSMLevels; level++) {
      if (vs_dist_from_camera <= ShadowCSMEndClipSpace[level]) {
         float kernel_size = ShadowPFC * 2.0 + 1.0;
         float num_samples =  kernel_size * kernel_size;
         float texel_size = 1.0 / ShadowMapSize[level];
         float shadowed_texels = 0.0;
         float bias = ShadowAcneBias * tan(acos(dp));
         float ref_dist = vs_shadow_map_uv[level].z - bias;

         for (int x = -ShadowPFC; x <= ShadowPFC; x++) {
            for (int y = -ShadowPFC; y <= ShadowPFC; y++) {
               vec3 shadow_coords =
                  vec3(vs_shadow_map_uv[level].xy + vec2(x, y) * texel_size, ref_dist);
               shadowed_texels += sample_shadow_map(level, shadow_coords);
            }
         }

         return 1.0 - shadowed_texels / num_samples * vs_shadow_map_uv[level].w;
      }
   }

   /* Fragment outside shadow map, no shadowing */
   return 1.0;
}

void main()
{
   vec3 light_dir;
   float attenuation;

   /* Light direction and attenuation factor */
   if (LightPosition.w == 0.0f) {
      /* Directional light */
      light_dir = normalize(vec3(LightPosition));
      attenuation = 1.0f;
   } else {
      /* Positional light */
      vec3 pos_to_light = vec3(LightPosition - vs_pos);
      float distance = length(pos_to_light);
      light_dir = normalize(pos_to_light);
      attenuation = 1.0 / (LightAttenuation * distance);
   }

   vec3 normal = normalize(vs_normal);
   float dp = dot(normal, light_dir);

   /* Is this pixel in the shade? Take mutiple samples to soften shadow edges */
   float shadow_factor = compute_shadow_factor(dp);

   vec3 vs_ambient = MaterialAmbient[vs_mat_idx];
   vec3 vs_diffuse = MaterialDiffuse[vs_mat_idx];
   vec3 vs_specular = MaterialSpecular[vs_mat_idx];
   float vs_shininess = MaterialShininess[vs_mat_idx];

   /* Diffuse */
   /* GLSL only permits indexing into sampler arrays with constants... */
   vec3 sampler_diffuse;
   if (vs_sampler_index == 0) {
      sampler_diffuse = vec3(texture(TexDiffuse[0], vs_uv));
   } else if (vs_sampler_index == 1) {
      sampler_diffuse = vec3(texture(TexDiffuse[1], vs_uv));
   } else if (vs_sampler_index == 2) {
      sampler_diffuse = vec3(texture(TexDiffuse[2], vs_uv));
   } else {
      sampler_diffuse = vec3(texture(TexDiffuse[3], vs_uv));
   }
   vec3 base_diffuse = sampler_diffuse + vs_diffuse;
   vec3 diffuse = attenuation * LightDiffuse * base_diffuse *
      max(0.0, dp) * shadow_factor;

   /* Ambient */
   vec3 ambient = LightAmbient * (base_diffuse + vs_ambient);

   /* Specular */
   vec3 specular;
   if (vs_shininess == 0.0 || dp < 0.0) {
      specular = vec3(0.0, 0.0, 0.0);
   } else {
      vec3 view_dir =
         normalize(vec3(ViewInv * vec4(0.0, 0.0, 0.0, 1.0) - vs_pos));
      vec3 reflection_dir = reflect(-light_dir, normal);
      float shine_factor = dot(reflection_dir, view_dir);
      specular =
         attenuation * LightSpecular * vs_specular *
            pow(max(0.0, shine_factor), vs_shininess) * (shadow_factor / 1.0);
   }

   /* Alpha (objects close to the far clipping plane fade-in progressively) */
   float near = NearPlane;
   float far = FarRenderPlane;
   float fade_dist = 15.0;
   float depth =
      2.0 * near * far / (far + near - (2.0 * gl_FragCoord.z - 1.0) * (far - near));
   float alpha = 1.0 - clamp((depth - (FarClipPlane - fade_dist)) / fade_dist, 0.0, 1.0);

   vec3 light_color = diffuse + ambient + specular;
   vec3 final_color = mix(SkyColor, light_color, vs_visibility);
   fs_color = vec4(final_color, alpha);

   /* Motion vector (0.5 means no motion) */
   vec3 ndc_pos = (vs_clip_pos / vs_clip_pos.w).xyz;
   vec3 prev_ndc_pos = (vs_prev_clip_pos / vs_prev_clip_pos.w).xyz;
   fs_motion_vector = vec4((ndc_pos - prev_ndc_pos).xy + 0.5, 0, 1);
}
