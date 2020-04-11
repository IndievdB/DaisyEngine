#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform vec4 color;

void main()
{
	vec4 pixel = color;
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	imageStore(img_output, pixel_coords, pixel);
}