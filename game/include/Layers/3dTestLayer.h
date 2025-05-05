#pragma once

#include <UrbanEngine.h>
#include "Renderer/RenderCommand.h"
#include <Core/Input.h>

using namespace UE;

class ModelTestLayer : public Layer {
public:
    ModelTestLayer(const glm::vec2& size) : Layer("Model Test"), m_Size(size) {}

    void OnAttach() override {
        // float aspect = m_Size.x / m_Size.y;
        mCam = Camera3D(m_Size, Pos);          
        // mCam.SetPosition(Pos);
        mManModel = CreateRef<Model>("Resources/FinalBaseMesh.obj");
        mCubeModel = CreateRef<Model>("Resources/sphere.fbx");
        mModel = CreateRef<Model>("Resources/backpack/backpack.obj");
        // mShader = CreateRef<Shader>("Data/Shaders/model.glsl");
        m_LightPos = { 3.0f, 5.0f, 2.0f };
    }

    void OnUpdate(Timestep ts) override {

        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    	RenderCommand::Clear();

        Renderer3D::BeginCamera(mCam);

        Renderer3D::DrawModel(mModel, {0,0,0});
        Renderer3D::DrawModel(mCubeModel, {5,0,0});

        mCam.ProcessInputAndMouse(ts);

        Renderer3D::EndCamera();

        
    }
    
private:
    Ref<Shader> mShader;
    Ref<Model> mModel;
    Ref<Model> mCubeModel;
    Ref<Model> mManModel;
    Camera3D mCam;     
    glm::vec2 m_Size;
    glm::vec3 m_LightPos;
    glm::vec3 Pos = {0.0f, 1.0f, 5.0f};
};
