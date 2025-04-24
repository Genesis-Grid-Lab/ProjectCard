#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Events/ApplicationEvent.h"
#include "Config.h"
#include "Game/Card.h"
#include "Game/UI.h"

enum class PlayerType { Human, AI1, AI2, AI3 };

class TurnManager {
public:
    PlayerType Current = PlayerType::Human;
    int TurnCount = 0; // Counts how many cards have been played in this round
    bool Played = false;

    void NextTurn() {
        Current = static_cast<PlayerType>((static_cast<int>(Current) + 1) % 4);
        TurnCount++;
    }

    bool IsRoundOver() const {
        return TurnCount >= 16;
    }

    void ResetRound() {
        TurnCount = 0;
        Current = PlayerType::Human;
    }

    bool IsHumanTurn() const {
        return Current == PlayerType::Human;
    }
};

class GameLayer : public Layer {
public:
    GameLayer();
    ~GameLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    void StartGame();
    void DealNewRound();
    void RunAITurn(PlayerType aiTurn);
    void Shuffle();

    void TakeButtonFnc(PlayerType player);
    void BuildButtonFnc(PlayerType player);
    void TrailButtonFnc(PlayerType player);
    void UndoButtonFnc();

    void CreateUI();

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;

    bool OnResize(WindowResizeEvent& e);

private:
    GA::Ref<CE::Scene> m_Scene;
    CE::Entity m_HoveredEntity;
    CE::Entity TakeButton;
    CE::Entity BuildButton;
    CE::Entity TrailButton;
    CE::Entity UndoButton;
    glm::vec3 OldPos;
    Border* mAIBorder;
    Border* mPlayerBorder;
    Border* mButtonBorder;
    Deck *mCard;
    Table *mTable;    
    Hand *mHand;
    AIHand *mAI1Hand;
    AIHand *mAI2Hand;
    AIHand *mAI3Hand;
    GA::Ref<CE::Framebuffer> m_Framebuffer;
    GA::Ref<CE::Texture2D> BackTex;
    GA::Ref<CE::Texture2D> AI1Tex;
    GA::Ref<CE::Texture2D> AI2Tex;
    GA::Ref<CE::Texture2D> AI3Tex; 
    GA::Ref<CE::Texture2D> PlayerTex; 
    GA::Ref<CE::Texture2D> TakeTex; 
    GA::Ref<CE::Texture2D> BuildTex; 
    GA::Ref<CE::Texture2D> TrailTex; 
    GA::Ref<CE::Texture2D> UndoTex; 
    GA::Ref<CE::Texture2D> SettingsTex; 
    bool debug = false;   
private:
    bool m_AITurnInProgress = false;
    float m_AITurnTimer = 0.0f;
    bool m_ShowAITurnText = false;    
    TurnManager m_TurnManager;
    bool m_RoundInProgress = false;
    std::vector<GA::Ref<Card>> GameCards;
    std::vector<GA::Ref<Card>> PlayedCards;
    int selectedValue = 0;
    int spotValue = 0;
    int playerPoints = 0;
    GA::Ref<Spot> workingSpot;
    GA::Ref<Card> workingCard;
};