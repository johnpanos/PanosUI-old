#ifndef _UI_SKIA_H
#define _UI_SKIA_H

#define SK_GL 1
#include <EGL/egl.h>
#include <GL/gl.h>
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>
#include <include/core/SkSurface.h>
#include <assert.h>
#include <iostream>

namespace UI
{
    class Skia
    {
        sk_sp<const GrGLInterface> interface = nullptr;
        GrDirectContext *context = nullptr;

    public:
        SkSurface *surface = nullptr;

        GrDirectContext *get_context();
        void setup(int width, int height);

        ~Skia();
    };
};

#endif