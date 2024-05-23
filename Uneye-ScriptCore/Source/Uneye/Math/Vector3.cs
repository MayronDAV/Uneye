using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Uneye
{
	public struct Vector3
	{
		public float X, Y, Z;

		public Vector2 XY {
			get => new Vector2(X, Y);
			set { X = value.X; Y = value.Y; }
		}

		public static Vector3 Zero => new Vector3(0.0f);

		public Vector3(float x, float y, float z)
		{

			X = x;
			Y = y;
			Z = z;
		}

		public Vector3(float value)
		{
			X = value;
			Y = value;
			Z = value;
		}

		public Vector3(Vector3 vec3)
		{

			X = vec3.X;
			Y = vec3.Y;
			Z = vec3.Z;
		}

		public Vector3(Vector2 xy, float z)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
		}

		public static Vector3 operator +(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		}
		public static Vector3 operator *(Vector3 left, Vector3 right)
		{
			return new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
		}

		public static Vector3 operator +(Vector3 left, float right)
		{
			return new Vector3(left.X + right, left.Y + right, left.Z + right);
		}
		public static Vector3 operator *(Vector3 left, float right)
		{
			return new Vector3(left.X * right, left.Y * right, left.Z * right);
		}
	}

}
