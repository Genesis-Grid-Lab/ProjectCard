#pragma once
#include <UrbanEngine.h>
// #include "Game/MainMenuLayer.h"
// #include "Layers/TestLayer.h"
#include "Layers/3dTestLayer.h"
// #include "GameLayer.h"

using namespace UE;
class GameApp : public Application {
public:
    GameApp(ApplicationCommandLineArgs args)
        :Application("CardGame",{1280, 720}, args){
            // PushLayer(new MainMenuLayer());
            // PushLayer(new GameLayer());
            // PushLayer(new TestLayer({1280, 720}));
            PushLayer(new ModelTestLayer({1280, 720}));
        }

    ~GameApp(){}
};