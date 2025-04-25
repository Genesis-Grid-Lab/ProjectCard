#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Config.h"
#include "UI.h"
#include <imgui.h>
#include "Core/Input.h"
#include "Application.h"
#include "GameLayer.h"

class MainMenuLayer : public Layer{
public:
    MainMenuLayer(){

    }
    ~MainMenuLayer(){
        // OnDetach();
    }

    void OnAttach() override{
        
        m_Scene = GA::CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);
        TitleBorder = new Border(m_Scene, "TitleBorder", {SCREEN_WIDTH / 2, 40}, {SCREEN_WIDTH, 80});
        m_Font = GA::CreateRef<CE::Font>("Data/Fonts/fa-solid-900.ttf");
        ButtonBorder = new Border(m_Scene, "ButtonBorder", {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, {SCREEN_WIDTH - 50, 400});
        StartTex = GA::CreateRef<CE::Texture2D>("Resources/startimg.png");
        ExitTex = GA::CreateRef<CE::Texture2D>("Resources/exitimg.png");

        TitleBorder->PushTextComponent("Title", Anchor::Center, "Kasino", m_Font, {1, 0, 0, 1}, {-55, -40});
        ButtonBorder->PushButtonComponent("Start", Anchor::Bottom, {0,0}, StartTex, {300, 150});
        ButtonBorder->PushButtonComponent("Exit", Anchor::Top, {0,0}, ExitTex, {300, 150});
    }
    void OnDetach() override{
        delete TitleBorder;
        delete ButtonBorder;
    }

    void OnUpdate(Timestep ts) override{
        CE::Renderer2D::ResetStats();        
        m_Scene->OnUpdateRuntime(ts);     
        
        m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0), ts); 

        ButtonBorder->GetComponent("Exit").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
            Application::Get().Close();
        };

        ButtonBorder->GetComponent("Start").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
            Application::Get().QueueLayerAction(LayerActionType::Push, new GameLayer());
            Application::Get().QueueLayerAction(LayerActionType::Pop, this);
        };
    }

    void OnImGuiRender() override{
        ImGui::Begin("Stats");
        				
        auto stats = CE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());                

    ImGui::End();
    }

    void OnEvent(Event& e) override{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(MainMenuLayer::OnResize));
    }

    bool OnResize(WindowResizeEvent& e){
        m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());
        return false;
    }
private:
    Border* TitleBorder;
    Border* ButtonBorder;
    GA::Ref<CE::Font> m_Font;
    GA::Ref<CE::Scene> m_Scene;   
    GA::Ref<CE::Texture2D> StartTex; 
    GA::Ref<CE::Texture2D> ExitTex;
};