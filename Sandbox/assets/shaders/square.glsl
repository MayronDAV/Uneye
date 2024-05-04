@type vertex
#version 330 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;


out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;

uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}


@type fragment
#version 330 core


out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	color = v_Color * texture(u_Textures[int(v_TexIndex)], v_TexCoord);
	//color = v_Color;
}