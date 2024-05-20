using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Uneye;


namespace Sandbox
{
	public class Player : Entity
	{
		public void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate - {ID}");

			
			//Console.WriteLine($"Translation: {Translation}");
		}

		public void OnUpdate(float ts)
		{
			Console.WriteLine($"Player.OnUpdate: {ts}");
			//Console.WriteLine($"Player.OnUpdate: {ID}");

			Vector3 Direction = Vector3.Zero;

			Vector3 Speed = new Vector3(1.0f * ts);

			if (Input.IsKeyDown(KeyCode.W))
				Direction.Y = 1;

			if (Input.IsKeyDown(KeyCode.A))
				Direction.X = -1;

			if (Input.IsKeyDown(KeyCode.S))
				Direction.Y = -1;

			if (Input.IsKeyDown(KeyCode.D))
				Direction.X = 1;

			// R = P + D * L

			Vector3 translation = Translation;
			translation += Direction * Speed;
			Translation = translation;
		}

	}
}
