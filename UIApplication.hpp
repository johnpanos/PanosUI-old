#ifndef _UIAPPLICATION_H
#define _UIAPPLICATION_H

#define SK_GL 1
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>
#include "egl.hpp"

namespace UI
{
    class Application
    {
        Application();
        ~Application();

    public:
        sk_sp<const GrGLInterface> interface;
        GrDirectContext *context;
        EGLProvider egl_provider;
        struct wl_display *display;

        static Application *getInstance();

        GrDirectContext *getSkiaContext();
    };
};

#endif