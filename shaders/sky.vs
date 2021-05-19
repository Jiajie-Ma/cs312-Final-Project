#version 400

layout (location = 1) in vec3 vpoint;
layout (location = 2) in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

void main(){
    gl_Position = MVP * vec4(100*vpoint,1);
    uv = vtexcoord;
}