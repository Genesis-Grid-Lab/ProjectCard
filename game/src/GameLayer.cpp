#include "GameLayer.h"
#include "ImGui/ImGuiInputs.h"
#include <random>

GameLayer::GameLayer(){
    
}

GameLayer::~GameLayer(){
    delete mCard;
    delete mTable;
    delete mHand;
    delete mAI1Hand;
    delete mAI2Hand;
    delete mAI3Hand;
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
    glm::vec2 tableSize = {Card_width * 6 + 30, Card_height};
    mTable = new Table(m_Scene, tablePos, tableSize);

    glm::vec2 handPos = {SCREEN_WIDTH / 2, 1000};
    glm::vec2 handSize = {Card_width * 4 + 10, Card_height};
    mHand = new Hand(m_Scene, handPos, handSize);

    mAI1Hand = new AIHand();
    mAI2Hand = new AIHand();
    mAI3Hand = new AIHand();

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

    TakeButton = m_Scene->CreateEntity("TakeButton");
    auto& TBUI = TakeButton.AddComponent<CE::ButtonComponent>();
    auto& TBTC = TakeButton.GetComponent<CE::TransformComponent>();

    TBTC.Translation = {BuildWidthPos - distButton, 1230, 5};
    TBTC.Scale = {120, 50, 1};
    TBUI.Color = { 1, 0, 0, 1};
    TBUI.OriginalScale = TBTC.Scale;
    TBUI.TargetScale = TBTC.Scale;
    TBUI.BaseColor = TBUI.Color;
    TBUI.CurrentColor = TBUI.Color;

    BuildButton = m_Scene->CreateEntity("BuildButton");
    auto& BBUI = BuildButton.AddComponent<CE::ButtonComponent>();
    auto& BBTC = BuildButton.GetComponent<CE::TransformComponent>();

    BBTC.Translation = {BuildWidthPos , 1230, 5};
    BBTC.Scale = {120, 50, 1};
    BBUI.Color = { 0, 1, 0, 1};
    BBUI.OriginalScale = BBTC.Scale;
    BBUI.TargetScale = BBTC.Scale;
    BBUI.BaseColor = BBUI.Color;
    BBUI.CurrentColor = BBUI.Color;

    TrailButton = m_Scene->CreateEntity("TrailButton");
    auto& TB2UI = TrailButton.AddComponent<CE::ButtonComponent>();
    auto& TB2TC = TrailButton.GetComponent<CE::TransformComponent>();
    
    TB2TC.Translation = {TrailWidthPos, 1230, 5};
    TB2TC.Scale = {120, 50, 1};
    TB2UI.Color = { 0, 0, 1, 1};
    TB2UI.OriginalScale = TB2TC.Scale;
    TB2UI.TargetScale = TB2TC.Scale;
    TB2UI.BaseColor = TB2UI.Color;
    TB2UI.CurrentColor = TB2UI.Color;

    UndoButton = m_Scene->CreateEntity("UndoButton");
    auto& UBUI = UndoButton.AddComponent<CE::ButtonComponent>();
    auto& UBTC = UndoButton.GetComponent<CE::TransformComponent>();

    UBTC.Translation = {TrailWidthPos + distButton, 1230, 5};
    UBTC.Scale = {120, 50, 1};
    UBUI.Color = { 1, 1, 0, 1};
    UBUI.OriginalScale = UBTC.Scale;
    UBUI.TargetScale = UBTC.Scale;
    UBUI.BaseColor = UBUI.Color;
    UBUI.CurrentColor = UBUI.Color;

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
        // CE_INFO("Pixel {0}",pixelData);
        // CE_INFO("mx: {0}, my: {1}" ,Input::GetMouseX(), Input::GetMouseY() );
        m_HoveredEntity = pixelData == -1 ? CE::Entity() : CE::Entity((entt::entity)pixelData, m_Scene.get());            
    }
    
    m_Framebuffer->Unbind();
    CE::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    CE::RenderCommand::Clear();
    
    m_Scene->DrawScreen(m_Framebuffer);   
    for(auto card : PlayedCards){
        auto& tc = card->GetID().GetComponent<CE::TransformComponent>();
        
        // Only animate if not selected (i.e., being dragged)
        if (!card->Selected) {
            tc.Translation = glm::mix(tc.Translation, card->TargetPos, 0.2f);
        }
        if(m_TurnManager.IsHumanTurn()){            
            
            if(m_HoveredEntity == card->GetID() && Input::IsMouseButtonPressed(0) && !card->OnTable ){            
                tc.Translation = {Input::GetMouseX(), Input::GetMouseY(), 4};
                card->Selected = true;      
                selectedValue = card->GetValue();
            }
            // Releasing
            if (card->Selected && !Input::IsMouseButtonPressed(0)) {
                card->Selected = false;
    
                bool dropped = false;
    
                // Try table spots
                for (int i = 0; i < mTable->GetSpotCount(); ++i) {
                    auto spot = mTable->GetSpot(i);
                    if (spot->IsInside(tc.Translation)) {
                        spot->PushCard(card, mHand->GetCards(), PlayedCards);
                        dropped = true;
                        break;
                    }
                }
    
                // Return to hand if not dropped
                if (!dropped) {
                    tc.Translation = card->LastPos;
                    selectedValue = 0;
                    // tc.Translation = glm::mix(tc.Translation, card->LastPos, 0.5f);
                }
            }
        }

        
        auto& sr = card->GetID().GetComponent<SpriteRendererComponent>();
        sr.Color = card->Selected ? glm::vec4(1, 1, 1, 0.8f) : glm::vec4(1, 1, 1, 1.0f);
        // tc.Translation = glm::mix(tc.Translation, card->OriginalPosition, 0.2f);
    }

    TakeButton.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        // if(selectedValue == )
        m_AITurnTimer = 2.0f; // 2 second delay before AI acts
        m_TurnManager.NextTurn();
        CE_INFO("TAKE");
    };

    BuildButton.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        CE_INFO("BUILD");
    };

    TrailButton.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        
        CE_INFO("TRAIL");
    };

    UndoButton.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        CE_INFO("UNDO");
    };

    if (!m_TurnManager.IsHumanTurn()) {
        m_AITurnTimer -= ts;
        m_ShowAITurnText = true;
        if(m_AITurnTimer <= 0.0f){
            RunAITurn(m_TurnManager.Current);
            m_TurnManager.NextTurn();
            m_AITurnTimer = 2.0f;
            m_ShowAITurnText = false;
            if (m_TurnManager.IsRoundOver()) {
                m_RoundInProgress = false;
                if (GameCards.size() == 0) {
                    // Game over
                    CE_INFO("Game Over - Deck is empty");
                } else {
                    DealNewRound();
                }
            }
        }
        
    }
    
    
    m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0), ts);
    debug = true;
}

