#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Events/ApplicationEvent.h"
#include "Config.h"
#include "Game/Card.h"

class GameLayer : public Layer {
public:
    GameLayer();
    ~GameLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    void StartGame();

    void CreateUI();

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;

    bool OnResize(WindowResizeEvent& e);

private:
    GA::Ref<CE::Scene> m_Scene;
    CE::Entity m_HoveredEntity;
    glm::vec3 OldPos;
    Deck *mCard;
    Table *mTable;
    Table *mTable1;
    Hand *mHand;
    GA::Ref<CE::Framebuffer> m_Framebuffer;
    GA::Ref<CE::Texture2D> BackTex;
    GA::Ref<CE::Texture2D> AI1Tex;
    GA::Ref<CE::Texture2D> AI2Tex;
    GA::Ref<CE::Texture2D> AI3Tex;
    // Card* Test;
};