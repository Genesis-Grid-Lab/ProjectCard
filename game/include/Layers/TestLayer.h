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
        CE::FramebufferSpecification fbSpec;
		fbSpec.Attachments = { CE::FramebufferTextureFormat::RGBA8, CE::FramebufferTextureFormat::RED_INTEGER, CE::FramebufferTextureFormat::Depth };
		fbSpec.Width = SCREEN_WIDTH;
		fbSpec.Height = SCREEN_HEIGHT;
		m_Framebuffer = GA::CreateRef<CE::Framebuffer>(fbSpec);

        m_Tex = CreateRef<CE::Texture2D>("Resources/Textures/Checkerboard.png");
        m_Scene = CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);        
        
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
        if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<CE::TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
        auto stats = CE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

        ImGui::Begin("View");
        ImTextureID textureID = m_Framebuffer->GetColorAttachmentRendererID();
        ImGui::Image(textureID, ImVec2{SCREEN_WIDTH, SCREEN_HEIGHT}, ImVec2{0, 1}, ImVec2{1,0});
        ImGui::End();
    }

    void OnUpdate(Timestep ts) override{       
        
        // Render
        CE::Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        CE::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        CE::RenderCommand::Clear();

        // Clear our entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);

        m_Scene->OnUpdateRuntime(ts);   

        auto my = Input::GetMouseY();
        auto mx = Input::GetMouseX();

        my = SCREEN_HEIGHT - my;
        // mx = 1600 - mx;
        
        if(Input::GetMouseX() >= 0 && Input::GetMouseY() >= 0 && Input::GetMouseX() < SCREEN_WIDTH && Input::GetMouseY() < SCREEN_HEIGHT){            
            int pixelData = m_Framebuffer->ReadPixel(1, mx, my);
            CE_INFO("Pixel {0}",pixelData);
            CE_INFO("mx: {0}, my: {1}" ,Input::GetMouseX(), Input::GetMouseY() );
            m_HoveredEntity = pixelData == -1 ? CE::Entity() : CE::Entity((entt::entity)pixelData, m_Scene.get());            
        }
        
        m_Framebuffer->Unbind();
        CE::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        CE::RenderCommand::Clear();
        
        m_Scene->DrawScreen(m_Framebuffer);        
    }
    void OnEvent(Event& e) override{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(TestLayer::OnResize));
    }

    bool OnResize(WindowResizeEvent& e){
        m_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
        m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());

        return false;
    }

private:           
    Ref<CE::Texture2D> m_Tex;
    Ref<CE::Scene> m_Scene;
    CE::Entity m_HoveredEntity;
    Ref<CE::Framebuffer> m_Framebuffer;    
};