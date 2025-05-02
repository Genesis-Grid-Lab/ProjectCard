#include "GameLayer.h"
#include "ImGui/ImGuiInputs.h"
#include <random>
#include "Game/InGameMenuLayer.h"

GameLayer::GameLayer(){
    
}

GameLayer::~GameLayer(){
    // OnDetach();
}

void GameLayer::OnAttach(){
    m_Scene = GA::CreateRef<CE::Scene>(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    CreateUI();
    
    mCard = new Deck(m_Scene);
    glm::vec2 tablePos = {SCREEN_WIDTH / 2, 300};
    glm::vec2 tableSize = {Card_width * 6 + 30, Card_height};
    mTable = new Table(m_Scene, tablePos, tableSize);

    glm::vec2 handPos = {SCREEN_WIDTH / 2, 1000};
    glm::vec2 handSize = {Card_width * 4 + 10, Card_height};
    mHand = new Hand(m_Scene, handPos, handSize);

    mAI1Hand = new AIHand();
    mAI2Hand = new AIHand();
    mAI3Hand = new AIHand();

    StartGame();
    
}

void GameLayer::CreateUI(){
    BackTex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Backgrounds/background_3.png");    
    AI1Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_1.png");    
    AI2Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_2.png");    
    AI3Tex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_3.png");    
    PlayerTex = GA::CreateRef<CE::Texture2D>("Resources/Cards/Characters/char_12.png");   
    TakeTex = GA::CreateRef<CE::Texture2D>("Resources/takeimg.png");
    BuildTex = GA::CreateRef<CE::Texture2D>("Resources/buildimg.png");
    TrailTex = GA::CreateRef<CE::Texture2D>("Resources/trail.png");
    UndoTex = GA::CreateRef<CE::Texture2D>("Resources/undoimg.png"); 
    SettingsTex = GA::CreateRef<CE::Texture2D>("Resources/settingimgpng.png"); 
    mFont = GA::CreateRef<CE::Font>("Data/Fonts/fa-solid-900.ttf");

    mPlayerBorder = new Border(m_Scene, "PlayerBorder", {SCREEN_WIDTH / 2, 40}, {SCREEN_WIDTH, 80});
    mAIBorder = new Border(m_Scene, "AIBorder", {SCREEN_WIDTH / 2, 1150}, {SCREEN_WIDTH, 50});
    mButtonBorder = new Border(m_Scene, "ButtonBorder", {SCREEN_WIDTH / 2, 1230}, {SCREEN_WIDTH, 50});

    auto& BackGround = m_Scene->CreateEntity("BackGround");
    BackGround.AddComponent<CE::UIElement>().Texture = BackTex;
    auto& BGTC = BackGround.GetComponent<CE::TransformComponent>();
    BGTC.Translation = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0};
    BGTC.Scale = { 720, 1280, 1};

    int buttonOffset = 50;

    mButtonBorder->PushButtonComponent("TakeButton", Anchor::Left, {30,0}, TakeTex, {120, 50});

    mButtonBorder->PushButtonComponent("BuildButton", Anchor::Left, {150 + buttonOffset,0}, BuildTex, {120, 50});   

    mButtonBorder->PushButtonComponent("TrailButton", Anchor::Right, {-150 - buttonOffset,0}, TrailTex, {120, 50});

    mButtonBorder->PushButtonComponent("UndoButton", Anchor::Right, {-30,0}, UndoTex, {120, 50});

    mAIBorder->PushComponent("AI1", Anchor::Left,{0,0}, AI1Tex, {50, 50});    
    mAIBorder->PushComponent("AI2", Anchor::Center,{0,0}, AI2Tex, {50, 50});    
    mAIBorder->PushComponent("AI3",Anchor::Right, {0,0}, AI3Tex , {50, 50});

    mPlayerBorder->PushComponent("PlayerIcon", Anchor::Left, {10, 0} , PlayerTex, {80, 80});  
    mPlayerBorder->PushComponent("PlayerPointsBack", Anchor::Right, {-65, 0}, nullptr, {100, 80}, {1,0,1,1});
    mPlayerBorder->PushButtonComponent("SettingsButton", Anchor::Right, {-10,0}, SettingsTex, {50, 80});  
    
}

void GameLayer::OnDetach(){
    delete mCard;
    delete mTable;
    delete mHand;
    delete mAI1Hand;
    delete mAI2Hand;
    delete mAI3Hand;
    delete mAIBorder;
    delete mPlayerBorder;
    delete mButtonBorder;
}