GA::Ref<Card> PickBestCard(std::vector<GA::Ref<Card>>& cards){
    // Pick best card from hand
	auto bestCardIt = std::max_element(cards.begin(), cards.end(),
    [](const GA::Ref<Card>& a, const GA::Ref<Card>& b) {
        return a->GetValue() < b->GetValue();
    });

    return *bestCardIt;
}

void GameLayer::RunAITurn(PlayerType aiTurn) {
	CE_INFO("Running AI turn: {0}", (int)aiTurn);

	// int aiIndex = (int)aiTurn - 1; // Turn::AI1 == 1
	// if (aiIndex < 0 || aiIndex >= (int)m_AIHands.size())
	// 	return;


	// GA::Ref<Card> bestCard = *bestCardIt;	

    switch (aiTurn)
    {
    case PlayerType::AI1:
        {

            // Find a spot to play it
            for (int i = 0; i < 6; ++i) {
                auto spot = mTable->GetSpot(i);
                if (spot->IsAvailable()) {
                    CE_INFO(" ai1 card play: {0}", PickBestCard(mAI1Hand->GetCards())->GetValue());
                    spot->PushCard(PickBestCard(mAI1Hand->GetCards()), mAI1Hand->GetCards(), PlayedCards);
                    return;
                }
            }
            break;
        }
        case PlayerType::AI2:
        {

            // Find a spot to play it
            for (int i = 0; i < 6; ++i) {
                auto spot = mTable->GetSpot(i);
                if (spot->IsAvailable()) {
                    CE_INFO(" ai2 card play: {0}", PickBestCard(mAI2Hand->GetCards())->GetValue());
                    spot->PushCard(PickBestCard(mAI2Hand->GetCards()), mAI2Hand->GetCards(), PlayedCards);
                    return;
                }
            }
            break;
        }
        case PlayerType::AI3:
        {

            // Find a spot to play it
            for (int i = 0; i < 6; ++i) {
                auto spot = mTable->GetSpot(i);
                if (spot->IsAvailable()) {
                    CE_INFO(" ai3 card play: {0}", PickBestCard(mAI3Hand->GetCards())->GetValue());
                    spot->PushCard(PickBestCard(mAI3Hand->GetCards()), mAI3Hand->GetCards(), PlayedCards);
                    return;
                }
            }
            break;
        }
    
    default:
        break;
    }
    
}

