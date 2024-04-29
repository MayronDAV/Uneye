#version 330 core
			
out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;


uniform vec4 u_Color;

void main()
{
	color = vec4(v_TexCoord, 0.0f, 1.0f);

}