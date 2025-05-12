#pragma once

#include <UrbanEngine.h>
#include "Renderer/RenderCommand.h"
#include <Core/Input.h>
#include "ImGui/ImGuiInputs.h"

using namespace UE;

class ModelTestLayer : public Layer {
public:
    ModelTestLayer(const glm::vec2& size) : Layer("Model Test"), m_Size(size) {}

    void OnAttach() override {
        // float aspect = m_Size.x / m_Size.y;         
        m_Scene = CreateRef<Scene>(m_Size.x, m_Size.y);

        mManModel = CreateRef<Model>("Resources/FinalBaseMesh.fbx");
        // mManModel = CreateRef<Model>("Resources/dancing_vampire.dae");
        danceAnimation = CreateRef<Animation>("Resources/FinalBaseMesh.fbx", mManModel);
        // danceAnimation = CreateRef<Animation>("Resources/dancing_vampire.dae", mManModel);
        mCubeModel = CreateRef<Model>("Resources/cube.fbx");
        mSphereModel = CreateRef<Model>("Resources/sphere.fbx");
        mModel = CreateRef<Model>("Resources/backpack/backpack.obj");  
        
        auto& ManEntity = m_Scene->CreateEntity("Vampire");
        auto& ManTC = ManEntity.GetComponent<TransformComponent>();
        auto& physTes = ManEntity.AddComponent<RigidbodyComponent>();
        physx::PxMaterial* Material = nullptr;
        glm::vec3 HalfExtents = glm::vec3(0.5f);

        physx::PxTransform pxTransform(
            physx::PxVec3(ManTC.Translation.x, ManTC.Translation.y, ManTC.Translation.z)
            // physx::PxQuat(transform.Rotation.x, transform.Rotation.y, transform.Rotation.z, 0.0f)
        );

        physTes.Body = m_Scene->m_Physics3D.GetPhysics()->createRigidDynamic(pxTransform);

        if (!Material)
            Material = m_Scene->m_Physics3D.GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);

        physx::PxShape* shape = m_Scene->m_Physics3D.GetPhysics()->createShape(
            physx::PxBoxGeometry(HalfExtents.x, HalfExtents.y, HalfExtents.z),
            *Material
        );
    
        physTes.Body->attachShape(*shape);
        m_Scene->m_Physics3D.GetScene()->addActor(*physTes.Body);
        auto& ManModel = ManEntity.AddComponent<ModelComponent>();
        ManModel.ModelData = mManModel;
        ManModel.AnimationData = danceAnimation;
        // ManModel.AnimationData->SetModel(ManModel.ModelData);
        ManTC.Translation = {-5,0,0};
        ManTC.Scale = glm::vec3(0.25f);

        auto& CubeEntity = m_Scene->CreateEntity("Cube");
        CubeEntity.AddComponent<ModelComponent>().ModelData = mCubeModel;
        auto& CubeTC = CubeEntity.GetComponent<TransformComponent>();
        CubeTC.Translation = {5,0,0};

        auto& SphereEntity = m_Scene->CreateEntity("Sphere");
        SphereEntity.AddComponent<ModelComponent>().ModelData = mSphereModel;
        auto& SphereTC = SphereEntity.GetComponent<TransformComponent>();
        SphereTC.Translation = {0, 0, -5};

        auto& BackEntity = m_Scene->CreateEntity("Backpack");
        BackEntity.AddComponent<ModelComponent>().ModelData = mModel;
        auto& BackTC = BackEntity.GetComponent<TransformComponent>();
        BackTC.Translation = {0, 1, 0};

        cubePos = {0.0f, -2.0f, 0.0f};
        cubeSize = {50, 1, 50};
        auto& floorEntity = m_Scene->CreateEntity("Floor");
        floorEntity.AddComponent<CubeComponent>().Color = {0.5f, 0.0f, 0.5f};
        auto& FloorTC = floorEntity.GetComponent<TransformComponent>();
        FloorTC.Translation = cubePos;
        FloorTC.Scale = cubeSize;  
        
        auto& uiTest = m_Scene->CreateEntity("UI");
        auto& CTC = uiTest.GetComponent<TransformComponent>();
        CTC.Translation = {100, 100, 5};
        CTC.Scale = {10, 10, 1};
        auto& CUI = uiTest.AddComponent<ButtonComponent>();
        CUI.Color = {1, 0, 0, 1};
        CUI.OriginalScale = CTC.Scale;
        CUI.TargetScale = CTC.Scale;
        CUI.BaseColor = {1, 1, 0, 1};
        CUI.CurrentColor = {1, 0, 0, 1};
    }

    void OnUpdate(Timestep ts) override {

        UE::Renderer2D::ResetStats();
        m_Scene->OnUpdateRuntime(ts);           
        m_Scene->OnMouseInput(Input::GetMouseX(), Input::GetMouseY(), Input::IsMouseButtonPressed(0), ts);        
        
    }

    void OnImGuiRender() override{
        ImGui::SetCurrentContext(UE::Application::Get().GetImGuiLayer()->GetContext());
        IM_ASSERT(ImGui::GetCurrentContext() && "No valid ImGui context!");
        IM_ASSERT(ImGui::GetFrameCount() > 0 && "ImGui::NewFrame() was not called!");

        ImGui::Begin("Stats");
        
        auto stats = UE::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        InputVec3("Cube Pos", &cubePos);
        InputVec3("Cube Size", &cubeSize);
        
		ImGui::End();

        ImGui::Begin("physics Debug");

        m_Scene->ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto& comp){
            auto& transform = entity.template GetComponent<TransformComponent>();
            InputVec3("Rigid Pos", &transform.Translation);
        });

        ImGui::End();
        
    }
    
    void OnEvent(Event& e) override{
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(ModelTestLayer::OnResize));
    }

    bool OnResize(WindowResizeEvent& e){
        m_Scene->OnViewportResize(e.GetWidth(), e.GetHeight());

        return false;
    }
private:
    Ref<Shader> mShader;
    Ref<Model> mModel;
    Ref<Model> mSphereModel;
    Ref<Model> mCubeModel;
    Ref<Model> mManModel;
    Ref<Animation> danceAnimation;    
    Camera3D mCam;
    Camera2D mCam2;     
    glm::vec2 m_Size;
    glm::vec3 m_LightPos;
    glm::vec3 cubeSize, cubePos;
    glm::vec3 Pos = {0.0f, 1.0f, 5.0f};
    Ref<Scene> m_Scene;
};
