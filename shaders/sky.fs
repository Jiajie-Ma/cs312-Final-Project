#version 400

in vec2 uv;

out vec3 color;

uniform sampler2D tex_day, tex_night;
uniform vec3 light_pos;
uniform float time;

void main(){
    float sunHeight = light_pos.z/100.0;
    vec3 day = texture(tex_day, uv).rgb;
    vec3 night = texture(tex_night, uv).rgb;
    float coef = smoothstep(-0.5, 0.0 ,sunHeight);
    color = mix(night, day, coef);
}