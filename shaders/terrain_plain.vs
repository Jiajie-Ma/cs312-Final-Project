#version 400

layout (location = 0) in vec3 vPos;

out vec3 vPos3D;
out vec2 vGradient;
out vec4 vpoint_mv;
out vec3 light_dir, view_dir;

uniform float gridlength, gridwidth, time, H, f, density, height;
uniform int lRes, wRes, window_w, window_h, mode, octaves;
uniform mat4 MVP, light_MVP, MV;
uniform mat3 NormalMatrix;
uniform vec3 cam_pos, light_pos;

float noise_hash(vec2 p)
{
   vec3 p3 = fract(vec3(p.xyx) * 0.13); 
   p3 += dot(p3, p3.yzx + 3.333); 
   return fract((p3.x + p3.y) * p3.z);
}

vec3 value_noised(vec2 p, int mode)
{
   vec2 i = floor(p);
   vec2 f = fract(p);
   vec2 u;
   vec2 du;

   if (mode == 1){
      // quintic interpolation
      u = f*f*f*(f*(f*6.0f-15.0f)+10.0f);
      du = 30.0f*f*f*(f*(f-2.0f)+1.0f);
   }
   else{
      // cubic interpolation
      u = f*f*(3.0f-2.0f*f);
      du = 6.0f*f*(1.0f-f);
   }  
    
    float va = noise_hash( i + vec2(0.0,0.0) );
    float vb = noise_hash( i + vec2(1.0,0.0) );
    float vc = noise_hash( i + vec2(0.0,1.0) );
    float vd = noise_hash( i + vec2(1.0,1.0) );
    
    float k0 = va;
    float k1 = vb - va;
    float k2 = vc - va;
    float k4 = va - vb - vc + vd;

    return vec3( va+(vb-va)*u.x+(vc-va)*u.y+(va-vb-vc+vd)*u.x*u.y, // value
                 du*(vec2(u.y, u.x)*(va-vb-vc+vd) + vec2(vb,vc) - va) );     // derivative                
}

vec3 value_fBM(vec2 p, int mode, float H, float f, int octaves)
{
   // initiation
   float s = pow(2.0f, -H);
   float a = 0.0f;
   float b = 1.0f;
   vec2 d = vec2(0.0f);
   // Rotate to reduce axial bias
   mat2 rot = mat2(cos(0.5f), sin(0.5f),
                    -sin(0.5f), cos(0.5f));
   vec2 shift = vec2(100.0f);

   for (int i = 0; i < octaves; i++)
   {
      vec3 n = value_noised(p, mode);
      a += b*n.x;
      d += b * vec2(n.y, n.z);
      b *= s;
      p = f * rot * p;
      p += shift;
   }

   return vec3(a, d);
}

vec3 getTerrainHeight(vec2 pos, int mode, float H, float f, int octaves, float density, float height){
    vec2 p = pos*density;
    vec3 noise = value_fBM(p, mode, H, f, octaves);
    return vec3((0.5 * noise.x + 0.5)* height, noise.yz * 0.5 * height);
}

vec2 getUV(vec3 pos){
   vec2 p = vec2(0.0f);

   p.x = (pos.x + gridwidth/2.0f)/gridwidth;
   p.y = (pos.y + gridlength/2.0f)/gridlength;

   return p;
}

void main() {
   vec2 uv = getUV(vPos);
   vec3 data = getTerrainHeight(uv, mode, H, f, octaves, density, height) - 15.0f;
   vPos3D = vec3(vPos.x, data.x, vPos.y);
   vGradient = data.yz;
   gl_Position = MVP * vec4(vPos3D, 1.0);
   vpoint_mv = MV * vec4(vPos3D, 1.0);
    gl_Position = MVP * vec4(vPos3D, 1.0);
   light_dir = normalize((vec4(light_pos, 1.0f) - vpoint_mv).xyz);
   view_dir = normalize(-vpoint_mv.xyz);
}
