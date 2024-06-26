#version 450

// layout(location = 0) in vec3 fragColor;
layout(location = 0) in vec2 fragUV;  

layout(set = 0, binding = 0) uniform sampler2D frame;

layout(location = 0) out vec4 outColor;

const float COLOR_ENCODE_VALUE = 5.0;
vec3 decode_color(vec3 encoded_color){
    return clamp(encoded_color,0,1)*vec3(COLOR_ENCODE_VALUE);
}
vec3 encode_color(vec3 color){
    return clamp(color/vec3(COLOR_ENCODE_VALUE), 0,1);
}
void main() {
    // outColor = vec4(fragColor, 1.0);
    // vec2 uv = fragUV - vec2(0.5);
    vec2 vxx = (fragUV / 2 + vec2(0.5)).xx; 
    vec2 uv = fragUV / 2 + vec2(0.5);
    uv.x = (vxx.yy).y;
    // vec4 sampledColor = texture(frame, uv); 
    vec3 final_color = decode_color(texture(frame, uv).rgb);
    outColor = vec4(final_color,1);
    // outColor = vec4(uv, 0.0, 1.0);
} 