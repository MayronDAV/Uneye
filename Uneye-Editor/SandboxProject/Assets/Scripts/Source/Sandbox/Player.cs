using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Uneye;



namespace Sandbox
{
	public static class Wraper
	{
		public static Vector3 player_position;
	}

	public class Player : Entity
	{

		private TransformComponent m_Transform;
		private Rigidbody2DComponent m_Rigidbody;

		public static Vector3 Position;

		public float f_speed;

		private Camera m_Camera;

		public void OnCreate()
		{
			m_Transform = GetComponent<TransformComponent>();
			m_Rigidbody = GetComponent<Rigidbody2DComponent>();
			//f_speed = 2.0f;
		}

		public void OnUpdate(float ts)
		{
			Vector3 Direction = Vector3.Zero;

			Vector3 Speed = new Vector3(f_speed * ts);

			if (Input.IsKeyDown(KeyCode.W))
				Direction.Y = 1;

			if (Input.IsKeyDown(KeyCode.A))
				Direction.X = -1;

			if (Input.IsKeyDown(KeyCode.S))
				Direction.Y = -1;

			if (Input.IsKeyDown(KeyCode.D))
				Direction.X = 1;

			m_Camera = FindFirstEntityByName("Camera").As<Camera>();
			if (m_Camera != null)
			{
				if(Input.IsKeyDown(KeyCode.Q))
					m_Camera.DistanceFromPlayer -= 3.0f * ts;

				if(Input.IsKeyDown(KeyCode.E))
					m_Camera.DistanceFromPlayer += 3.0f * ts;
			}



			// R = P + D * L

			Position = m_Transform.Translation;
			Position += Direction * Speed;
			m_Transform.Translation = Position;

			//m_Rigidbody.ApplyLinearImpulse(Direction.XY * Speed.XY, true);
			//m_Rigidbody.SetTransform(Position.XY, 0);
		}

	}
}
