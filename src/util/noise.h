#pragma once

#include "AGLM.h"

using namespace glm;

inline float noise_hash(glm::vec2 p)
{
   glm::vec3 p3 = glm::fract(glm::vec3(p.x, p.y, p.x) * 0.13f); 
   p3 += glm::dot(p3, glm::vec3(p3.y + 3.333f, p3.z + 3.333f, p.x + 3.333f)); 
   return glm::fract((p3.x + p3.y) * p3.z);
}

inline glm::vec2 classic_hash(int p)
{
   int r = p % 3;
   if (r == 0)
   {
      return glm::vec2(1.0f);
   }
   else if (r == 1)
   {
      return glm::vec2(-1.0f, 1.0f);
   }
   else if (r == 2)
   {
      return glm::vec2(-1.0f);
   }
   else{
      return glm::vec2(1.0f, -1.0f);
   }
}

inline float fade(float t) {
    return t*t*t*(t*(t*6.0f-15.0f)+10.0f);
}

inline glm::vec3 value_noised(glm::vec2 p, int mode)
{
   glm::vec2 i = glm::floor(p);
   glm::vec2 f = glm::fract(p);
   glm::vec2 u;
   glm::vec2 du;

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
    
    float va = noise_hash( i + glm::vec2(0.0,0.0) );
    float vb = noise_hash( i + glm::vec2(1.0,0.0) );
    float vc = noise_hash( i + glm::vec2(0.0,1.0) );
    float vd = noise_hash( i + glm::vec2(1.0,1.0) );
    
    float k0 = va;
    float k1 = vb - va;
    float k2 = vc - va;
    float k4 = va - vb - vc + vd;

    return glm::vec3( va+(vb-va)*u.x+(vc-va)*u.y+(va-vb-vc+vd)*u.x*u.y, // value
                 du*(vec2(u.y, u.x)*(va-vb-vc+vd) + glm::vec2(vb,vc) - va) );     // derivative                
}

inline float classic_perlin(glm::vec2 p)
{
   // permutation table, from: https://adrianb.io/2014/08/09/perlinnoise.html
   int perm[256] = { 151,160,137,91,90,15,131,13,201,95,96,53,194,
    233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

   int x = (int) p.x & 255;
   int y = (int) p.y & 255;

   float xf = x - glm::floor(x);
   float yf = y - glm::floor(y);

   glm::vec2 topRight = glm::vec2(xf-1.0f, yf-1.0f);
   glm::vec2 topLeft = glm::vec2(xf, yf-1.0f);
   glm::vec2 bottomRight = glm::vec2(xf-1.0f, yf);
   glm::vec2 bottomLeft = glm::vec2(xf, yf);
	
	//Select a value in the array for each of the 4 corners
   int valueTopRight = perm[perm[x+1]+y+1];
   int valueTopLeft = perm[perm[x]+y+1];
   int valueBottomRight = perm[perm[x+1]+y];
   int valueBottomLeft = perm[perm[x]+y];
   
   float dotTopRight = glm::dot(topRight, classic_hash(valueTopRight));
   float dotTopLeft = glm::dot(topLeft, classic_hash(valueTopLeft));
   float dotBottomRight = glm::dot(bottomRight, classic_hash(valueBottomRight));
   float dotBottomLeft = glm::dot(bottomLeft, classic_hash(valueBottomLeft));
	
	float u = fade(xf);
	float v = fade(yf);
	
	return glm::mix(
      glm::mix(dotBottomLeft, dotTopLeft, v),
		glm::mix(dotBottomRight, dotTopRight, v), u);
}

inline float fBM(vec2 p, float H = 1.0f, float f = 2.0f, int octaves = 8)
{
   float s = pow(2.0f, -H);
   float a = 0.0f;
   float b = 1.0f;
   // Rotate to reduce axial bias
   glm::mat2 rot = glm::mat2(cos(0.5f), sin(0.5f),
                    -sin(0.5f), cos(0.5f));
   glm::vec2 shift = glm::vec2(100.0f);

   for (int i = 0; i < octaves; i++)
   {
      a += b*classic_perlin(p);
      b *= s;
      p = f * rot * p;
      p += shift;
   }

   return a;
}

inline glm::vec3 value_fBM(vec2 p, int mode = 1, float H = 1.0f, float f = 2.0f, int octaves = 8)
{
   // initiation
   float s = pow(2.0f, -H);
   float a = 0.0f;
   float b = 1.0f;
   glm::vec2 d = vec2(0.0f);
   // Rotate to reduce axial bias
   glm::mat2 rot = glm::mat2(cos(0.5f), sin(0.5f),
                    -sin(0.5f), cos(0.5f));
   glm::vec2 shift = glm::vec2(100.0f);

   for (int i = 0; i < octaves; i++)
   {
      vec3 n = value_noised(p, mode);
      a += b*n.x;
      d += b * vec2(n.y, n.z);
      b *= s;
      p = f * rot * p;
      p += shift;
   }

   return glm::vec3(a, d);
}

vec3 getTerrainHeight(vec2 pos, int mode, float H, float f, int octaves, float density, float height){
    vec2 p = pos*density;
    vec3 noise = value_fBM(p, mode, H, f, octaves);
    return vec3((0.5f * noise.x + 0.5f)* height, vec2(noise.y,noise.z) * 0.5f * height);
}

vec2 getUV(vec3 pos, float gridwidth, float gridlength){
   vec2 p = vec2(0.0f);

   p.x = (pos.x + gridwidth/2.0f)/gridwidth;
   p.y = (pos.y + gridlength/2.0f)/gridlength;

   return p;
}

float gridPos(vec3 pos, float mode, float H, float f, int octaves, float density, float height, float gridwidth, float gridlength){
   vec2 uv = getUV(pos, gridwidth, gridlength);
   float h = getTerrainHeight(uv, mode, H, f, octaves, density, height).x;
   return h;
}