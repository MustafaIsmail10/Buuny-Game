#version 330 core

uniform float offset;
uniform float xoffset = 10;
uniform float yoffset = 0;
out vec4 fragColor;
in vec4 pos;

void main(void) {

	int x = int((pos.x + xoffset) * 1.2) % 2;

	int y = int((pos.y + yoffset)) % 2;

	bool z = bool(int((pos.z + offset)) % 2);

	bool xorXY = x != y;

	vec4 black = vec4(0, 0, 0, 1);
	vec4 white = vec4(.5, .5, .8, 1);

	if(xorXY != z) {
		fragColor = black;
	} else {
		fragColor = white;
	}
}