void GameLayer::OnUpdate(Timestep ts){
    
    if(!m_Paused){
        m_HoveredEntity = m_Scene->GetHoveredEntity();
        // Render
        CE::Renderer2D::ResetStats();    
        m_Scene->OnUpdateRuntime(ts);   
        
        PlayerTurn();

        mPlayerBorder->PushTextComponent("PlayerPoints", Anchor::Top,std::to_string(playerPoints),mFont,
                                                             glm::vec4(1));

        ButtonLogic();

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
    }  

    // CE_INFO("PAUSED: {0}", m_Paused);
}

void GameLayer::PlayerTurn(){
    for(auto card : PlayedCards){
        auto& tc = card->GetID().GetComponent<CE::TransformComponent>();
        
        // Only animate if not selected (i.e., being dragged)
        if (!card->Selected) {
            tc.Translation = glm::mix(tc.Translation, card->TargetPos, 0.2f);
        }
        if(m_TurnManager.IsHumanTurn()){            
            
            if(m_HoveredEntity == card->GetID() && Input::IsMouseButtonPressed(0) && !card->OnTable && !m_TurnManager.Played){            
                tc.Translation = {Input::GetMouseX(), Input::GetMouseY(), 4};
                card->Selected = true;      
                selectedValue = card->GetValue();
            }
            // Releasing
            if (card->Selected && !Input::IsMouseButtonPressed(0)) {
                card->Selected = false;
    
                bool dropped = false;
                bool spotEmpty = false;
                // Try table spots
                for (int i = 0; i < mTable->GetSpotCount(); ++i) {
                    auto spot = mTable->GetSpot(i);
                    if (spot->IsInside(tc.Translation)) {
                        if(!spot->GetCards().empty())
                            spotValue = spot->GetCards().front()->GetValue();
                        workingSpot = spot;
                        workingCard = card;
                        spot->PushCard(card, mHand->GetCards(), PlayedCards);
                        dropped = true;
                        spotEmpty = true;
                        m_TurnManager.Played = true;
                        break;
                    }
                }

                auto mouseX = Input::GetMouseX();
                auto mouseY = Input::GetMouseY();

                auto& transform = mTable->GetID().GetComponent<CE::TransformComponent>();
                float halfWidth  = transform.Scale.x * 0.5f;
                float halfHeight = transform.Scale.y * 0.5f;

                bool hovered = mouseX >= (transform.Translation.x - halfWidth) &&
                mouseX <= (transform.Translation.x + halfWidth) &&
                mouseY >= (transform.Translation.y - halfHeight) &&
                mouseY <= (transform.Translation.y + halfHeight);

                if(hovered && !spotEmpty){
                    workingCard = card;
                    for(auto& spot : mTable->GetAllSpots()){
                        if(spot->IsEmpty()){
                            spot->PushCard(card, mHand->GetCards(), PlayedCards);
                            break;
                        }else{

                            mTable->AddSpotAndPushCard(card, mHand->GetCards(), PlayedCards);
                            break;
                        }
                    }
                    dropped = true;
                    m_TurnManager.Played = true;
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
}

void GameLayer::ButtonLogic(){
    mButtonBorder->GetComponent("TakeButton").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        m_AITurnTimer = 1.0f; // 2 second delay before AI acts
        if(selectedValue == spotValue){
            m_TurnManager.Played = false;
            TakeButtonFnc(PlayerType::Human);
            m_TurnManager.NextTurn();
        }else{
            CE_INFO("NOT EQUAL");
            if(workingCard){
                CE_INFO("working");
                workingCard->GetID().GetComponent<CE::TransformComponent>().Translation = workingCard->LastPos;
                m_TurnManager.Played = false;
            }
        }
    };

    mButtonBorder->GetComponent("BuildButton").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        m_TurnManager.Played = false;
        BuildButtonFnc(PlayerType::Human);
        m_TurnManager.NextTurn();
        CE_INFO("BUILD");
    };

    mButtonBorder->GetComponent("TrailButton").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        m_TurnManager.Played = false;
        m_TurnManager.NextTurn();
        CE_INFO("TRAIL");
    };

    mButtonBorder->GetComponent("UndoButton").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){        
       UndoButtonFnc();
    };

    mPlayerBorder->GetComponent("SettingsButton").entity.GetComponent<CE::ButtonComponent>().OnClick = [this](){
        m_Paused = true;
        Application::Get().QueueLayerAction(LayerActionType::Push, new InGameMenuLayer(&m_Paused));
    };
}

