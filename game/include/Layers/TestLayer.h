#pragma once

#include "Core/Layer.h"
#include "Core/Input.h"
#include "CardEngine.h"

class TestLayer : public Layer {
public:
    TestLayer(){

    }
    virtual ~TestLayer() = default;

    virtual void OnAttach() override{
        CE_INFO("ONATTACH()");
    }
    virtual void OnDetach() override{
        CE_INFO("ONDETACH()");
    }

    void OnUpdate(Timestep ts) override{
        if(Input::IsKeyPressed(Key::A))
            CE_INFO("A pressed");
    }
    void OnEvent(Event& e) override{
        CE_INFO("{0}", e.GetName());
    }
};