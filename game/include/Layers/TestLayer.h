#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "CardEngine.h"
#include "ImGui/ImGuiInputs.h"
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
        m_Tex2 = CreateRef<CE::Texture2D>("Resources/Textures/board.png");
        m_Scene = CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);        
        
        //entity        
        auto& button = m_Scene->CreateEntity("Button");
        auto& utc = button.GetComponent<CE::TransformComponent>();
        auto& uic = button.AddComponent<CE::ButtonComponent>();

        utc.Translation = { 720 / 2, 1000, 0};
        utc.Scale = {50, 50, 1};
        uic.Texture = m_Tex;

        uic.OnClick = []() {
            CE_INFO("CLISK");
        };
        
        auto& square = m_Scene->CreateEntity("Green Square");
        auto& sc = square.AddComponent<CE::SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
        auto& tc = square.GetComponent<CE::TransformComponent>();
        tc.Translation = {100, 100, 0};
        tc.Scale = {100, 100, 1};
        
        sc.Texture = m_Tex;
        
        auto& squareRed = m_Scene->CreateEntity("Red Square");
        auto& scr = squareRed.AddComponent<CE::SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
        auto& tcr = squareRed.GetComponent<CE::TransformComponent>();
        tcr.Translation = {100, 500, 0};
        tcr.Scale = {100, 100, 0};
        
        scr.Texture = m_Tex2;        


        auto& dummy = m_Scene->CreateEntity("dummt");
                
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

        int count = 0;

        m_Scene->ViewEntity<CE::Entity, CE::TransformComponent>([this, &count] (auto entity, auto& comp){            

            std::string trans = "Translation" + std::to_string(count);
            std::string rot = "Rotation" + std::to_string(count);
            std::string scal = "Scale" + std::to_string(count);
            InputVec3(trans.c_str(), &comp.Translation);
            InputVec3(rot.c_str(), &comp.Rotation);
            InputVec3(scal.c_str(), &comp.Scale);

            count++;
        });

        
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
            // CE_INFO("Pixel {0}",pixelData);
            // CE_INFO("mx: {0}, my: {1}" ,Input::GetMouseX(), Input::GetMouseY() );
            m_HoveredEntity = pixelData == -1 ? CE::Entity() : CE::Entity((entt::entity)pixelData, m_Scene.get());            
        }
        
        m_Framebuffer->Unbind();
        CE::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        CE::RenderCommand::Clear();
        
        m_Scene->DrawScreen(m_Framebuffer);       
        if(Input::IsMouseButtonPressed(0) && m_HoveredEntity){
            auto& tc = m_HoveredEntity.GetComponent<CE::TransformComponent>();
            tc.Translation = {Input::GetMouseX(), Input::GetMouseY(), tc.Translation.z};
        } 

        m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0));
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
    Ref<CE::Texture2D> m_Tex2;
    Ref<CE::Scene> m_Scene;
    CE::Entity m_HoveredEntity;    
    Ref<CE::Framebuffer> m_Framebuffer;    
};