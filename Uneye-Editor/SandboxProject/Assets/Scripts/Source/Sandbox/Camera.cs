using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Uneye;

namespace Sandbox
{
	public class Camera : Entity
	{
		public Entity m_Player;

		public float DistanceFromPlayer = 0.0f;

		public void OnCreate()
		{
			m_Player = FindFirstEntityByName("Player");
			if(m_Player != null)
				Translation = new Vector3(m_Player.Translation.XY, DistanceFromPlayer);
		}

		public void OnUpdate(float ts)
		{
			if (m_Player != null) 
				Translation = new Vector3(m_Player.Translation.XY, DistanceFromPlayer);
		}

	}
}
