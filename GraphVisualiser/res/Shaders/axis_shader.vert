#version 330 core
layout (location = 0) in vec2 position;
uniform float offset_x;
uniform float offset_y;

uniform mat4 scale;
uniform mat4 projection;

void main()
{
	if((position.x == -1.0f && position.y == 0.0f)
		||	(position.x == 1.0f && position.y == 0.0f))
			gl_Position = vec4(position.x , position.y + offset_y, 0.0f, 1.0f);
	else
		gl_Position = vec4(position.x + offset_x , position.y, 0.0f, 1.0f);
}
