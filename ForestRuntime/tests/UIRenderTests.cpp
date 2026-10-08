#include "Engine/Core/Application.h"
#include "Engine/Core/RuntimePaths.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Serialization/SceneSerialize.h"
#include "Engine/Project/Project.h"
#include "Engine/Scripts/ScriptEngine.h"
#include <mono/metadata/object.h>
#include <mono/metadata/class.h>
#include <mono/metadata/appdomain.h>
#include "../src/RuntimeLayer.h"
#include <glad/glad.h>
#include <fstream>
#include <array>
#include <stdexcept>

using namespace Engine;
namespace fs = std::filesystem;

namespace
{
    void Check(bool condition, const char *message) { if (!condition) throw std::runtime_error(message); }
    void ManagedTransformSafety()
    {
        auto scene = CreateRef<Scene>();
        auto ui = scene->CreateCanvas();
        auto world = scene->CreateEntity("World");
        ScriptEngine::SetActiveScene(scene.get());
        auto *image = ScriptEngine::GetCoreAssemblyImage();
        auto *entityClass = mono_class_from_name(image, "Engine", "Entity");
        auto *transformClass = mono_class_from_name(image, "Engine", "TransformComponent");
        auto *componentClass = mono_class_from_name(image, "Engine", "Component");
        auto *getter = mono_class_get_method_from_name(transformClass, "get_Position", 0);
        auto *bind = mono_class_get_method_from_name(componentClass, "set_Entity", 1);
        auto *idField = mono_class_get_field_from_name(entityClass, "ID");
        Check(getter && bind && idField, "managed binding metadata");
        for (auto entity : {world, ui})
        {
            auto *wrapper = mono_object_new(mono_domain_get(), transformClass);
            const auto handle = mono_gchandle_new(wrapper, false);
            auto *managedEntity = mono_object_new(mono_domain_get(), entityClass);
            auto id = uint64_t(entity.GetUUID());
            mono_field_set_value(managedEntity, idField, &id);
            void *args[]{managedEntity};
            MonoObject *exception = nullptr;
            mono_runtime_invoke(bind, mono_gchandle_get_target(handle), args, &exception);
            Check(!exception, "managed component entity binding");
            mono_runtime_invoke(getter, mono_gchandle_get_target(handle), nullptr, &exception);
            if (entity == world) Check(!exception, "ordinary managed Transform remains usable");
            else Check(exception && std::string(mono_class_get_name(mono_object_get_class(exception))) == "InvalidOperationException", "UI Transform access throws managed error instead of native assertion");
            mono_gchandle_free(handle);
        }
        ScriptEngine::ReleaseSceneInstances(scene.get());
    }

