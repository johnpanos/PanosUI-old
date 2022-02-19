#define SK_GL 1
#include "UIApplication.hpp"
#include <iostream>

using namespace UI;

Application::Application()
{
    this->display = wl_display_connect(nullptr);
}

GrDirectContext *Application::getSkiaContext()
{
    if (this->interface == nullptr && this->context == nullptr)
    {
        std::cout << "making skia context\n";
        GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
        {
            return eglGetProcAddress(name);
        };
        this->interface = GrGLMakeAssembledGLESInterface(this, get_proc);
        this->context = GrDirectContext::MakeGL(interface).release();
    }

    return context;
}

Application *Application::getInstance()
{
    static Application *sharedInstance = new Application;
    return sharedInstance;
}

Application::~Application()
{
    if (this->display != nullptr)
    {
        wl_display_disconnect(display);
    }
}