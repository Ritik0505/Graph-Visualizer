#version 330 core
layout (location = 0) in vec2 position;
uniform float posX;
uniform float posY;

uniform mat4 projection;
uniform mat4 translate;
uniform mat4 scale;

void main()
{
	gl_Position = translate * scale *  vec4(position.x ,position.y ,0.0f ,10.0f );
}