void GameLayer::DealNewRound() {

	// Clear previous round hands if needed (optional)

	// Player hand
	for (int i = 0; i < 4; ++i) {
		mHand->PushCard(GameCards[i], GameCards,PlayedCards, i);	
        mAI1Hand->PushCard(GameCards[i], GameCards,PlayedCards, i);
        mAI2Hand->PushCard(GameCards[i], GameCards,PlayedCards, i);
        mAI3Hand->PushCard(GameCards[i], GameCards,PlayedCards, i);
	}	

    m_TurnManager.ResetRound();
    m_RoundInProgress = true;

	CE_INFO("New round dealt. Cards remaining: {0}", (int)GameCards.size());
}


void GameLayer::StartGame(){
    GameCards = mCard->GetCards();
    std::shuffle(GameCards.begin(), GameCards.end(), std::default_random_engine());
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(GameCards), std::end(GameCards), g);

	// Place next cards on table
    mTable->AddSpotAndPushCard(GameCards[4],GameCards,PlayedCards);
    mTable->AddSpotAndPushCard(GameCards[5],GameCards,PlayedCards);
    mTable->AddSpotAndPushCard(GameCards[6],GameCards,PlayedCards);
    mTable->AddSpotAndPushCard(GameCards[7],GameCards,PlayedCards);    
        
    DealNewRound();
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
        if (m_ShowAITurnText) {
            ImGui::Begin("Turn Info");
            ImGui::Text("AI %d is thinking...", (int)m_TurnManager.Current);
            ImGui::End();
        }

    ImGui::End();
    ImGui::Begin("Game");
    ImGui::Text("Selected Value: %d", selectedValue);
    ImGui::Text("Current turn: %d", (int)m_TurnManager.Current);
        ImGui::Text("AI1:");
        ImGui::Text("Number of Cards: %d",mAI1Hand->GetCards().size());
        for(auto card : mAI1Hand->GetCards()){
            if(debug)
                ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("AI2:");
        ImGui::Text("Number of Cards: %d",mAI2Hand->GetCards().size());
        for(auto card : mAI2Hand->GetCards()){
            if(debug)
                ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("AI3:");
        ImGui::Text("Number of Cards: %d",mAI3Hand->GetCards().size());
        for(auto card : mAI3Hand->GetCards()){
            if(debug)
                ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("Player:");
        ImGui::Text("Number of Cards: %d", mHand->GetCards().size());
        for(auto card : mHand->GetCards()){
            if(debug)
                ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("Game:");
        ImGui::Text("Number of GameCards: %d",GameCards.size());
        ImGui::Text("Number of PlayedCards: %d",PlayedCards.size());
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