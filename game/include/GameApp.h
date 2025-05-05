#pragma once
#include <UrbanEngine.h>
// #include "Game/MainMenuLayer.h"
// #include "Layers/TestLayer.h"
#include "Layers/3dTestLayer.h"
// #include "GameLayer.h"

#define now_width 1280
#define now_height 720

using namespace UE;
class GameApp : public Application {
public:
    GameApp(ApplicationCommandLineArgs args)
        :Application("CardGame",{now_width, now_height}, args){
            // PushLayer(new MainMenuLayer());
            // PushLayer(new GameLayer());
            // PushLayer(new TestLayer({1280, 720}));
            PushLayer(new ModelTestLayer({1280, 720}));
        }

    ~GameApp(){}
};