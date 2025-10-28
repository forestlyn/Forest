#pragma once
#include "Core.h"
namespace Engine {
	class FOREST_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}


