#include "Renderer/GraphicsContext.h"
#include "CE_Assert.h"

namespace CE {

    GraphicsContext::GraphicsContext(){}                

    void GraphicsContext::Init(GLADloadproc proc){

        int status = gladLoadGLLoader(proc);
		CE_CORE_ASSERT(status, "Failed to initialize Glad!");

		CE_CORE_INFO("OpenGL Info:");
		CE_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		CE_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		CE_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		CE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Engine requires at least OpenGL version 4.5!");
    }
}