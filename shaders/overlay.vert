#version 450

layout(location = 0) in vec2 posIn;
layout(location = 1) in vec4 colorIn;
layout(location = 2) in vec2 UvIn;
layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;



layout(push_constant) uniform constant {
    vec4 shift_size;
    mat4 transform;
} pco;

void main() {
    vec2 shift = pco.shift_size.xy;
    vec2 scale = pco.shift_size.zw;

    vec4 pos_in_pixels = vec4(posIn + shift, 0, 1);
    // vec4 pix_pos_transformed = pco.transform * pos_in_pixels;
    vec4 pix_pos_transformed = pos_in_pixels;
    vec2 pos = pix_pos_transformed.xy / scale;
    
    
    vec2 clip_pos = pos.xy*2.0 - 1.0;

    // frag_pos = ; //todo
    gl_Position = vec4(clip_pos,0,1);

    fragUV = UvIn;
    fragColor = colorIn;
    // fragUV = positions[gl_VertexIndex].xy;
    // fragColor = colors[gl_VertexIndex];
    // fragColor.x = sin(colors[gl_VertexIndex].y);
    // fragColor.y = sin(colors[gl_VertexIndex].x);
}