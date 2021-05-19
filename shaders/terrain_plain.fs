#version 400

layout(location = 0) out vec4 FragColor;

in vec3 vPos3D;
in vec2 vGradient;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

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
   vec3 vpoint_mv_3 = vpoint_mv.xyz;
   vec3 t_x = normalize(dFdx(vpoint_mv_3));
   vec3 t_y = normalize(dFdy(vpoint_mv_3));
   vec3 triangle_normal = cross(t_x,t_y);
   vec3 reflect_dir_mv = normalize(reflect(-light_dir, triangle_normal));
   float diffuse_angle = max(dot(triangle_normal,light_dir),0);
   vec3 diffuse = vec3(1.0)*diffuse_angle;

   FragColor = vec4(diffuse, 1.0);
}