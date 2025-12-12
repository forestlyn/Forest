#pragma once

//------core system------
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Layer.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCode.h"
#include "Engine/Core/MouseCode.h"
#include "Engine/Core/Timestep.h"
//-----------------------

//------event system------
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/WindowEvent.h"
//-----------------------

#include "Engine/ImGui/ImGuiLayer.h"

//------renderer system------
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Renderer/RenderCommand.h"

#include "Engine/Renderer/Shader/Buffer.h"
#include "Engine/Renderer/Shader/Shader.h"
#include "Engine/Renderer/Shader/VertexArray.h"
#include "Engine/Renderer/Shader/Texture.h"
#include "Engine/Renderer/Shader/ShaderLibrary.h"

#include "Engine/Renderer/Camera/Camera.h"
#include "Engine/Renderer/Camera/OrthographicCamera.h"
#include "Engine/Renderer/Camera/PerspectiveCamera.h"

//-------------------------

//-------profile system------
#include "Engine/Profile/Timer.h"
#include "Engine/Profile/ProfileLayer.h"
//--------------------------

//------camera controller system------
#include "Engine/Controller/OrthographicCameraController.h"
#include "Engine/Controller/PerspectiveCameraController.h"
