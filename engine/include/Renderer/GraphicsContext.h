#pragma once

#include "Config.h"
#include <glad/glad.h>

namespace CE {

    class CE_API GraphicsContext{
    public:
        GraphicsContext();

        void Init(GLADloadproc proc);        

    private:
    };
}