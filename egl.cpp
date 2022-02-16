#include "egl.hpp"
#include <iostream>
#include <GL/gl.h>

#define SK_GL 1

#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>
#include <include/gpu/GrBackendSurface.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>

#include <include/gpu/gl/GrGLFunctions.h>

const EGLint config_attributes[] = {
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_ALPHA_SIZE,
    8,
    EGL_STENCIL_SIZE,
    8,
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_ES2_BIT,
    EGL_NONE,
};

const EGLint context_attributes[] = {
    EGL_CONTEXT_CLIENT_VERSION,
    2,
    EGL_NONE,
};

void EGLProvider::create_window(struct wl_display *display, struct wl_surface *surface)
{
    this->egl_window = wl_egl_window_create(surface, 500, 500);

    const char *client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if (client_extensions == nullptr)
    {
        if (eglGetError() == EGL_BAD_DISPLAY)
        {
            std::cerr << "EGL_EXT_client_extensions not supported\n";
        }
        else
        {
            std::cerr << "Failed to query EGL client extensions\n";
        }
    }

    // TODO: Query EGL client extensions for EGL_PLATFORM_WAYLAND_EXT instead of just using it

    this->egl_display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_EXT, display, NULL);

    EGLint major = 0;
    EGLint minor = 0;
    if (eglInitialize(this->egl_display, &major, &minor) == EGL_FALSE)
    {
        std::cerr << "Failed to initialize EGL\n";
    }
    else
    {
        std::cout << major << " " << minor << ": Initialized EGL\n";
        if (!((major == 1 && minor >= 4) || major >= 2))
        {
            std::cerr << "Too old\n";
            return;
        }
    }

    EGLint matched = 0;
    if (!eglChooseConfig(this->egl_display, config_attributes, &this->egl_config, 1, &matched))
    {
        std::cerr << "eglChooseConfig failed\n";
        return;
    }

    this->egl_context = eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, context_attributes);

    if (this->egl_context == EGL_NO_CONTEXT)
    {
        std::cerr << "Failed to create EGL context\n";
        return;
    }

    this->egl_surface = eglCreateWindowSurface(this->egl_display, this->egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
        return;
    }

    if (eglMakeCurrent(this->egl_display, this->egl_surface, this->egl_surface, egl_context) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Brokey\n";
    }

    GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
    {
        std::cout << name << "\n";
        return eglGetProcAddress(name);
    };

    sk_sp<const GrGLInterface> interface = GrGLMakeAssembledGLESInterface(this, get_proc);
    GrDirectContext *context = GrDirectContext::MakeGL(interface).release();

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0; // assume default framebuffer
    // We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
    //(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType;
    colorType = kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget(500, 500,
                                              0, // sample count
                                              0, // stencil bits
                                              framebufferInfo);

    SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

    auto gpuSurface = SkSurface::MakeFromBackendRenderTarget(
        context,                     // context
        backendRenderTarget,         // backend render target
        kBottomLeft_GrSurfaceOrigin, // surface origin
        kN32_SkColorType,            // color type
        SkColorSpace::MakeSRGB(),    // color space
        &surface_properties,         // surface properties
        nullptr,                     // release proc
        nullptr                      // release context
    );
    if (!gpuSurface)
    {
        SkDebugf("SkSurface::MakeRenderTarget returned null\n");
        // return;
    }
    SkCanvas *gpuCanvas = gpuSurface->getCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);

    std::cout << "MakeFromBackendRenderTarget\n";

    gpuCanvas->clear(SK_ColorTRANSPARENT);

    gpuCanvas->drawCircle(SkPoint::Make(100, 100), 20, paint);
    context->flushAndSubmit();

    std::cout << "Clear\n";

    if (eglSwapBuffers(this->egl_display, this->egl_surface) == EGL_FALSE)
    {
    }

    while (true)
    {
        wl_display_dispatch(display);
    }
}