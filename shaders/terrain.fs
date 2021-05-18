#version 400

layout(location = 0) out vec4 FragColor;

in vec3 vPos3D;
in vec2 vGradient;

uniform vec3 light_pos;

vec3 compute_normal(vec2 gradient){
   vec3 gx = vec3(1.0f, 0.0f, gradient.x);
   vec3 gy = vec3(0.0f, 1.0f, gradient.y);
   vec3 n = cross(gx, gy);
   if (n.z < 0){
      n = -n;
   }
   return n;
}

void main() {
   FragColor = vec4(compute_normal(vGradient), 1.0);
}