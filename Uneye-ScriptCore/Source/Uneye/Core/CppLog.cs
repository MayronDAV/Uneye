using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Uneye
{
	public static class CppLog
	{
		public static void Log(string message)
		{
			InternalCalls.NativeLog(message);
		}
	}
}
