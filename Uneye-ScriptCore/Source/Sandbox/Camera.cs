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
		public void OnUpdate(float ts)
		{
			Vector3 Direction = Vector3.Zero;

			Vector3 Speed = new Vector3(5.0f * ts);

			if (Input.IsKeyDown(KeyCode.Up))
				Direction.Y = 1;

			if (Input.IsKeyDown(KeyCode.Left))
				Direction.X = -1;

			if (Input.IsKeyDown(KeyCode.Down))
				Direction.Y = -1;

			if (Input.IsKeyDown(KeyCode.Right))
				Direction.X = 1;

			// R = P + D * L

			Vector3 translation = Translation;
			translation += Direction * Speed;
			Translation = translation;
		}


	}
}
