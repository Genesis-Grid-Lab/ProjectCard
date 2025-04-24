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
    glm::vec3 TargetPos = {0, 0, 3};
    int m_Value;
private:
    Entity ID;
    GA::Ref<Scene> m_SceneRef;
    GA::Ref<Texture2D> m_Tex;
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
            Cards.back()->TargetPos = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + diamonds + "/" + diamonds + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->TargetPos = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + hearts + "/" + hearts + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->TargetPos = {Pos, 3};
            Cards.back()->GetID().GetComponent<TransformComponent>().Scale = { Card_width, Card_height, 1};
            filePath = resourcePath + spades + "/" + spades + "_" + std::to_string(i) + ".png";
            Cards.emplace_back(GA::CreateRef<Card>(scene, filePath, i));
            Cards.back()->TargetPos = {Pos, 3};
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

    void PushCard(GA::Ref<Card>& card, std::vector<GA::Ref<Card>>& GameCards,std::vector<GA::Ref<Card>>& playedCards) {
        float yOffset = 15.0f; // vertical spacing between stacked cards

        glm::vec3 basePos = { m_Pos.x, m_Pos.y, 3.0f };
        basePos.y += m_Cards.size() * yOffset; // Stack downwards

        // card->GetID().GetComponent<TransformComponent>().Translation = basePos;
        // card->LastPos = basePos;
        // card->OnTable = true;

        glm::vec3 target = { m_Pos.x, m_Pos.y + (m_Cards.size() * 15.0f), 3.0f };

        card->TargetPos = target; // set where it *wants* to be
        card->LastPos = target;
        card->OnTable = true;

        m_Cards.push_back(card);
        auto it = std::find(playedCards.begin(), playedCards.end(), card);
        if( it == playedCards.end() )
            playedCards.emplace_back(card);
        GameCards.erase(std::find(GameCards.begin(), GameCards.end(), card));

        for (size_t i = 0; i < m_Cards.size(); ++i) {
            auto& sr = m_Cards[i]->GetID().GetComponent<SpriteRendererComponent>();
        
            if (i == m_Cards.size() - 1)
                sr.Color = glm::vec4(1, 1, 1, 1); // top = normal
            else
                sr.Color = glm::vec4(1, 1, 1, 0.6f); // faded
        }
    }

    void RemoveCard(GA::Ref<Card> card){   
        card->TargetPos = {900, 1500, 3};
        m_Cards.erase(std::find(m_Cards.begin(), m_Cards.end(), card));        
        Reorder();
    }

    void Reorder() {
        for (size_t i = 0; i < m_Cards.size(); ++i) {
            auto& card = m_Cards[i];
            glm::vec3 newPos = { m_Pos.x, m_Pos.y - i * 15.0f, 3.0f };
            card->TargetPos = newPos;
            card->LastPos = newPos;
        }
    }

    void RemoveTopCard() {
        if (!m_Cards.empty())
            m_Cards.pop_back();
    
        Reorder();
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

    bool IsEmpty() const {

        return m_Cards.empty();
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
        Table(GA::Ref<Scene>& scene, glm::vec2& pos, glm::vec2& size)
            : m_Pos(pos), m_Scene(scene),m_InitialSize(size) {
            ID = scene->CreateEntity("Table");
            ID.AddComponent<SpriteRendererComponent>().Color = {0, 0, 1, 1};

            glm::vec2 anchorPos = pos - glm::vec2(0, size.y * 0.5f); // anchor top center
            ID.GetComponent<TransformComponent>().Translation = {anchorPos, 1};
            ID.GetComponent<TransformComponent>().Scale = {size, 1};        
        }
    
        ~Table() {}
    
        const std::vector<GA::Ref<Spot>>& GetAllSpots() const {
            return mSpot;
        }
    
        int GetSpotCount() const {
            return (int)mSpot.size();
        }
    
        GA::Ref<Spot> GetSpot(int place) {
            return mSpot[place];
        }
    
        void AddSpotAndPushCard(GA::Ref<Card>& card, 
                            std::vector<GA::Ref<Card>>& GameCards,
                            std::vector<GA::Ref<Card>>& playedCards) {
            glm::vec2 spotSize = { Card_width, Card_height };
            glm::ivec2 offset = GetSpotPatternOffset(mSpot.size());
            int row = offset.y;

            GrowHeightIfNeeded(row + 1); // Ensure enough height for this row
            glm::vec2 newSpotPos = m_Pos + glm::vec2((offset.x * (Card_width + 10.0f)) - 50, (offset.y * (Card_height + 10.0f)) - 55);
            auto spot = GA::CreateRef<Spot>(m_Scene, newSpotPos, spotSize);
            mSpot.push_back(spot);
            spot->PushCard(card, GameCards, playedCards);
            
        }

        Entity GetID() { return ID;}
    
    private:
        glm::vec2 GetSpotPatternOffset(int index) {
            // Pattern: center, right, down-center, down-right, left, down-left, etc.
            static const std::vector<glm::ivec2> pattern = {
                {0, 0}, {1, 0}, {0, 1}, {1, 1}, {-1, 0}, {-1, 1}, {2, 0}, {2, 1},
                {0, 2}, {1, 2}, {-1, 2}, {2, 2}, {-2, 0}, {-2, 1}, {-2, 2}, {0, 3}
            };
    
            
            return pattern[index % pattern.size()];
        }

        void GrowHeightIfNeeded(int newRowCount) {
            float newHeight = newRowCount * (Card_height + 10.0f);
        
            auto& tc = ID.GetComponent<TransformComponent>();
            float oldHeight = tc.Scale.y;
        
            if (newHeight > oldHeight) {
                tc.Scale.y = newHeight;
                float delta = (newHeight - oldHeight) * 0.5f;
                tc.Translation.y += delta; // move down so top stays fixed
            }
        }
    
    private:
        std::vector<GA::Ref<Spot>> mSpot;
        GA::Ref<Scene> m_Scene;
        Entity ID;
        glm::vec2 m_Pos;
        glm::vec2 m_InitialSize;
    };

class Hand {
public:
    Hand() = default;
    Hand(GA::Ref<Scene> scene, glm::vec2 position, glm::vec2 size, int maxCards = 4)
    : m_Pos(position), m_Scene(scene), m_MaxCards(maxCards)
    {
        // Create background
        ID = scene->CreateEntity("Hand");
        ID.AddComponent<SpriteRendererComponent>().Color = { 0, 0, 1, 1 };
        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = { position, 1 };
        tc.Scale = { size, 1 };

        // m_Cards.resize(maxCards, nullptr);
    }

    ~Hand() = default;

    void PushCard(GA::Ref<Card>& card, std::vector<GA::Ref<Card>>& GameCards,std::vector<GA::Ref<Card>>& playedCards, int slot) {        

        card->OnHand = true;
        card->OnTable = false;
        
        glm::vec3 targetPos = GetCardPosition(slot);
        card->GetID().GetComponent<TransformComponent>().Translation = targetPos;
        card->LastPos = targetPos;
        card->TargetPos = targetPos;
        
        m_Cards.emplace_back(card);
        playedCards.emplace_back(card);
        GameCards.erase(std::find(GameCards.begin(), GameCards.end(), card));
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

    std::vector<GA::Ref<Card>>& GetCards() {
        return m_Cards; 
    }

private:
    Entity ID;
    glm::vec2 m_Pos;
    GA::Ref<Scene> m_Scene;
    std::vector<GA::Ref<Card>> m_Cards;
    int m_MaxCards;
    friend class AIHand;
    
};

class AIHand : public Hand {
public:
    AIHand(){
        m_Pos = {-80, -80};
    }

    ~AIHand() = default;
private:
};