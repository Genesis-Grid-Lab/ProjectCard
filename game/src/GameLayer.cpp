#include "GameLayer.h"
#include "ImGui/ImGuiInputs.h"

GameLayer::GameLayer(){

}

GameLayer::~GameLayer(){

}

void GameLayer::OnAttach(){
    CE::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { CE::FramebufferTextureFormat::RGBA8, CE::FramebufferTextureFormat::RED_INTEGER, CE::FramebufferTextureFormat::Depth };
    fbSpec.Width = SCREEN_WIDTH;
    fbSpec.Height = SCREEN_HEIGHT;
    m_Framebuffer = GA::CreateRef<CE::Framebuffer>(fbSpec);

    m_Scene = GA::CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);  
}

void GameLayer::OnDetach(){

}

void GameLayer::OnUpdate(Timestep ts){

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
    if(Input::IsMouseButtonPressed(0) && m_HoveredEntity){
        auto& tc = m_HoveredEntity.GetComponent<CE::TransformComponent>();
        tc.Translation = {Input::GetMouseX(), Input::GetMouseY(), tc.Translation.z};
    } 

    m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0));

}

void GameLayer::OnImGuiRender(){
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
}

void GameLayer::OnEvent(Event& e){
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(GameLayer::OnResize));
}

bool GameLayer::OnResize(WindowResizeEvent& e){
    m_Framebuffer->Resize(e.GetWidth(), e.GetHeight());
    m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());

    return false;
}