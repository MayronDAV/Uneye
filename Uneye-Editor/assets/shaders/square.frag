#version 460 core

#extension GL_ARB_bindless_texture : enable
			
out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
layout(binding = 0, bindless_sampler) uniform sampler2D u_Texture;

void main()
{
	color = u_Color * texture(u_Texture, v_TexCoord);
	//color = u_Color;
}
