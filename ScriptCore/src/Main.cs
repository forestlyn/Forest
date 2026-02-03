using System;
using System.Runtime.CompilerServices;
namespace Engine
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog(string text, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void NativeLog_Vector(ref Vector3 parameter, out Vector3 result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float NativeLog_VectorDot(ref Vector3 parameter);
    }

    public class Entity
    {
        public float FloatVar { get; set; }

        public Entity()
        {
            Console.WriteLine("Main constructor!");
            Log("AAstroPhysiC", 8058);

            Vector3 pos = new Vector3(5, 2.5f, 1);
            Vector3 result = Log_Vector(pos);
            Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");
            Console.WriteLine("{0}", Log_VectorDot(pos));
        }

        public void PrintMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }
        public void Log(string text, int parameter)
        {
            InternalCalls.NativeLog(text, parameter);
        }

        public Vector3 Log_Vector(Vector3 parameter)
        {
            Vector3 result;
            InternalCalls.NativeLog_Vector(ref parameter, out result);
            return result;
        }

        public float Log_VectorDot(Vector3 parameter)
        {
            return InternalCalls.NativeLog_VectorDot(ref parameter);
        }

    }
}