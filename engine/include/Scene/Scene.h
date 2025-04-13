#pragma once

#include "Config.h"
#include "Timestep.h"
#include "UUID.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "CE_Assert.h"
#include <entt.hpp>

namespace CE {

    class Entity;

    class CE_API Scene{
    public:
        Scene(uint32_t width, uint32_t height);
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);      
        template<typename Entt, typename Comp, typename Task>
        void ViewEntity(Task&& task){
            // CE_CORE_ASSERT(std::is_base_of<Entity, Entt>::value, "error viewing entt");
            m_Registry.view<Comp>().each([this, &task] 
            (auto entity, auto& comp) 
            { 
                // task(std::move(Entt(&m_Registry, entity)), comp);
                task(std::move(Entt(entity, this)), comp);
            });
        }

        void OnUpdateRuntime(Timestep ts);  
        void DrawScreen(Ref<Framebuffer>& buffer);      
        void OnViewportResize(uint32_t width, uint32_t height);        

        void DuplicateEntity(Entity entity);

        Camera& GetCam() { return m_Cam;}
        // Ref<Framebuffre>& GetBuffer() { return m_Framebuffer;}
        //temp
        Ref<Framebuffer> m_Framebuffer;
    private:
        template<typename T>
        void CE_API OnComponentAdded(Entity entity, T& component);
    private:
        entt::registry m_Registry;
        Ref<CE::Texture2D> m_Screen;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        Camera m_Cam;
        friend class Entity;
        // friend class SceneSerializer;    
    };
}