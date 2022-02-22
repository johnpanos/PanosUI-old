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

void EGLProvider::setup(struct wl_display *display)
{
    // // std::cout << "width: " << initial_width << " height: " << initial_height << "\n";
    // this->egl_window = wl_egl_window_create(surface, initial_width, initial_width);

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
        // std::cout << major << " " << minor << ": Initialized EGL\n";
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
}

void EGLProvider::on_resize(int32_t width, int32_t height)
{
    this->width = width;
    this->height = height;
    wl_egl_window_resize(this->egl_window, width, height, 0, 0);

    if (this->surface != nullptr)
    {
        delete this->surface.release();
        this->surface = nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0; // assume default framebuffer
    // We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
    // framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType;
    colorType = kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget(this->width, this->height,
                                              0, // sample count
                                              0, // stencil bits
                                              framebufferInfo);

    SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

    this->surface = SkSurface::MakeFromBackendRenderTarget(
        context,                     // context
        backendRenderTarget,         // backend render target
        kBottomLeft_GrSurfaceOrigin, // surface origin
        kN32_SkColorType,            // color type
        SkColorSpace::MakeSRGB(),    // color space
        &surface_properties,         // surface properties
        nullptr,                     // release proc
        nullptr                      // release context
    );
}

void EGLProvider::create_window(int32_t width, int32_t height, struct wl_display *display, struct wl_surface *surface, int x, int y)
{
}

void EGLProvider::draw(int x, int y)
{
    SkCanvas *gpuCanvas = this->surface->getCanvas();
    SkPaint paint;
    paint.setAntiAlias(true);

    gpuCanvas->clear(SK_ColorWHITE);

    paint.setAlpha(255);
    paint.setColor(SK_ColorBLACK);

    gpuCanvas->drawCircle(SkPoint::Make(width / 2 - 10, height / 2 - 10), 50, paint);

    paint.setColor(SK_ColorBLUE);
    paint.setAlpha(255 / 2);
    gpuCanvas->drawRoundRect(SkRect::MakeXYWH(x, y, 100, 100), 16, 16, paint);

    context->flushAndSubmit();

    if (eglSwapBuffers(this->egl_display, this->egl_surface) == EGL_FALSE)
    {
    }
}