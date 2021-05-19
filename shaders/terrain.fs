#version 400

layout(location = 0) out vec4 FragColor;

in vec3 vPos3D;
in vec2 uv;
in float height;
in float dist;
in vec2 vGradient;
in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

uniform mat4 MV, MVP, inv_MV, light_MVP;
uniform vec3 l_a, l_d, l_s;
uniform vec3 light_pos;
uniform sampler2D tex_grass, tex_sand, tex_snow, tex_rock, tex_noise;
uniform float gridlength, gridwidth, time, max_height;
uniform int lRes, wRes, window_w, window_h;

vec3 kd;
float snow_height = 0.8 * 15-11.0f;
float grass_height = 0.5*15 - 11.0f;
float mix_zone = 0.1 * 15;

const float PI = 3.1415926535;

vec2 getUV(vec2 pos){
   vec2 p = vec2(0.0f);

   p.x = (pos.x + gridwidth/2.0f)/gridwidth;
   p.y = (pos.y + gridlength/2.0f)/gridlength;

   return p;
}

vec4 get3Dcoord(vec2 pos_2D){
    vec2 uv = getUV(pos_2D);
    float h = texture(tex_noise, uv).r * max_height ;
    return vec4(pos_2D, h, 1.0);
}

vec3 compute_normal(vec2 gradient){
   vec3 gx = vec3(1.0f, 0.0f, gradient.x);
   vec3 gy = vec3(0.0f, 1.0f, gradient.y);
   vec3 n = cross(gx, gy);
   if (n.z < 0){
      n = -n;
   }
   return n;
}

vec4 compute_normal(vec4 current_pos) {
    float offset = 1.0/100.0;
    vec2 posx    = current_pos.xy + vec2(offset, 0.0);
    vec2 posxneg = current_pos.xy - vec2(offset, 0.0);
    vec2 posy    = current_pos.xy + vec2(0.0, offset);
    vec2 posyneg = current_pos.xy - vec2(0.0, offset);
    vec4 dx = get3Dcoord(posx) - get3Dcoord(posxneg);
    vec4 dy = get3Dcoord(posy) - get3Dcoord(posyneg);
    return vec4(normalize(cross(dx.xyz, dy.xyz)), 1.0);
}

vec3 applyFog(vec3 rgb, float gDistance, vec3 rayDir, vec3 sunDir ){
    float b = 0.02;
    float fogAmount = 1.0 - exp( gDistance*b );
    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = mix(vec3(0.5,0.6,0.7), vec3(1.0,0.9,0.7), pow(sunAmount,8.0));
    return mix( rgb, fogColor, fogAmount );
}

vec3 computeDayColor(){
    vec4 currentNormal = compute_normal(vec4(vPos3D, 1.0));
    vec3 result;
    vec3 v = view_dir;
    vec3 n = normalize(vec3(inv_MV * currentNormal));
    vec3 l = normalize((MV * vec4(light_pos, 1.0)).xyz);
    vec3 r = normalize(reflect(-l, n));

    vec2 current_uv = uv;
    vec3 snow = texture(tex_snow, current_uv*100).rgb;
    vec3 grass = texture(tex_grass, current_uv*30).rgb;
    vec3 sand = texture(tex_sand, current_uv*50).rgb;
    vec3 rock = texture(tex_rock, current_uv*100).rgb;
    vec3 vertical = vec3(0, 0, 1);

    float angleDiff = abs(dot(currentNormal.xyz, vertical));
    float pureRock = 0.3;
    float lerpRock = 0.45;
    float coef = 1.0 - smoothstep(pureRock, lerpRock, angleDiff);
    vec3 rockgrass = mix(grass, rock, coef);
    snow = mix(snow, rock, coef);
    coef = smoothstep(0.90, 0.98, angleDiff);
    vec3 snowgrass = mix(rockgrass, snow, coef);


    if (height > snow_height + mix_zone){
        kd = snow;
    } else if (height > snow_height - mix_zone) {
        float coef = (height-(snow_height - mix_zone))/(2.0 * mix_zone);
        kd = mix(snowgrass, snow, coef);
    } else if (height > grass_height + mix_zone){
        kd = snowgrass;
    } else if ( height > grass_height - mix_zone){
        float coef = (height-(grass_height - mix_zone))/(2.0 * mix_zone);
        kd = mix(sand, rockgrass, coef);
    } else {
        kd = sand;
    }

    vec3 ambiant = kd * l_a;

    float nl = max(0.0, dot(n,l));
    float shadowCoef = 1.0;
    vec3 diffuse = kd * nl * l_d;

    result = vec3(applyFog(kd, dist, -v, l));

    return result;
}

vec3 computeNightColor(){
    vec3 v = view_dir;
    vec3 c = vec3(0.0, 0.2, 0.3);
    vec3 l = normalize((MV * vec4(light_pos, 1.0)).xyz);
    vec3 nightColor = pow(-dist/20.0, 0.6) * c;
    return nightColor;
}

void main() {
   float sunHeight = light_pos.z/100.0;
   vec3 day = computeDayColor();
   vec3 night = computeNightColor();
   float coef = smoothstep(-0.5, 0.0, sunHeight);
   vec3 color = mix(night, day, coef);

   FragColor = vec4(color, 1.0);
}