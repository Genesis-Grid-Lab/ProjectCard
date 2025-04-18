#include "GameLayer.h"
#include "ImGui/ImGuiInputs.h"

GameLayer::GameLayer(){
    
}

GameLayer::~GameLayer(){
    delete mCard;
    delete mTable;
    delete mHand;
}

void GameLayer::OnAttach(){
    CE::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { CE::FramebufferTextureFormat::RGBA8, CE::FramebufferTextureFormat::RED_INTEGER, CE::FramebufferTextureFormat::Depth };
    fbSpec.Width = SCREEN_WIDTH;
    fbSpec.Height = SCREEN_HEIGHT;
    m_Framebuffer = GA::CreateRef<CE::Framebuffer>(fbSpec);
    m_Scene = GA::CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    CreateUI();
    
    mCard = new Deck(m_Scene);
    glm::vec2 tablePos = {SCREEN_WIDTH / 2, 200};
    glm::vec2 table1Pos = {tablePos.x, tablePos.y + 150};
    glm::vec2 tableSize = {Card_width * 6 + 30, Card_height};
    mTable = new Table(m_Scene, tablePos, tableSize);
    mTable1 = new Table(m_Scene, table1Pos, tableSize);

    glm::vec2 handPos = {SCREEN_WIDTH / 2, 1000};
    glm::vec2 handSize = {Card_width * 4 + 10, Card_height};
    mHand = new Hand(m_Scene, handPos, handSize);

    // auto& tc = mCard->GetID().GetComponent<TransformComponent>();
    // tc.Translation = { 720 / 2, 1280 / 2, 1};
    // tc.Scale = { 80, 120, 1};

    StartGame();
    
}

