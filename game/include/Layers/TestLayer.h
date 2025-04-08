#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "CardEngine.h"
#include <glm/glm.hpp>
#include <imgui.h>

class TestLayer : public Layer {
public:
    TestLayer(){

    }
    virtual ~TestLayer() = default;

    virtual void OnAttach() override{
        m_Cam = CE::Camera(0.0f, 1600.0f, 900.0f, 0.0f);
        m_Tex = CreateRef<CE::Texture2D>("Resources/Textures/Checkerboard.png");
        m_Scene = CreateRef<CE::Scene>();            

        //entity        

        auto& square = m_Scene->CreateEntity("Green Square");
        auto& sc = square.AddComponent<CE::SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
        auto& tc = square.GetComponent<CE::TransformComponent>();
        tc.Translation = {100, 100, 0};
        tc.Scale = {100, 100, 1};

        sc.Texture = m_Tex;
        
    }
    virtual void OnDetach() override{
        
    }

    void OnImGuiRender() override{

        ImGui::Begin("Stats");

		std::string name = "None";
        auto stats = CE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();
    }

    void OnUpdate(Timestep ts) override{
        
        // Render
        CE::Renderer2D::ResetStats();

        CE::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        CE::RenderCommand::Clear();

        CE::Renderer2D::BeginCamera(m_Cam);

        m_Scene->OnUpdateRuntime(ts);
        CE::Renderer2D::DrawQuad({100,200}, {10, 200}, {0.2, 10, 7, 1});        
        
        CE::Renderer2D::EndCamera();
        
    }
    void OnEvent(Event& e) override{
        // CE_INFO("{0}", e.GetName());
    }

private:       
    CE::Camera m_Cam;
    Ref<CE::Texture2D> m_Tex;
    Ref<CE::Scene> m_Scene;
};