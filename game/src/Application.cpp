#include "Application.h"
#include "Layers/TestLayer.h"

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, ApplicationCommandLineArgs args)
    :m_CommandLineArgs(args){

        CE_CORE_ASSERT(!s_Instance, "Application already exists!");

        s_Instance = this;
        m_Window = CreateScope<Window>(WindowProps(name));
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        // render init

        PushLayer(new TestLayer());

    }

Application::~Application(){

}

void Application::PushLayer(Layer* layer)
{    
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close(){
    m_Running = false;
}

void Application::OnEvent(Event& e){

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
        if (e.Handled) 
            break;
        (*it)->OnEvent(e);
    }
}

void Application::Run(){

    while(m_Running){
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if(!m_Minimized){
            for(Layer* layer : m_LayerStack)
                layer->OnUpdate(timestep);
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