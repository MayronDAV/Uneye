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
			Translation = Player.Position;
		}

	}
}
