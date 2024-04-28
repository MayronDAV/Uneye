#version 330 core
			
out vec4 color;

in vec3 v_pos;
uniform vec4 u_Color;

void main()
{
	color = vec4(v_pos * 0.5f + 0.5f, 1.0f);
	color = u_Color;
}