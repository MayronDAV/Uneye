@type vertex
#version 450 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
}


@type fragment
#version 450 core

#extension GL_ARB_bindless_texture : enable
			
out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color;
layout(binding = 0, bindless_sampler) uniform sampler2D u_Texture;

void main()
{
	color = u_Color * texture(u_Texture, v_TexCoord);
}