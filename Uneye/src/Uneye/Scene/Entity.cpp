#include "uypch.h"
#include "Uneye/Scene/Entity.h"




namespace Uneye
{
	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityHandle(handle), m_Scene(scene)
	{
	}
}
