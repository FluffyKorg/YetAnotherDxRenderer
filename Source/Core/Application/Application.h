#pragma once

#include <WindowsPlatform.h>
#include "../../Core/Window/Window.h"
#include "../../Core/Graphics/Graphics.h"
#include "Timer.h"

// Application configuration
struct ApplicationConfig {
    String name = "RTS Game";
    WindowDesc windowDesc;
    bool enableDebugLayer = DEBUG_BUILD;
    bool enableValidation = DEBUG_BUILD;
};

class Application {
public:
    Application(const ApplicationConfig& config = {});
    virtual ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    // Main application flow
    int32 Run();
    void Shutdown();

    // Application control
    void RequestExit() { m_shouldExit = true; }
    bool ShouldExit() const { return m_shouldExit; }
	void SetPaused(bool paused) { m_isAppPaused = paused; }

    // Accessors
    Window* GetWindow() const { return m_window.get(); }
    const Timer& GetTimer() const { return m_timer; }
    const ApplicationConfig& GetConfig() const { return m_config; }

    // Static instance access
    static Application* GetInstance() { return s_instance; }

protected:
    // Virtual methods for derived classes
    virtual bool OnInitialize() { return true; }
    virtual void OnShutdown() {}
    virtual void OnUpdate(float32 deltaTime) {}
    virtual void OnRender() {}
    virtual void OnWindowResize(uint32 width, uint32 height) {}
    virtual void OnKeyEvent(const KeyEvent& event) {}
    virtual void OnMouseButtonEvent(const MouseButtonEvent& event) {}
    virtual void OnMouseMoveEvent(const MouseMoveEvent& event) {}
    virtual void OnMouseWheelEvent(const MouseWheelEvent& event) {}

private:
    // Internal methods
    bool CreateAppWindow();
    void SetupEventCallbacks();
    void MainLoop();
    void Update();
    void Render();

    // Event handlers
    void HandleWindowResize(const WindowResizeEvent& event);
    void HandleWindowClose();
    void HandleKeyEvent(const KeyEvent& event);
    void HandleMouseButtonEvent(const MouseButtonEvent& event);
    void HandleMouseMoveEvent(const MouseMoveEvent& event);
    void HandleMouseWheelEvent(const MouseWheelEvent& event);

private:
    // Application state
    ApplicationConfig m_config;
    bool m_initialized = false;
    bool m_shouldExit = false;
	bool m_isAppPaused = false;

    // Core systems
    UniquePtr<Window> m_window = nullptr;
	UniquePtr<Graphics> m_graphics = nullptr;
    Timer m_timer;

    // Static instance
    static Application* s_instance;
};

// Macro for creating application entry point
#define IMPLEMENT_APPLICATION(AppClass) \
    int CALLBACK WinMain( \
    _In_ HINSTANCE hInstance, \
    _In_opt_ HINSTANCE hPrevInstance, \
    _In_ LPSTR lpCmdLine, \
    _In_ int nCmdShow) { \
        try { \
            AppClass app; \
            if (!app.Initialize()) { \
                return -1; \
            } \
            return app.Run(); \
        } \
        catch (const std::exception& e) { \
            Platform::ShowMessageBox("Error", e.what()); \
            return -1; \
        } \
        catch (...) { \
            Platform::ShowMessageBox("Error", "Unknown error occurred"); \
            return -1; \
        } \
    }