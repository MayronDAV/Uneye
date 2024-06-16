// Line Shader

@type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in int a_EntityID;
layout(location = 3) in uvec2 a_SceneHandle;


layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;
layout (location = 5) out flat uvec2 v_SceneHandle;

void main()
{
	Output.Color = a_Color;

	v_EntityID = a_EntityID;
	v_SceneHandle = a_SceneHandle;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
}


@type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;
layout(location = 2) out uvec2 o_SceneHandle;

struct VertexOutput
{
	vec4 Color;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;
layout (location = 5) in flat uvec2 v_SceneHandle;


void main()
{
	o_Color = Input.Color;

	o_EntityID = v_EntityID;
	o_SceneHandle = v_SceneHandle;
}
