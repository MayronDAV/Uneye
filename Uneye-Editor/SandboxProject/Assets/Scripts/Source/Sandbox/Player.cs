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

		public void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			bool hasTransform = HasComponent<TransformComponent>();

			Console.WriteLine($"Has Transform {hasTransform}");

			m_Transform = GetComponent<TransformComponent>();
			m_Rigidbody = GetComponent<Rigidbody2DComponent>();

		}

		public void OnUpdate(float ts)
		{
			//Console.WriteLine($"Player.OnUpdate: {ts}");
			//Console.WriteLine($"Player.OnUpdate: {ID}");

			Vector3 Direction = Vector3.Zero;

			Vector3 Speed = new Vector3(2.0f * ts);

			if (Input.IsKeyDown(KeyCode.W))
				Direction.Y = 1;

			if (Input.IsKeyDown(KeyCode.A))
				Direction.X = -1;

			if (Input.IsKeyDown(KeyCode.S))
				Direction.Y = -1;

			if (Input.IsKeyDown(KeyCode.D))
				Direction.X = 1;


			//m_Rigidbody.ApplyLinearImpulse(Direction.XY * Speed.XY, true);

			// R = P + D * L

			Vector3 translation = m_Transform.Translation;
			translation += Direction * Speed;
			m_Transform.Translation = translation;

			Wraper.player_position = m_Transform.Translation;
		}

	}
}
