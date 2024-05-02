@type vertex
#version 430 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_ViewProjection;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}


@type fragment
#version 430 core

//#extension GL_ARB_bindless_texture : enable
			
out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform vec4 u_Color;
//layout(binding = 0, bindless_sampler) uniform sampler2D u_Texture;

void main()
{
	//color = u_Color * texture(u_Texture, v_TexCoord);
	color = v_Color;
}