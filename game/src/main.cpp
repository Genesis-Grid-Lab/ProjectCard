#include "Application.h"

int main(int argc, char** argv){
    
    printf("init\n");

    auto app = new Application("Card Game", {argc, argv});

    app->Run();


    delete app;

    return 0;
}