void GameLayer::CreateUI(){
    BackTex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Backgrounds/background_3.png");    
    AI1Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_1.png");    
    AI2Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_2.png");    
    AI3Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_3.png");    

    auto& BackGround = m_Scene->CreateEntity("BackGround");
    BackGround.AddComponent<CE::UIElement>().Texture = BackTex;
    auto& BGTC = BackGround.GetComponent<CE::TransformComponent>();
    BGTC.Translation = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0};
    BGTC.Scale = { 720, 1280, 1};
    
    float distButton = 150;
    float BuildWidthPos = (SCREEN_WIDTH / 2) - (distButton / 2); 
    float TrailWidthPos = (SCREEN_WIDTH / 2) + (distButton / 2);

    auto& TakeButton = m_Scene->CreateEntity("TakeButton");
    auto& TBUI = TakeButton.AddComponent<CE::ButtonComponent>();
    auto& TBTC = TakeButton.GetComponent<CE::TransformComponent>();

    TBTC.Translation = {BuildWidthPos - distButton, 1230, 5};
    TBTC.Scale = {120, 50, 1};
    TBUI.Color = { 1, 0, 0, 1};

    auto& BuildButton = m_Scene->CreateEntity("BuildButton");
    auto& BBUI = BuildButton.AddComponent<CE::ButtonComponent>();
    auto& BBTC = BuildButton.GetComponent<CE::TransformComponent>();

    BBTC.Translation = {BuildWidthPos , 1230, 5};
    BBTC.Scale = {120, 50, 1};
    BBUI.Color = { 0, 1, 0, 1};

    auto& TrailButton = m_Scene->CreateEntity("TrailButton");
    auto& TB2UI = TrailButton.AddComponent<CE::ButtonComponent>();
    auto& TB2TC = TrailButton.GetComponent<CE::TransformComponent>();
    
    TB2TC.Translation = {TrailWidthPos, 1230, 5};
    TB2TC.Scale = {120, 50, 1};
    TB2UI.Color = { 0, 0, 1, 1};

    auto& UndoButton = m_Scene->CreateEntity("UndoButton");
    auto& UBUI = UndoButton.AddComponent<CE::ButtonComponent>();
    auto& UBTC = UndoButton.GetComponent<CE::TransformComponent>();

    UBTC.Translation = {TrailWidthPos + distButton, 1230, 5};
    UBTC.Scale = {120, 50, 1};
    UBUI.Color = { 1, 1, 0, 1};

    auto& AI1 = m_Scene->CreateEntity("AI1");
    auto& AI1UI = AI1.AddComponent<CE::UIElement>();    
    AI1UI.Texture = AI1Tex;
    auto& AI1TC = AI1.GetComponent<CE::TransformComponent>();
    AI1TC.Translation = {50, 50, 5};
    AI1TC.Scale = { 50, 50, 1};

    auto& AI2 = m_Scene->CreateEntity("AI2");
    auto& AI2UI = AI2.AddComponent<CE::UIElement>();
    AI2UI.Texture = AI2Tex;
    auto& AI2TC = AI2.GetComponent<CE::TransformComponent>();
    AI2TC.Translation = {SCREEN_WIDTH / 2, 50, 5};
    AI2TC.Scale = { 50, 50, 1};

    auto& AI3 = m_Scene->CreateEntity("AI3");
    auto& AI3UI = AI3.AddComponent<CE::UIElement>();
    AI3UI.Texture = AI3Tex;
    auto& AI3TC = AI3.GetComponent<CE::TransformComponent>();
    AI3TC.Translation = {SCREEN_WIDTH - 50, 50, 5};
    AI3TC.Scale = { 50, 50, 1};

    auto& PlayerIcon = m_Scene->CreateEntity("PlayerIcon");
    PlayerIcon.AddComponent<CE::UIElement>().Color = {1, 0, 1, 1};
    auto& PITC = PlayerIcon.GetComponent<CE::TransformComponent>();
    PITC.Translation = {BuildWidthPos - distButton, 1150, 5};
    PITC.Scale = { 80, 80, 1};

    auto& PlayerPoints = m_Scene->CreateEntity("PlayerPoints");
    PlayerPoints.AddComponent<CE::UIElement>().Color = {1, 0, 1, 1};
    auto& PPTC = PlayerPoints.GetComponent<CE::TransformComponent>();
    PPTC.Translation = {TrailWidthPos + distButton, 1150, 5};
    PPTC.Scale = { 100, 80, 1};
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
    for(auto card : mCard->GetCards()){
        auto& tc = card->GetID().GetComponent<CE::TransformComponent>();
        if(m_HoveredEntity == card->GetID() && Input::IsMouseButtonPressed(0) && !card->OnTable ){            
            tc.Translation = {Input::GetMouseX(), Input::GetMouseY(), 4};
            card->Selected = true;       
        }
        
        // Releasing
        if (card->Selected && !Input::IsMouseButtonPressed(0)) {
            card->Selected = false;

            bool dropped = false;

            // Try table spots
            for (int i = 0; i < 6; ++i) {
                auto spot = mTable->GetSpot(i);
                if (spot->IsInside(tc.Translation)) {
                    spot->PushCard(card);
                    dropped = true;
                    break;
                }
            }

            for (int i = 0; i < 6; ++i) {
                auto spot = mTable1->GetSpot(i);
                if (spot->IsInside(tc.Translation)) {
                    spot->PushCard(card);
                    dropped = true;
                    break;
                }
            }

            // Return to hand if not dropped
            if (!dropped) {
                tc.Translation = card->LastPos;
                // tc.Translation = glm::mix(tc.Translation, card->LastPos, 0.5f);
            }
        }
        auto& sr = card->GetID().GetComponent<SpriteRendererComponent>();
        sr.Color = card->Selected ? glm::vec4(1, 1, 1, 0.8f) : glm::vec4(1, 1, 1, 1.0f);
        // tc.Translation = glm::mix(tc.Translation, card->OriginalPosition, 0.2f);
    }

    m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0));

}

void GameLayer::StartGame(){
    mHand->PushCard(mCard->GetCards()[0], 0);
    mHand->PushCard(mCard->GetCards()[1], 1);
    mHand->PushCard(mCard->GetCards()[2], 2);
    mHand->PushCard(mCard->GetCards()[3], 3);
    mTable->GetSpot(2)->PushCard(mCard->GetCards()[4]);
    mTable->GetSpot(3)->PushCard(mCard->GetCards()[5]);
    mTable1->GetSpot(2)->PushCard(mCard->GetCards()[6]);
    mTable1->GetSpot(3)->PushCard(mCard->GetCards()[7]);
   
        
}

void GameLayer::OnImGuiRender(){
    ImGui::Begin("Stats");
        
		std::string name = "None";
        int index = 0;
        if (m_HoveredEntity){
			name = m_HoveredEntity.GetComponent<CE::TagComponent>().Tag;
            index = m_HoveredEntity.GetComponent<CE::TransformComponent>().Translation.z;
        }
		ImGui::Text("Hovered Entity: %s", name.c_str());
        auto stats = CE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
        // ImGui::Text("selected: %i", mCard->Selected);
        ImGui::Text("index: %d", index);

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