void GameLayer::TakeButtonFnc(PlayerType player){
    CE_INFO("TAKE");
    if(selectedValue == spotValue){
        for(auto& spot : mTable->GetAllSpots()){
            
            if(spot == workingSpot){

                for(auto& card : spot->GetCards()){
                    
                    if(card == workingCard){
                        spot->RemoveCard(card);      
                        switch (player)
                        {
                        case PlayerType::Human:
                            playerPoints++;
                            break;
                        
                        default:
                            break;
                        }

                    }
                }
                spot->RemoveCard(spot->GetCards().front());  
                switch (player)
                {
                case PlayerType::Human:
                    playerPoints++;
                    break;
                
                default:
                    break;
                }              
            }
        }        
    }
}

void GameLayer::BuildButtonFnc(PlayerType player){
    if (!workingCard || !workingSpot || selectedValue == 0)
        return;

    for (auto& spot : mTable->GetAllSpots()) {
        if (!spot->IsEmpty()) {
            int spotCardValue = spot->GetCards().front()->GetValue();
            int combined = selectedValue + spotCardValue;

            // Search for a card with that combined value in hand
            for (auto& handCard : mHand->GetCards()) {
                if (handCard != workingCard && handCard->GetValue() == combined) {
                    // Perform build
                    spot->PushCard(workingCard, mHand->GetCards(), PlayedCards);
                    spot->PushCard(handCard, mHand->GetCards(), PlayedCards);
                    m_TurnManager.Played = true;
                    m_TurnManager.NextTurn();
                    CE_INFO("BUILD SUCCESS: {0} + {1} = {2}", selectedValue, spotCardValue, combined);
                    return;
                }
            }
        }
    }

    // If no build, reset card position
    if (workingCard) {
        workingCard->GetID().GetComponent<TransformComponent>().Translation = workingCard->LastPos;
    }

}
void GameLayer::TrailButtonFnc(PlayerType player){

}
void GameLayer::UndoButtonFnc(){
    if (!workingCard) return;

    workingCard->GetID().GetComponent<CE::TransformComponent>().Translation = workingCard->LastPos;

    // Optional: remove card from table if already pushed
    // for (auto& spot : mTable->GetAllSpots()) {
    //     spot->RemoveCard(workingCard); // You need a RemoveCard method in Spot
    // }

    m_TurnManager.Played = false;
    CE_INFO("Move undone.");
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

            // // Find a spot to play it
            // for (int i = 0; i < 6; ++i) {
            //     auto spot = mTable->GetSpot(i);
            //     if (spot->IsAvailable()) {
            //         CE_INFO(" ai1 card play: {0}", PickBestCard(mAI1Hand->GetCards())->GetValue());
            //         spot->PushCard(PickBestCard(mAI1Hand->GetCards()), mAI1Hand->GetCards(), PlayedCards);
            //         return;
            //     }
            // }

            bool noSpot = true;

            for(auto& spot : mTable->GetAllSpots()){
                if(!spot->IsEmpty()){
                    if(spot->GetCards().front()->GetValue() == PickBestCard(mAI1Hand->GetCards())->GetValue()){
                        workingSpot = spot;
                        workingCard = PickBestCard(mAI1Hand->GetCards());
                        CE_INFO(" ai1 card TAKE: {0}", PickBestCard(mAI1Hand->GetCards())->GetValue());
                        TakeButtonFnc(PlayerType::AI1);
                        noSpot = false;
                        return;
                    }                    
                }else{                    
                    spot->PushCard(PickBestCard(mAI1Hand->GetCards()), mAI1Hand->GetCards(), PlayedCards);
                    noSpot = false;
                    CE_INFO(" ai1 card PLAY: {0}", PickBestCard(mAI1Hand->GetCards())->GetValue());
                    return;
                }                

            }

            if(noSpot){
                mTable->AddSpotAndPushCard(PickBestCard(mAI1Hand->GetCards()), mAI1Hand->GetCards(), PlayedCards);
                CE_INFO(" ai1 card PLAY AND PSUH: {0}", PickBestCard(mAI1Hand->GetCards())->GetValue());
            }
            break;
        }
        case PlayerType::AI2:
        {

            // Find a spot to play it
            // for (int i = 0; i < 6; ++i) {
            //     auto spot = mTable->GetSpot(i);
            //     if (spot->IsAvailable()) {
            //         CE_INFO(" ai2 card play: {0}", PickBestCard(mAI2Hand->GetCards())->GetValue());
            //         spot->PushCard(PickBestCard(mAI2Hand->GetCards()), mAI2Hand->GetCards(), PlayedCards);
            //         return;
            //     }
            // }

            bool noSpot = true;

            for(auto& spot : mTable->GetAllSpots()){
                if(!spot->IsEmpty()){
                    if(spot->GetCards().front()->GetValue() == PickBestCard(mAI2Hand->GetCards())->GetValue()){
                        workingSpot = spot;
                        workingCard = PickBestCard(mAI2Hand->GetCards());
                        noSpot = false;
                        CE_INFO(" ai2 card TAKE: {0}", PickBestCard(mAI2Hand->GetCards())->GetValue());
                        TakeButtonFnc(PlayerType::AI1);
                        return;
                    }                    
                }else{                    
                    spot->PushCard(PickBestCard(mAI2Hand->GetCards()), mAI2Hand->GetCards(), PlayedCards);
                    noSpot = false;
                    CE_INFO(" ai2 card PLAY: {0}", PickBestCard(mAI2Hand->GetCards())->GetValue());
                    return;
                }

            }

            if(noSpot){
                mTable->AddSpotAndPushCard(PickBestCard(mAI2Hand->GetCards()), mAI2Hand->GetCards(), PlayedCards);
                CE_INFO(" ai2 card PLAY AND PUSH: {0}", PickBestCard(mAI2Hand->GetCards())->GetValue());
            }
            break;
        }
        case PlayerType::AI3:
        {

            // Find a spot to play it
            // for (int i = 0; i < 6; ++i) {
            //     auto spot = mTable->GetSpot(i);
            //     if (spot->IsAvailable()) {
            //         CE_INFO(" ai3 card play: {0}", PickBestCard(mAI3Hand->GetCards())->GetValue());
            //         spot->PushCard(PickBestCard(mAI3Hand->GetCards()), mAI3Hand->GetCards(), PlayedCards);
            //         return;
            //     }
            // }

            bool noSpot = true;

            for(auto& spot : mTable->GetAllSpots()){
                if(!spot->IsEmpty()){
                    if(spot->GetCards().front()->GetValue() == PickBestCard(mAI3Hand->GetCards())->GetValue()){
                        workingSpot = spot;
                        workingCard = PickBestCard(mAI3Hand->GetCards());
                        CE_INFO(" ai2 card TAKE: {0}", PickBestCard(mAI3Hand->GetCards())->GetValue());
                        noSpot = false;
                        TakeButtonFnc(PlayerType::AI1);
                        return;
                    }                    
                }else{
                    spot->PushCard(PickBestCard(mAI3Hand->GetCards()), mAI3Hand->GetCards(), PlayedCards);
                    noSpot = false;
                    CE_INFO(" ai2 card PLAY: {0}", PickBestCard(mAI3Hand->GetCards())->GetValue());
                    return;
                }

            }

            if(noSpot){
                mTable->AddSpotAndPushCard(PickBestCard(mAI3Hand->GetCards()), mAI3Hand->GetCards(), PlayedCards);
                CE_INFO(" ai2 card PLAY AND PUSH: {0}", PickBestCard(mAI3Hand->GetCards())->GetValue());
            }
            break;
        }
    
    default:
        break;
    }
    
}

