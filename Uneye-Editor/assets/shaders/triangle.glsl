@type triangle
#version 330 core
							
layout(location = 0) in vec3 a_Position;

out vec3 v_pos;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	v_pos = a_Position;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
}

@type fragment
#version 330 core
			
out vec4 color;

in vec3 v_pos;
uniform vec4 u_Color;

void main()
{
	color = vec4(v_pos * 0.5f + 0.5f, 1.0f);
	color = u_Color;
}