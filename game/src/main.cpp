#include "CardEngine.h"
#include "Application.h"

int main(int argc, char** argv){
    
    CE::Log::Init();

    CE_CORE_INFO("Loading Application");

    auto app = new Application("Card Game", {argc, argv});

    app->Run();

    CE_CORE_INFO("Shuting down");
    delete app;

    return 0;
}