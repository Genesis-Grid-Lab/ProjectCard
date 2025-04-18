#pragma once
#include <CardEngine.h>
#include <filesystem>
#include <string>
#include <array>

using namespace CE;

#define Card_width 80
#define Card_height 120

class Card {
public:
    Card(GA::Ref<Scene>& scene,const std::string& texture, int value)
    : m_SceneRef(scene){
        m_Tex = GA::CreateRef<Texture2D>(texture);
        ID = scene->CreateEntity(trimPath(texture));
        ID.AddComponent<SpriteRendererComponent>().Texture = m_Tex;        
        m_Value = value;

    }
    ~Card(){
        
    }

    std::string trimPath(const std::string& fullPath) {
        std::filesystem::path path(fullPath);
        std::string filename = path.stem().string(); // stem() = filename without extension
        return filename;
    }

    Entity GetID() { return ID;}
    int GetValue() { return m_Value;}
    bool Selected = false;
    bool OnHand = false;
    bool OnTable = false;
    glm::vec3 LastPos = {0,0,3};
private:
    Entity ID;
    GA::Ref<Scene> m_SceneRef;
    GA::Ref<Texture2D> m_Tex;
    int m_Value;
};

class Deck{
public:
    Deck(GA::Ref<Scene>& scene): m_SceneRef(scene){
        glm::vec2 Pos = {-80, -80};

        std::string resourcePath = "Resources/Cards/Standard/normal_cards/individual/";
        std::string name, filePath;
        std::string clubs = "clubs", diamonds = "diamonds",
                    hearts = "hearts", spades = "spades";

        for(int i = 1; i <= 13; i++){

            filePath = resourcePath + clubs + "/" + clubs + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->GetID().GetComponent<TransformComponent>().Translation = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + diamonds + "/" + diamonds + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->GetID().GetComponent<TransformComponent>().Translation = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + hearts + "/" + hearts + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->GetID().GetComponent<TransformComponent>().Translation = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + spades + "/" + spades + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->GetID().GetComponent<TransformComponent>().Translation = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};            
        }
    }

    ~Deck(){
    }

    std::vector<GA::Ref<Card>> GetCards() { return Cards;}
private:
    std::vector<GA::Ref<Card>> Cards;
    GA::Ref<Scene> m_SceneRef;
};


class Spot {
public:
    Spot(GA::Ref<Scene> scene, glm::vec2 pos, glm::vec2 size)
        : m_Scene(scene), m_Pos(pos)
    {
        ID = scene->CreateEntity("TableSpot");
        ID.AddComponent<SpriteRendererComponent>().Color = { 0, 1, 0, 1 };

        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = { pos, 2.0f };
        tc.Scale = { size, 1.0f };
        m_Size = size;
    }

    void PushCard(GA::Ref<Card> card) {
        float yOffset = 15.0f; // vertical spacing between stacked cards

        glm::vec3 basePos = { m_Pos.x, m_Pos.y, 3.0f };
        basePos.y += m_Cards.size() * yOffset; // Stack downwards

        card->GetID().GetComponent<TransformComponent>().Translation = basePos;
        card->LastPos = basePos;
        card->OnTable = true;

        m_Cards.push_back(card);
    }

    bool IsInside(const glm::vec3& point) {
        auto& tc = ID.GetComponent<TransformComponent>();
        glm::vec2 halfSize = tc.Scale * 0.5f;
        glm::vec2 min = tc.Translation - glm::vec3(halfSize, 0.0f);
        glm::vec2 max = tc.Translation + glm::vec3(halfSize, 0.0f);

        return point.x >= min.x && point.x <= max.x &&
                point.y >= min.y && point.y <= max.y;
    }

    bool IsAvailable() const {
        // Optional: limit number of cards in a stack
        return true;
    }

    const std::vector<GA::Ref<Card>>& GetCards() const { return m_Cards; }

    Entity GetID() const { return ID; }

private:
    Entity ID;
    GA::Ref<Scene> m_Scene;
    glm::vec2 m_Pos;
    glm::vec2 m_Size;

    std::vector<GA::Ref<Card>> m_Cards;
};


class Table {
public:
    Table(GA::Ref<Scene>& scene, glm::vec2& pos, glm::vec2& size, int spotCount = 6): m_Pos(pos), m_Scene(scene){
        ID = scene->CreateEntity("Table");
        ID.AddComponent<SpriteRendererComponent>().Color = {0, 0,1,1};
        ID.GetComponent<TransformComponent>().Translation = {pos, 1};
        ID.GetComponent<TransformComponent>().Scale = {size, 1};        

        CreateSpots(pos, spotCount);
    }

    ~Table(){

    }

    const std::vector<GA::Ref<Spot>>& GetAllSpots() const {
		return mSpot;
	}

	int GetSpotCount() const {
		return (int)mSpot.size();
	}

    GA::Ref<Spot> GetSpot(int place){
        return mSpot[place];
    }
private:
    void CreateSpots(const glm::vec2& center, int count) {
        mSpot.reserve(count);
        glm::vec2 spotSize = { Card_width, Card_height };

        float spacing = 5.0f;
        float totalWidth = count * Card_width + (count - 1) * spacing;
        float startX = center.x - totalWidth / 2 + Card_width / 2;

        for (int i = 0; i < count; ++i) {
            float x = startX + i * (Card_width + spacing);
            glm::vec2 pos = { x, center.y };
            auto spot = GA::CreateRef<Spot>(m_Scene, pos, spotSize);
            mSpot.push_back(spot);
        }
    }   
private:
    std::vector<GA::Ref<Spot>> mSpot;
    GA::Ref<Scene> m_Scene;
    Entity ID;
    glm::vec2 m_Pos;    
};

class Hand {
public:
    Hand(GA::Ref<Scene> scene, glm::vec2 position, glm::vec2 size, int maxCards = 4)
    : m_Pos(position), m_Scene(scene), m_MaxCards(maxCards)
    {
        // Create background
        ID = scene->CreateEntity("Hand");
        ID.AddComponent<SpriteRendererComponent>().Color = { 0, 0, 1, 1 };
        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = { position, 1 };
        tc.Scale = { size, 1 };

        m_Cards.resize(maxCards, nullptr);
    }

    ~Hand() = default;

    void PushCard(GA::Ref<Card> card, int slot) {
        if (slot < 0 || slot >= m_MaxCards) return;

        m_Cards[slot] = card;
        card->OnHand = true;

        glm::vec3 targetPos = GetCardPosition(slot);
        card->GetID().GetComponent<TransformComponent>().Translation = targetPos;
        card->LastPos = targetPos;
    }

    void RemoveCard(int slot) {
        if (slot < 0 || slot >= m_MaxCards) return;
        m_Cards[slot] = nullptr;
    }

    int GetMaxCards() const { return m_MaxCards; }

    glm::vec3 GetCardPosition(int index) const {
        const float spacing = 10.0f;
        float totalWidth = m_MaxCards * Card_width + (m_MaxCards - 1) * spacing;
        float startX = m_Pos.x - totalWidth / 2 + Card_width / 2;
        float x = startX + index * (Card_width + spacing);
        return { x, m_Pos.y, 2.0f };
    }

    const std::vector<GA::Ref<Card>>& GetCards() const {
        return m_Cards; 
    }

private:
    Entity ID;
    glm::vec2 m_Pos;
    GA::Ref<Scene> m_Scene;
    std::vector<GA::Ref<Card>> m_Cards;
    int m_MaxCards;
    
};