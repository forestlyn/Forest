#include "Application.h"
#include "Engine/pcheader.h"
#include "Engine/Events/Event.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Profile/Instrumentor.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <filesystem>

namespace Engine::Core
{

	Application *Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification &spec)
	{
		ENGINE_PROFILING_SCOPE("START");

		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Specification = spec;

		ENGINE_INFO("current path:{}", std::filesystem::current_path().string());

		// Set working directory
		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		ENGINE_INFO("Set working directory to: {}", std::filesystem::current_path().string());
		// Create Window
		WindowProps windowSpec;
		windowSpec.Title = m_Specification.Name;
		windowSpec.Width = m_Specification.Width;
		windowSpec.Height = m_Specification.Height;

		InitRendererMemoryPool();
		StartRenderThread();

		m_Window = Scope<Window>(Window::Create(windowSpec));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(m_Specification.VSync);
		m_Window->SetFullScreen(m_Specification.Fullscreen);
		Renderer::Renderer::Init();

		ENGINE_INFO("Window created successfully!");
		std::atomic_bool renderBootstrapReady = false;
		SubmitRendererCommand([&renderBootstrapReady]()
							  { renderBootstrapReady.store(true, std::memory_order_release); });

		while (!renderBootstrapReady.load(std::memory_order_acquire))
		{
			DispatchRendererCommands();
			std::this_thread::yield();
		}
		ENGINE_INFO("Renderer initialized successfully! {0}", renderBootstrapReady.load(std::memory_order_acquire));

		// imgui init needs to be after window creation and s_Instance assignment
		m_ImGuiLayer = new MyImGui::ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		m_ImGuiLayer->SetDarkThemeColors();

#if defined(ENGINE_ENABLE_PROFILELAYER)
		m_ProfileLayer = new Engine::Profile::ProfileLayer();
		PushOverlay(m_ProfileLayer);
#endif
		ScriptEngine::Init();
	}

	Application::~Application()
	{
		ENGINE_PROFILING_FUNC();
		Renderer::Renderer::Shutdown();
		DispatchRendererCommands();

		if (m_Window)
			m_Window.reset();
		DispatchRendererCommands();

		StopRenderThread();
		ReleaseRendererMemoryPool();
		ScriptEngine::Shutdown();
	}

	void Application::Init()
	{
	}

	void Application::Run()
	{
		ENGINE_PROFILING_FUNC();
		while (m_Running)
		{
			{
				ENGINE_PROFILING_SCOPE("Run Loop");
				Timestep time = glfwGetTime(); // Get time in seconds
				float deltaTime = time - m_LastFrameTime;
				m_LastFrameTime = time;

				DispatchRendererCommands();

				m_Window->OnUpdateEvent();

				ExecuteMainThreadQueueBack();

				if (!m_Minimized && m_Focused)
				{
					ExecuteMainThreadQueueFront();
					ENGINE_PROFILING_SCOPE("LayerStack OnUpdate");
					for (auto layer : m_LayerStack)
						layer->OnUpdate(deltaTime);
				}
				{
					ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender");
					{
						ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender Begin");
						m_ImGuiLayer->Begin();
					}
					{
						ENGINE_PROFILING_SCOPE("Layers OnImGuiRender");
						for (auto layer : m_LayerStack)
						{
							layer->OnImGuiRender();
						}
					}

					{
						ENGINE_PROFILING_SCOPE("LayerStack OnImGuiRender End");
						m_ImGuiLayer->End();
					}
				}

				{
					ENGINE_PROFILING_SCOPE("Window OnUpdate");
					m_Window->OnUpdate();
				}

				{
					ENGINE_PROFILING_SCOPE("Renderer NextFrame");
#ifdef ENGINE_ENABLE_RENDERTHREAD
					m_RendererMemoryPool->Swap();
#endif
				}

#ifdef ENGINE_ENABLE_RENDERTHREAD
				std::atomic_bool frameFenceReady = false;
				SubmitRendererCommand([&frameFenceReady]()
									  { frameFenceReady.store(true, std::memory_order_release); });

				while (!frameFenceReady.load(std::memory_order_acquire))
				{
					DispatchRendererCommands();
					std::this_thread::yield();
				}
#else
				DispatchRendererCommands();
#endif
			}
		}
	}

	void Application::Shutdown()
	{
		m_Running = false;
		DispatchRendererCommands();
	}

	bool Application::OnEvent(Event::Event &e)
	{
		ENGINE_PROFILING_FUNC();
		Event::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Event::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<Event::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<Event::WindowFocusEvent>(BIND_EVENT_FN(Application::OnFocusWindow));
		dispatcher.Dispatch<Event::WindowLostFocusEvent>(BIND_EVENT_FN(Application::OnLostFocusWindow));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}

		return false;
	}

	bool Application::OnWindowClose(Event::WindowCloseEvent &e)
	{
		ENGINE_PROFILING_FUNC();
		Shutdown();
		return true;
	}

	bool Application::OnWindowResize(Event::WindowResizeEvent &e)
	{
		ENGINE_PROFILING_FUNC();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
		}
		else
		{
			m_Minimized = false;
			Renderer::Renderer::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		}
		return false;
	}

	bool Application::OnFocusWindow(Event::WindowFocusEvent &e)
	{
		m_Focused = true;
		return false;
	}

	bool Application::OnLostFocusWindow(Event::WindowLostFocusEvent &e)
	{
		m_Focused = false;
		return false;
	}

	void Application::ExecuteMainThreadQueueFront()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		for (auto func : m_MainThreadQueueFront)
		{
			func();
		}
		m_MainThreadQueueFront.clear();
	}

	void Application::ExecuteMainThreadQueueBack()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		for (auto func : m_MainThreadQueueBack)
		{
			func();
		}
		m_MainThreadQueueBack.clear();
	}

	void Application::PushLayer(Layer *layer)
	{
		ENGINE_PROFILING_FUNC();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay)
	{
		ENGINE_PROFILING_FUNC();
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	void *Application::RendererAllocator(size_t size)
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		return m_RendererMemoryPool->Allocate(size);
#else
		ENGINE_ASSERT(false, "use RendererAllocator Wrong");
		return nullptr;
#endif
	}
	void Application::SubmitRendererCommand(std::function<void()> &&renderCmd)
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		if (!m_RenderThreadStarted)
		{
			renderCmd();
			return;
		}

		std::scoped_lock<std::mutex> lock(m_RenderThreadQueueMutex);
		m_RenderSubmitQueue.emplace_back(std::move(renderCmd));
		// renderCmd();
