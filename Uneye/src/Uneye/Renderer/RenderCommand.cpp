#include "uypch.h"
#include "Uneye/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"


namespace Uneye
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;



}