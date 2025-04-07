#pragma once

#include "Core/Config.h"
#include "CardEngine.h"

#include "Window.h"
#include "Core/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Core/Timestep.h"

int main(int argc, char** argv);

struct ApplicationCommandLineArgs{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const{
        CE_CORE_ASSERT(index < Count);
        return Args[index];
    }
};


class Application{
public:
    Application(const std::string& name = "Application Name", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
    virtual ~Application();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    Window& GetWindow() { return *m_Window;}

    void Close();

    static Application& Get() { return *s_Instance;}

    ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs;}

private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
private:
    ApplicationCommandLineArgs m_CommandLineArgs;
    Scope<Window> m_Window;

    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
private:
    static Application* s_Instance;
    friend int main(int argc, char** argv);
};