#pragma once

#include <CardEngine.h>
#include <vector>
#include <string>
#include <map>

using namespace CE;

enum class Anchor {
    TopLeft, TopRight, BottomLeft, BottomRight,
    Center, Left, Right, Top, Bottom, Custom
};

struct AnchoredComponent {
    Entity entity;
    Anchor anchor;
    glm::vec2 offset; // Relative to anchor
};

class Border {
public:
    Border(GA::Ref<Scene>& scene,const std::string& name, const glm::vec2& position, const glm::vec2& size)
    :m_Position(position), m_Size(size), m_Scene(scene){
        ID = scene->CreateEntity(name);
        ID.AddComponent<SpriteRendererComponent>().Color =  { 0,0,1,1};
        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = {position, 1};
        tc.Scale = {size, 1};
    }

    void PushComponent(const std::string& name, Anchor anchor, const glm::vec2& offset = {0, 0},
        GA::Ref<Texture2D> texture = nullptr, const glm::vec2& size = {0, 0}, const glm::vec4& color = glm::vec4(0)) {

        glm::vec2 compSize = (size == glm::vec2(0)) ? m_Size : size;
        glm::vec2 compPos = GetAnchoredPosition(m_Position, m_Size, compSize, anchor, offset);

        Entity comp = m_Scene->CreateEntity(name);
        auto& CTC = comp.GetComponent<TransformComponent>();
        CTC.Translation = {compPos, 5};
        CTC.Scale = {compSize, 1};
        auto& CUI = comp.AddComponent<UIElement>();
        if (texture) CUI.Texture = texture;
        if (color != glm::vec4(0)) CUI.Color = color;        

        m_Components[name] = {comp, anchor, offset};        
    }

    void PushButtonComponent(const std::string& name, Anchor anchor, const glm::vec2& offset = {0,0},
                            GA::Ref<Texture2D> texture = nullptr, const glm::vec2& size = {0,0},
                            const glm::vec4& color = glm::vec4(0)){

        glm::vec2 compSize = (size == glm::vec2(0)) ? m_Size : size;
        glm::vec2 compPos = GetAnchoredPosition(m_Position, m_Size, compSize, anchor, offset);

        Entity comp = m_Scene->CreateEntity(name);
        auto& CTC = comp.GetComponent<TransformComponent>();
        CTC.Translation = {compPos, 5};
        CTC.Scale = {compSize, 1};

        auto& CUI = comp.AddComponent<ButtonComponent>();
        if(texture) CUI.Texture = texture;
        CUI.Color = color;
        CUI.OriginalScale = CTC.Scale;
        CUI.TargetScale = CTC.Scale;
        CUI.BaseColor = CUI.Color;
        CUI.CurrentColor = CUI.Color;

        m_Components[name] = {comp, anchor, offset};
    }

    void PushTextComponent(const std::string& name, Anchor anchor, const std::string& text,GA::Ref<Font> font,const glm::vec4& color = glm::vec4(1),
                const glm::vec2& offset = {0,0}, const glm::vec2& size = {0,0}){
        
        glm::vec2 compSize = (size == glm::vec2(0)) ? m_Size : size;
        glm::vec2 compPos = GetAnchoredPosition(m_Position, m_Size, compSize, anchor, offset);

        Entity comp = m_Scene->CreateEntity(name);
        auto& CTC = comp.GetComponent<TransformComponent>();
        CTC.Translation = {compPos, 5};
        CTC.Scale = {compSize, 1};

        auto& CUI = comp.AddComponent<TextUIComponent>();
        CUI.Text = text;
        CUI.Font = font;
        CUI.Color = color;
    }

    void SetPosition(glm::vec2 newPosition) {
        m_Position = newPosition;
        auto& tc = ID.GetComponent<TransformComponent>();
        tc.Translation = {newPosition, tc.Translation.z};
        UpdateAllChildren();
    }

    void UpdateAllChildren() {
        for (auto& [name, comp] : m_Components)
            UpdateComponentTransform(comp);
    }

    AnchoredComponent& GetComponent(const std::string& name){
        return m_Components[name];
    }

    ~Border() = default;
private:
    glm::vec2 GetAnchoredPosition(const glm::vec2& borderPos, const glm::vec2& borderSize,
                                const glm::vec2& compSize, Anchor anchor, const glm::vec2& offset) {
        glm::vec2 pos = borderPos;

        if (anchor == Anchor::TopLeft)
            pos += glm::vec2(-borderSize.x / 2 + compSize.x / 2,  borderSize.y / 2 - compSize.y / 2);
        else if (anchor == Anchor::TopRight)
            pos += glm::vec2( borderSize.x / 2 - compSize.x / 2,  borderSize.y / 2 - compSize.y / 2);
        else if (anchor == Anchor::BottomLeft)
            pos += glm::vec2(-borderSize.x / 2 + compSize.x / 2, -borderSize.y / 2 + compSize.y / 2);
        else if (anchor == Anchor::BottomRight)
            pos += glm::vec2( borderSize.x / 2 - compSize.x / 2, -borderSize.y / 2 + compSize.y / 2);
        else if (anchor == Anchor::Left)
            pos += glm::vec2(-borderSize.x / 2 + compSize.x / 2, 0);
        else if (anchor == Anchor::Right)
            pos += glm::vec2( borderSize.x / 2 - compSize.x / 2, 0);
        else if (anchor == Anchor::Top)
            pos += glm::vec2(0, borderSize.y / 2 - compSize.y / 2);
        else if (anchor == Anchor::Bottom)
            pos += glm::vec2(0, -borderSize.y / 2 + compSize.y / 2);
        // else center (no offset)

        //todo change top and bottom

        return pos + offset;
    }
    void UpdateComponentTransform( AnchoredComponent& ac) {
        auto& tc = ac.entity.GetComponent<TransformComponent>();
        glm::vec2 anchorBase;

        // Compute base anchor point on the border
        if (ac.anchor == Anchor::TopLeft)          anchorBase = m_Position + glm::vec2(-m_Size.x/2, m_Size.y/2);
        else if (ac.anchor == Anchor::TopRight)    anchorBase = m_Position + glm::vec2( m_Size.x/2, m_Size.y/2);
        else if (ac.anchor == Anchor::BottomLeft)  anchorBase = m_Position + glm::vec2(-m_Size.x/2, -m_Size.y/2);
        else if (ac.anchor == Anchor::BottomRight) anchorBase = m_Position + glm::vec2( m_Size.x/2, -m_Size.y/2);
        else if (ac.anchor == Anchor::Center)      anchorBase = m_Position;
        else if (ac.anchor == Anchor::Left)        anchorBase = m_Position + glm::vec2(-m_Size.x/2, 0);
        else if (ac.anchor == Anchor::Right)       anchorBase = m_Position + glm::vec2( m_Size.x/2, 0);
        else if (ac.anchor == Anchor::Top)         anchorBase = m_Position + glm::vec2(0, m_Size.y/2);
        else if (ac.anchor == Anchor::Bottom)      anchorBase = m_Position + glm::vec2(0, -m_Size.y/2);
        else if (ac.anchor == Anchor::Custom)      anchorBase = m_Position;

        tc.Translation = { anchorBase + ac.offset, 5 };
    }
private:
    Entity ID;
    // std::vector<AnchoredComponent> m_Components;
    std::map<std::string, AnchoredComponent> m_Components;
    glm::vec2 m_Position, m_Size;
    GA::Ref<Scene> m_Scene;
};