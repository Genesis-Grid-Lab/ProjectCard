#pragma once
#include <UrbanEngine.h>
#include "Game/MainMenuLayer.h"
// #include "Layers/TestLayer.h"
// #include "GameLayer.h"

using namespace UE;
class GameApp : public Application {
public:
    GameApp(ApplicationCommandLineArgs args)
        :Application("CardGame", args){
            PushLayer(new MainMenuLayer());
            // PushLayer(new GameLayer());
            // PushLayer(new TestLayer());
        }

    ~GameApp(){}
};