    void CheckGL(const char *stage)
    {
        GLenum error = GL_NO_ERROR;
        ENQUEUE_RENDER_COMMAND(&error)
        error = glGetError();
        ENQUEUE_RENDER_COMMAND_END()
        Core::Application::Get().FlushRendererCommands();
        if (error != GL_NO_ERROR) throw std::runtime_error(std::string(stage) + ": OpenGL error " + std::to_string(error));
    }
    struct Pixel { unsigned char R, G, B, A; };
    struct Target
    {
        GLuint FBO = 0, Color = 0, IDs = 0, Depth = 0;
        int Width = 0, Height = 0;
        void Reset(int width = 64, int height = 64)
        {
            Width = width; Height = height;
            ENQUEUE_RENDER_COMMAND(this)
            glDeleteFramebuffers(1, &FBO);
            glDeleteTextures(1, &Color); glDeleteTextures(1, &IDs); glDeleteTextures(1, &Depth);
            glCreateFramebuffers(1, &FBO);
            glCreateTextures(GL_TEXTURE_2D, 1, &Color); glTextureStorage2D(Color, 1, GL_RGBA8, Width, Height);
            glCreateTextures(GL_TEXTURE_2D, 1, &IDs); glTextureStorage2D(IDs, 1, GL_R32I, Width, Height);
            glCreateTextures(GL_TEXTURE_2D, 1, &Depth); glTextureStorage2D(Depth, 1, GL_DEPTH_COMPONENT24, Width, Height);
            glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, Color, 0);
            glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT1, IDs, 0);
            glNamedFramebufferTexture(FBO, GL_DEPTH_ATTACHMENT, Depth, 0);
            const GLenum buffers[]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glNamedFramebufferDrawBuffers(FBO, 2, buffers);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glViewport(0, 0, Width, Height);
            glDisable(GL_SCISSOR_TEST); glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); glDepthMask(GL_TRUE);
            glEnable(GL_BLEND); glBlendEquation(GL_FUNC_ADD); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            const GLfloat black[]{0, 0, 0, 1}, depth = 1;
            const GLint noEntity = -1;
            glClearBufferfv(GL_COLOR, 0, black); glClearBufferiv(GL_COLOR, 1, &noEntity);
            glClearBufferfv(GL_DEPTH, 0, &depth);
            ENQUEUE_RENDER_COMMAND_END()
            Core::Application::Get().FlushRendererCommands();
        }
        Pixel Read(int x, int y)
        {
            Pixel result{};
            ENQUEUE_RENDER_COMMAND(this, x, y, &result)
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(x, Height - 1 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &result);
            ENQUEUE_RENDER_COMMAND_END()
            Core::Application::Get().FlushRendererCommands();
            return result;
        }
        void Expect(int x, int y, int r, int g, int b, const char *message)
        {
            const auto p = Read(x, y);
            if (std::abs(int(p.R) - r) > 2 || std::abs(int(p.G) - g) > 2 || std::abs(int(p.B) - b) > 2)
                throw std::runtime_error(std::string(message) + ": got " + std::to_string(p.R) + "," + std::to_string(p.G) + "," + std::to_string(p.B));
        }
        void Save(const fs::path &path)
        {
            std::vector<Pixel> pixels(Width * Height);
            ENQUEUE_RENDER_COMMAND(this, &pixels)
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glReadPixels(0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            ENQUEUE_RENDER_COMMAND_END()
            Core::Application::Get().FlushRendererCommands();
            std::ofstream out(path, std::ios::binary);
            out << "P6\n" << Width << ' ' << Height << "\n255\n";
            for (int y = Height - 1; y >= 0; --y)
                for (int x = 0; x < Width; ++x) out.write(reinterpret_cast<const char *>(&pixels[y * Width + x]), 3);
        }
        ~Target()
        {
            ENQUEUE_RENDER_COMMAND(this)
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &FBO);
            glDeleteTextures(1, &Color); glDeleteTextures(1, &IDs); glDeleteTextures(1, &Depth);
            ENQUEUE_RENDER_COMMAND_END()
            Core::Application::Get().FlushRendererCommands();
        }
    };

    Entity Image(Scene &scene, Entity parent, glm::vec2 position, glm::vec2 size, glm::vec4 color, int order)
    {
        auto e = scene.CreateUIEntity(parent, "Image");
        auto &rect = e.GetComponent<RectTransformComponent>();
        rect.AnchorMin = rect.AnchorMax = rect.Pivot = {0, 0};
        rect.AnchoredPosition = position; rect.SizeDelta = size; scene.SetSiblingOrder(e, order);
        e.AddComponent<UIImageComponent>().Color = color;
        return e;
    }
    Entity Canvas(Scene &scene)
    {
        auto e = scene.CreateCanvas();
        e.GetComponent<CanvasComponent>().ScaleMode = CanvasScaleMode::ConstantPixelSize;
        return e;
    }

    void WriteTexture(const fs::path &path)
    {
        // Top-left origin TGA: red/green above blue/white. Tests image orientation.
        const unsigned char header[]{0,0,2,0,0,0,0,0,0,0,0,0,2,0,2,0,32,0x28};
        const unsigned char bgra[]{0,0,255,255, 0,255,0,255, 255,0,0,255, 255,255,255,255};
        std::ofstream stream(path, std::ios::binary);
        stream.write(reinterpret_cast<const char *>(header), sizeof(header));
        stream.write(reinterpret_cast<const char *>(bgra), sizeof(bgra));
    }

    void Composition(Target &target, const fs::path &output)
    {
        auto scene = CreateRef<Scene>();
        auto canvas = Canvas(*scene);
        Image(*scene, canvas, {0,0}, {64,64}, {1,0,0,1}, 0);
        auto panel = Image(*scene, canvas, {16,16}, {32,32}, {0,0,1,0.5f}, 1);
        Image(*scene, panel, {8,8}, {8,8}, {0,1,0,1}, 0);
        auto textured = Image(*scene, canvas, {0,40}, {16,16}, {1,1,1,1}, 2);
        textured.GetComponent<UIImageComponent>().TextureRef.SetPath("corners.tga");
        auto disabled = Image(*scene, canvas, {0,0}, {64,64}, {1,1,1,1}, 3);
        disabled.GetComponent<UIImageComponent>().SetEnabled(false);
        auto higher = Canvas(*scene);
        higher.GetComponent<CanvasComponent>().SortOrder = 1;
        Image(*scene, higher, {0,0}, {8,8}, {1,1,0,1}, 0);
        scene->SetViewportSize(64,64);
        target.Reset();
        // An occluding depth buffer must not block the UI; test non-default state restoration too.
        ENQUEUE_RENDER_COMMAND()
        const GLfloat depth = 0.25f;
        glClearBufferfv(GL_DEPTH, 0, &depth);
        glDisable(GL_BLEND); glEnable(GL_CULL_FACE); glEnable(GL_SCISSOR_TEST); glScissor(0,0,1,1);
        glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_SUBTRACT);
        glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_DST_ALPHA, GL_ONE);
        ENQUEUE_RENDER_COMMAND_END()
        scene->RenderUI();
        bool restored = false;
        ENQUEUE_RENDER_COMMAND(&restored)
        GLboolean mask; GLint equation, src; GLfloat depth;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &mask);
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &equation); glGetIntegerv(GL_BLEND_SRC_ALPHA, &src);
        glReadPixels(20,20,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
        restored = glIsEnabled(GL_DEPTH_TEST) && mask && !glIsEnabled(GL_BLEND) && glIsEnabled(GL_CULL_FACE) &&
                   glIsEnabled(GL_SCISSOR_TEST) && equation == GL_FUNC_REVERSE_SUBTRACT && src == GL_DST_ALPHA && std::abs(depth - 0.25f) < 0.001f;
        ENQUEUE_RENDER_COMMAND_END()
        Core::Application::Get().FlushRendererCommands();
        Check(restored, "overlay must restore graphics state and preserve depth");
        target.Expect(4,4,255,255,0,"higher Canvas");
        target.Expect(60,60,255,0,0,"solid image");
        target.Expect(20,20,128,0,128,"alpha composition and sibling order");
        target.Expect(26,26,0,255,0,"child above parent");
        target.Expect(2,42,255,0,0,"texture top-left");
        target.Expect(13,42,0,255,0,"texture top-right");
        target.Expect(2,53,0,0,255,"texture bottom-left");
        target.Expect(13,53,255,255,255,"texture bottom-right");
        int picked = -1;
        ENQUEUE_RENDER_COMMAND(&picked)
        glReadBuffer(GL_COLOR_ATTACHMENT1); glReadPixels(2,21,1,1,GL_RED_INTEGER,GL_INT,&picked);
        ENQUEUE_RENDER_COMMAND_END()
        Core::Application::Get().FlushRendererCommands();
        Check(picked == static_cast<int>(entt::entity(textured)), "image writes editor entity ID");
        target.Save(output / "composition.ppm");

        const auto file = output / "composition.scene";
        Serialization::SceneSerialize(scene).Serialize(file.string());
        auto loaded = CreateRef<Scene>();
        Check(Serialization::SceneSerialize(loaded).Deserialize(file.string()), "image scene load");
        auto loadedImage = loaded->GetEntityByUUID(textured.GetUUID());
        Check(loadedImage.GetComponent<UIImageComponent>().TextureRef.path == "corners.tga", "relative image path round trip");
        Check(!loaded->GetEntityByUUID(disabled.GetUUID()).GetComponent<UIImageComponent>().IsEnabled(), "image enabled round trip");
        loaded->SetViewportSize(64,64);
        target.Reset(); loaded->OnUpdateEditor(0.0f, glm::mat4(1));
        target.Expect(20,20,128,0,128,"editor preview after reload");
        target.Reset(); loaded->OnRuntimeStart(); loaded->OnUpdateRuntime(0.0f); loaded->OnRuntimeStop();
        target.Expect(20,20,128,0,128,"camera-free runtime matches editor");
        target.Reset(); loaded->OnSimulationStart(); loaded->OnUpdateSimulate(0.0f, glm::mat4(1)); loaded->OnSimulationStop();
        target.Expect(20,20,128,0,128,"simulation preview");
        target.Reset(); loaded->SetViewportSize(0,0); loaded->RenderUI();
        target.Expect(20,20,0,0,0,"zero viewport does not draw");
    }

    void ResizeAndWorld(Target &target)
    {
        Scene scene;
        auto canvas = scene.CreateCanvas();
        canvas.GetComponent<CanvasComponent>().ReferenceResolution = {64,64};
        Image(scene, canvas, {16,16}, {32,32}, {1,0,0,1}, 0);
        auto world = scene.CreateEntity("World");
        world.AddComponent<SpriteComponent>().Color = {0,0,1,1};
        world.GetComponent<TransformComponent>().SetScale({2,2,1});
        target.Reset(128,128); scene.SetViewportSize(128,128);
        scene.OnUpdateEditor(0.0f, glm::mat4(1));
        target.Expect(40,40,255,0,0,"scaled UI covers world");
        target.Expect(20,20,0,0,255,"world remains outside UI");
        target.Reset(128,128);
        scene.OnUpdateEditor(0.0f, glm::scale(glm::mat4(1), glm::vec3(0.25f)));
        target.Expect(40,40,255,0,0,"editor camera does not change UI");
        target.Expect(20,20,0,0,0,"world camera actually changed");
        target.Reset(128,64); scene.SetViewportSize(128,64);
        canvas.GetComponent<CanvasComponent>().MatchWidthOrHeight = 1;
        scene.RenderUI();
        target.Expect(20,20,255,0,0,"non-square height match");
        target.Expect(60,20,0,0,0,"non-square layout bounds");
        auto camera = scene.CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().OrthographicSize = 2.0f;
        target.Reset(); scene.SetViewportSize(64,64);
        scene.OnRuntimeStart(); scene.OnUpdateRuntime(0.0f); scene.OnRuntimeStop();
        target.Expect(20,20,255,0,0,"UI overlays primary-camera runtime");
        target.Expect(4,4,0,0,255,"primary-camera world remains visible");
    }

    void BatchBoundaries(Target &target, const fs::path &assets)
    {
        Scene scene;
        auto canvas = Canvas(scene);
        for (int i = 0; i < 10000; ++i) Image(scene, canvas, {-4,-4}, {1,1}, {1,0,0,1}, i);
        auto last = Image(scene, canvas, {0,0}, {64,64}, {1,1,1,1}, 10000);
        last.GetComponent<UIImageComponent>().TextureRef.SetPath("corners.tga");
        scene.SetViewportSize(64,64); target.Reset(); scene.RenderUI();
        target.Expect(8,8,255,0,0,"texture after vertex batch boundary");
        target.Expect(56,8,0,255,0,"texture slot retained after vertex flush");
        Scene textures;
        canvas = Canvas(textures);
        for (int i = 0; i < 34; ++i)
        {
            const auto name = "slot-" + std::to_string(i) + ".tga";
            WriteTexture(assets / name);
            auto image = Image(textures, canvas, {0,0}, {64,64}, {1,1,1,1}, i);
            image.GetComponent<UIImageComponent>().TextureRef.SetPath(name);
        }
        Image(textures, canvas, {0,0}, {16,16}, {0,0,1,0.5f}, 35);
        textures.SetViewportSize(64,64); target.Reset(); textures.RenderUI();
        target.Expect(8,8,128,0,128,"painter order across texture-slot flush");
        target.Expect(56,8,0,255,0,"texture after slot flush");
    }
}

