#include "Source/Core/Application/Application.h"

// Simple RTS application class
class RTSApplication : public Application {
public:
    RTSApplication() : Application() {
        // Configure application
        ApplicationConfig config;
        config.name = "RTS Game - DirectX 12";
        config.windowDesc.title = "RTS Game - DirectX 12";
        config.windowDesc.width = 1280;
        config.windowDesc.height = 720;
        config.windowDesc.resizable = true;
        config.windowDesc.vsync = true;
   }

protected:
    bool OnInitialize() override {
        Platform::OutputDebugMessage("RTS Application initializing...\n");

        // Initialize game systems here
        // - Renderer
        // - ECS
        // - Resource Manager
        // - Input System
        // etc.

        Platform::OutputDebugMessage("RTS Application initialized successfully!\n");
        return true;
    }

    void OnShutdown() override {
        Platform::OutputDebugMessage("RTS Application shutting down...\n");

        // Cleanup game systems here
    }

    void OnUpdate(float32 deltaTime) override {
        // Update game logic here

        // Example: Print FPS every second
        static float32 fpsTimer = 0.0f;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            String fpsText = "FPS: " + std::to_string(GetTimer().GetFPS());
            Platform::OutputDebugMessage(fpsText + "\n");
            fpsTimer = 0.0f;
        }
    }

    void OnRender() override {
        // Render game here
        // For now, just clear to a color
    }

    void OnWindowResize(uint32 width, uint32 height) override {
    }

    void OnKeyEvent(const KeyEvent& event) override {
        if (event.key == KeyCode::Escape && event.pressed) {
            Platform::OutputDebugMessage("Escape pressed, requesting exit...\n");
            RequestExit();
        }

        if (event.key == KeyCode::F11 && event.pressed) {
            Platform::OutputDebugMessage("F11 pressed - Toggle fullscreen (not implemented yet)\n");
        }
    }

    void OnMouseButtonEvent(const MouseButtonEvent& event) override {
        String buttonName = (event.button == MouseButton::Left) ? "Left" :
                           (event.button == MouseButton::Right) ? "Right" : "Middle";
        String action = event.pressed ? "pressed" : "released";

        Platform::OutputDebugMessage("Mouse " + buttonName + " " + action +
            " at (" + std::to_string(event.x) + ", " + std::to_string(event.y) + ")\n");
    }

    void OnMouseMoveEvent(const MouseMoveEvent& event) override {
        // Uncomment for mouse tracking (will be very verbose)
        /*
        Platform::OutputDebugMessage("Mouse moved to (" +
            std::to_string(event.x) + ", " + std::to_string(event.y) +
            ") delta: (" + std::to_string(event.deltaX) + ", " +
            std::to_string(event.deltaY) + ")\n");
        */
    }

    void OnMouseWheelEvent(const MouseWheelEvent& event) override {
        Platform::OutputDebugMessage("Mouse wheel: " + std::to_string(event.delta) +
            " at (" + std::to_string(event.x) + ", " + std::to_string(event.y) + ")\n");
    }
};

// Use the macro to create the entry point
//IMPLEMENT_APPLICATION(RTSApplication)

int CALLBACK WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    try {
        RTSApplication app;
        return app.Run();
    }
    catch (const std::exception& e) {
        Platform::ShowMessageBox("Error", e.what());
        return -1;
    }
    catch (...) {
        Platform::ShowMessageBox("Error", "Unknown error occurred");
        return -1;
    }

	return 0;
}