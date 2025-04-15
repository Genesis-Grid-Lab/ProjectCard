#pragma once

#include <CardEngine.h>
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Events/ApplicationEvent.h"
#include "Config.h"

class GameLayer : public Layer {
public:
    GameLayer();
    ~GameLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnImGuiRender() override;

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& e) override;

    bool OnResize(WindowResizeEvent& e);

private:
    GA::Ref<CE::Scene> m_Scene;
    CE::Entity m_HoveredEntity;
    GA::Ref<CE::Framebuffer> m_Framebuffer;
};