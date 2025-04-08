#pragma once

#include "Config.h"
#include "Timestep.h"
#include "UUID.h"

#include <entt.hpp>

namespace CE {

    class Entity;

    class CE_API Scene{
    public:
        Scene();
        ~Scene();

        static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);        

        void OnUpdateRuntime(Timestep ts);        
        void OnViewportResize(uint32_t width, uint32_t height);

        void DuplicateEntity(Entity entity);

    private:
        template<typename T>
        void CE_API OnComponentAdded(Entity entity, T& component);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

        friend class Entity;
        // friend class SceneSerializer;    
    };
}