void GameLayer::DealNewRound() {

    Shuffle();
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

void GameLayer::Shuffle(){
    std::shuffle(GameCards.begin(), GameCards.end(), std::default_random_engine());
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(std::begin(GameCards), std::end(GameCards), g);
}

void GameLayer::StartGame(){
    GameCards = mCard->GetCards();
    
    Shuffle();

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
        // if (m_HoveredEntity){
		// 	name = m_HoveredEntity.GetComponent<CE::TagComponent>().Tag;
        //     index = m_HoveredEntity.GetComponent<CE::TransformComponent>().Translation.z;
        // }
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
    ImGui::Text("Spot Value: %d", spotValue);
    ImGui::Text("Current turn: %d", (int)m_TurnManager.Current);
        ImGui::Text("AI1:");
        ImGui::Text("Number of Cards: %d",mAI1Hand->GetCards().size());
        for(auto card : mAI1Hand->GetCards()){            
            ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("AI2:");
        ImGui::Text("Number of Cards: %d",mAI2Hand->GetCards().size());
        for(auto card : mAI2Hand->GetCards()){
            
            ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("AI3:");
        ImGui::Text("Number of Cards: %d",mAI3Hand->GetCards().size());
        for(auto card : mAI3Hand->GetCards()){
            
            ImGui::Text("Card: %d", card->m_Value);
        }
        ImGui::Separator();
        ImGui::Text("Player:");
        ImGui::Text("Number of Cards: %d", mHand->GetCards().size());
        for(auto card : mHand->GetCards()){
            
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
    m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());

    return false;
}