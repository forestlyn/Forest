using System;
namespace Engine
{

    public class CSharpTesting
    {
        private float myPrivateFloatVar = 5.0f;
        public int MyPublicIntVar = 10;
        public string MyPublicStringVar = "Hello from C#";
        public float MyPublicFloatVar
        {
            get { return myPrivateFloatVar; }
            set { myPrivateFloatVar = value; }
        }

        public void PrintFloatVar()
        {
            Console.WriteLine("MyPublicFloatVar = {0:F}", MyPublicFloatVar);
        }

        private void IncrementFloatVar(float value)
        {
            MyPublicFloatVar += value;
            throw new Exception("Test exception from C# IncrementFloatVar");
        }

    }
}