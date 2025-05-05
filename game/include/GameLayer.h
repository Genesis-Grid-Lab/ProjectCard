#pragma once

#include <UrbanEngine.h>

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
    void PlayerTurn();
    void RunAITurn(PlayerType aiTurn);
    void ButtonLogic();
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
    Ref<UE::Scene> m_Scene;
    UE::Entity m_HoveredEntity;
    UE::Entity TakeButton;
    UE::Entity BuildButton;
    UE::Entity TrailButton;
    UE::Entity UndoButton;
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
    Ref<UE::Texture2D> BackTex;
    Ref<UE::Texture2D> AI1Tex;
    Ref<UE::Texture2D> AI2Tex;
    Ref<UE::Texture2D> AI3Tex; 
    Ref<UE::Texture2D> PlayerTex; 
    Ref<UE::Texture2D> TakeTex; 
    Ref<UE::Texture2D> BuildTex; 
    Ref<UE::Texture2D> TrailTex; 
    Ref<UE::Texture2D> UndoTex; 
    Ref<UE::Texture2D> SettingsTex; 
    Ref<UE::Font> mFont;    
private:
    bool m_AITurnInProgress = false;
    float m_AITurnTimer = 0.0f;
    bool m_ShowAITurnText = false;    
    TurnManager m_TurnManager;
    bool m_RoundInProgress = false;
    bool m_Paused = false;
    std::vector<Ref<Card>> GameCards;
    std::vector<Ref<Card>> PlayedCards;
    int selectedValue = 0;
    int spotValue = 0;
    int playerPoints = 0;
    Ref<Spot> workingSpot;
    Ref<Card> workingCard;
};