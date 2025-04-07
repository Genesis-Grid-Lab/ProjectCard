#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, ApplicationCommandLineArgs args)
    :m_CommandLineArgs(args){

        CE_CORE_ASSERT(!s_Instance, "Application already exists!");

        s_Instance = this;
        m_Window = CreateScope<Window>(WindowProps(name));
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        // render init

    }

Application::~Application(){

}

void Application::Close(){
    m_Running = false;
}

void Application::OnEvent(Event& e){

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

    // for(auto it = )
}

void Application::Run(){

    while(m_Running){
        float time = (float)glfwGetTime();
        // timestep
        m_LastFrameTime = time;

        if(!m_Minimized){

        }

        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e){

    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e){

    if(e.GetWidth() == 0 || e.GetHeight() == 0){
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;


    return false;
}