int main(int argc, char **argv)
{
    Core::Log::Init();
    try
    {
        Check(argc == 2 || argc == 3, "Expected output directory and optional example project");
        const auto output = fs::absolute(argv[1]);
        fs::create_directories(output / "Assets");
        WriteTexture(output / "Assets/corners.tga");
        std::ofstream(output / "Test.forestproj") << "Name: UIRenderTests\nAssetDirectory: Assets\nScriptAssembly: ''\n";
        Check(bool(Project::Load(output / "Test.forestproj")), "load test project");
        const auto root = Core::RuntimePaths::ExecutableDirectory();
        Core::ApplicationSpecification spec;
        spec.Name = "UI pixel tests"; spec.Width = spec.Height = 64;
        spec.WindowVisible = false; spec.VSync = false;
        spec.EnableImGui = spec.EnableProfileLayer = spec.EnableScriptDebugging = spec.EnableScriptHotReload = false;
        spec.EngineResourceDirectory = (root / "resources").string();
        spec.UserDataDirectory = (output / "UserData").string();
        spec.MonoAssemblyPath = (root / "Mono/4.5").string();
        spec.CoreAssemblyPath = (root / "Managed/Engine-ScriptCore.dll").string();
        spec.AppAssemblyPath.clear();
        Core::Application app(spec);
        // Attribute errors to the pass under test. The existing renderer bootstrap can
        // report driver errors before any UI is submitted; log those separately.
        ENQUEUE_RENDER_COMMAND()
        for (GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError())
            ENGINE_WARN("Pre-UI renderer initialization OpenGL error: {}", error);
        ENQUEUE_RENDER_COMMAND_END()
        app.FlushRendererCommands();
        ManagedTransformSafety();
        Target target;
        Composition(target, output);
        CheckGL("Composition");
        ResizeAndWorld(target);
        CheckGL("Resize and world");
        BatchBoundaries(target, output / "Assets");
        CheckGL("Batch boundaries");
        // Exercise the actual RuntimeLayer startup guard and rendering entry point.
        target.Reset();
        ForestRuntime::RuntimeLayer layer(output / "composition.scene", 0);
        layer.OnAttach(); layer.OnUpdate(0.0f); layer.OnDetach();
        target.Expect(20,20,128,0,128,"RuntimeLayer accepts camera-free UI");
        std::ofstream(output / "empty.scene") << "Scene: Empty\nEntities: []\n";
        bool rejected = false;
        try { ForestRuntime::RuntimeLayer empty(output / "empty.scene", 0); empty.OnAttach(); }
        catch (const std::runtime_error &) { rejected = true; }
        Check(rejected, "RuntimeLayer still rejects scenes with neither camera nor valid Canvas");
        CheckGL("RuntimeLayer");
        if (argc == 3)
        {
            Check(bool(Project::Load(fs::absolute(argv[2]))), "example project load");
            auto example = CreateRef<Scene>();
            // The project's selected startup scene is editable; this test targets the UI fixture.
            Check(Serialization::SceneSerialize(example).Deserialize(Project::GetActiveProjectAssetPath("Scenes/Images.scene").string()), "example scene load");
            Check(example->HasValidCanvas(), "example is a valid camera-free UI scene");
            for (auto entity : example->GetRegistry().view<RectTransformComponent>())
                Check(!example->GetRegistry().all_of<TransformComponent>(entity), "Images.scene contains only RectTransform");
            target.Reset(1280,720); example->SetViewportSize(1280,720);
            example->OnUpdateEditor(0.0f, glm::mat4(1));
            target.Save(output / "example-1280x720.ppm");
            CheckGL("Example preview");
        }
        ENGINE_INFO("PASS: UI pixels, texture orientation, alpha, ordering, state restoration, batch boundaries, resize, serialization and RuntimeLayer");
        return 0;
    }
    catch (const std::exception &error) { ENGINE_ERROR("UI P2 test failed: {}", error.what()); return 1; }
}
