#version 400

layout (location = 0) in vec3 vPos;

out vec3 vPos3D;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

out vec4 tePosition;
out vec4 tePosMVP;
out vec3 tePatchDistance;
out vec2 uv;
out float height;
out float dist;

uniform float gridlength, gridwidth, time, max_height;
uniform int lRes, wRes, window_w, window_h;
uniform mat4 MVP, light_MVP, MV;
uniform mat3 NormalMatrix;
uniform vec3 cam_pos, light_pos;

uniform sampler2D tex_noise;

vec2 getUV(vec2 pos){
   vec2 p = vec2(0.0f);

   p.x = (pos.x + gridwidth/2.0f)/gridwidth;
   p.y = (pos.y + gridlength/2.0f)/gridlength;

   return p;
}


void main() {
   uv = getUV(vPos.xy);
   height = texture(tex_noise, uv).x * max_height;
   if (height <= 2.0f){
       height = 11.0f;
   }
   height = height - 11.0f;
   vPos3D = vec3(vPos.x, height, vPos.y);
   gl_Position = MVP * vec4(vPos3D, 1.0);
   vpoint_mv = MV * vec4(vPos3D, 1.0);
   dist = vpoint_mv.z;
   light_dir = normalize((vec4(light_pos, 1.0f) - vpoint_mv).xyz);
   view_dir = normalize(-vpoint_mv.xyz);
}
