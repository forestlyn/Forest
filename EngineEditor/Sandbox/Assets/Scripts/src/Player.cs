using System.Numerics;
using Engine;
namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;

        public float speed = 0.1f;
        public int IntValue = 42;
        public double DoubleValue = 3.14;
        public bool IsActive = true;
        public Vector3 Vector3Value = new Vector3(0.0f, 0.0f, 0.0f);
        public Vector2 Vector2Value = new Vector2(1.0f, 2.0f);

        public string Name { get; set; }

        public int Score { get; set; }

        public override void OnCreate()
        {
            Score = 0;
            Name = "Player_" + ID;
            Log.NativeLog($"Player {Name} created with initial score {Score}");
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        public override void OnUpdate(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.Space))
            {
                IncreaseScore(10);
                Log.NativeLog($"Player {Name} scored! New score: {Score}");
            }

            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyPressed(KeyCode.W))
                velocity.Y = 1.0f;
            else if (Input.IsKeyPressed(KeyCode.S))
                velocity.Y = -1.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed;

            m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);
        }

        public void IncreaseScore(int amount)
        {
            Score += amount;
        }

        public override string ToString()
        {
            return $"Player(Name: {Name}, Score: {Score})";
        }
    }
}