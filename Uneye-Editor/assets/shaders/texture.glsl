// Basic shader type

@type vertex 
#version 450 core
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec3 v_Position;
out vec2 v_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

void main()
{
	v_Position = a_Position;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_ModelMatrix * vec4(a_Position, 1.0f);
}



@type fragment
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