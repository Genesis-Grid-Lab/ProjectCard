#pragma once
#include <CardEngine.h>

using namespace CE;

class Card {
public:
    Card(GA::Ref<Scene>& scene)
    : m_SceneRef(scene){
        m_Tex = GA::CreateRef<Texture2D>("Resources/Cards/Standard/normal_cards/individual/card back/cardBackBlue.png");
        ID = scene->CreateEntity("Card");
        ID.AddComponent<SpriteRendererComponent>().Texture = m_Tex;
        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = { 720 / 2, 1280 / 2, 1};
        tc.Scale = { 80, 120, 1};
    }
    ~Card(){
        
    }
private:
    Entity ID;
    GA::Ref<Scene> m_SceneRef;
    GA::Ref<Texture2D> m_Tex;
};