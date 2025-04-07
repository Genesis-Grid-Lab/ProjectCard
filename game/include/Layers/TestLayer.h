#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "CardEngine.h"

#include <imgui.h>

class TestLayer : public Layer {
public:
    TestLayer(){

    }
    virtual ~TestLayer() = default;

    virtual void OnAttach() override{
        m_Cam = new CE::Camera(0.0f, 800.0f, 0.0f, 600.0f);
    }
    virtual void OnDetach() override{
        delete m_Cam;
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

        CE::Renderer2D::BeginCamera(*m_Cam);

        CE::Renderer2D::DrawQuad({100,200}, {10, 200}, {0.2, 10, 7, 1});

        CE::Renderer2D::EndCamera();
    }
    void OnEvent(Event& e) override{
        // CE_INFO("{0}", e.GetName());
    }

private:
    CE::Camera *m_Cam;
};