#version 330 core

out vec4 fragColor;

void main(void)
{
	// Set the color of this fragment to the interpolated color
	// value computed by the rasterizer.

	fragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
