using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Uneye
{
	public struct Vector2
	{
		public float X, Y;

		public static Vector2 Zero => new Vector2(0.0f);

		public Vector2(float x, float y)
		{
			X = x;
			Y = y;
		}

		public Vector2(float value)
		{
			X = value;
			Y = value;
		}

		public Vector2(Vector2 vec2)
		{
			X = vec2.X;
			Y = vec2.Y;
		}

		public static Vector2 operator +(Vector2 left, Vector2 right)
		{
			return new Vector2(left.X + right.X, left.Y + right.Y);
		}
		public static Vector2 operator *(Vector2 left, Vector2 right)
		{
			return new Vector2(left.X * right.X, left.Y * right.Y);
		}

		public static Vector2 operator +(Vector2 left, float right)
		{
			return new Vector2(left.X + right, left.Y + right);
		}
		public static Vector2 operator *(Vector2 left, float right)
		{
			return new Vector2(left.X * right, left.Y * right);
		}
	}

}
