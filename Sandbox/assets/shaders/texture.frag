#version 450 core
			
#extension GL_ARB_bindless_texture : enable

out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;


uniform vec4 u_Color;

layout(binding = 0, bindless_sampler) uniform sampler2D u_Texture;



void main()
{
	color = vec4(v_TexCoord, 0.0f, 1.0f);
	color = texture(u_Texture, v_TexCoord);
}