#else
		renderCmd();
#endif
	}
	void Application::SubmitToMainThread(const std::function<void()> func, bool isFront)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		if (isFront)
			m_MainThreadQueueFront.emplace_back(func);
		else
			m_MainThreadQueueBack.emplace_back(func);
	}

	void Application::InitRendererMemoryPool()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		const size_t Memory_Size = 1024 * 1024 * 256;
		m_RendererMemoryPool = new Memory::RenderMemoryPool(Memory_Size);

#endif
	}

	void Application::ReleaseRendererMemoryPool()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		delete m_RendererMemoryPool;

#endif
	}

	void Application::StartRenderThread()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		if (m_RenderThreadRunning)
		{
			ENGINE_ERROR("Render thread already running!");
			return;
		}

		m_RenderThreadRunning = true;
		m_RenderThread = std::thread(&Application::RenderThreadLoop, this);
		m_RenderThreadStarted = true;
#endif
	}

	void Application::StopRenderThread()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		if (!m_RenderThreadStarted)
			return;

		DispatchRendererCommands();
		{
			std::scoped_lock<std::mutex> lock(m_RenderThreadQueueMutex);
			m_RenderThreadRunning = false;
		}
		m_RenderThreadCV.notify_one();

		if (m_RenderThread.joinable())
			m_RenderThread.join();

		m_RenderThreadStarted = false;
#endif
	}

	void Application::DispatchRendererCommands()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		if (!m_RenderThreadStarted)
			return;

		{
			std::scoped_lock<std::mutex> lock(m_RenderThreadQueueMutex);
			if (m_RenderSubmitQueue.empty())
				return;
			if (!m_RenderExecuteQueue.empty())
				return;

			m_RenderExecuteQueue.swap(m_RenderSubmitQueue);
		}

		m_RenderThreadCV.notify_one();
#endif
	}

	void Application::RenderThreadLoop()
	{
#ifdef ENGINE_ENABLE_RENDERTHREAD
		while (true)
		{
			std::vector<std::function<void()>> work;

			{
				std::unique_lock<std::mutex> lock(m_RenderThreadQueueMutex);
				m_RenderThreadCV.wait(lock, [this]()
									  { return !m_RenderThreadRunning || !m_RenderExecuteQueue.empty(); });

				if (!m_RenderThreadRunning && m_RenderExecuteQueue.empty())
					break;

				work.swap(m_RenderExecuteQueue);
			}

			for (auto &cmd : work)
				cmd();
		}
#endif
	}
}