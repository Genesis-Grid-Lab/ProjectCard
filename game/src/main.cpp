#include <UrbanEngine.h>
#include <Core/EntryPoint.h>
#include "GameApp.h"

UE::Application* UE::CreateApplication(UE::ApplicationCommandLineArgs args){

    return new GameApp(args);
}