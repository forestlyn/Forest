namespace Engine
{
    public class Player : Entity
    {
        public TransformComponent Transform;

        public string Name { get; set; }

        public int Score { get; set; }

        public override void OnCreate()
        {
            Score = 0;
            Name = "Player_" + ID;
            Log.NativeLog($"Player {Name} created with initial score {Score}");
            Transform = GetComponent<TransformComponent>();
            
        }

        public override void OnUpdate(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.Space))
            {
                IncreaseScore(10);
                Log.NativeLog($"Player {Name} scored! New score: {Score}");
            }

            Vector3 position = Transform.Position;
            if (Input.IsKeyPressed(KeyCode.W))
            {
                position.Y += 1.0f * deltaTime;
            }
            if (Input.IsKeyPressed(KeyCode.S))
            {
                position.Y -= 1.0f * deltaTime;
            }
            if (Input.IsKeyPressed(KeyCode.A))
            {
                position.X -= 1.0f * deltaTime;
            }
            if (Input.IsKeyPressed(KeyCode.D))
            {
                position.X += 1.0f * deltaTime;
            }
            Transform.Position = position;
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