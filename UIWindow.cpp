#include <GL/gl.h>
#include "UIWindow.hpp"
#include "UIApplication.hpp"
using namespace UI;

Window::Window(const char *title, SkRect frame)
{
    this->needsRepaint = true;
    this->surface = nullptr;
    this->frame = frame;

    Application *app = Application::getInstance();
    xdg = new WaylandXDG(app->display);
    wl_display_dispatch(app->display);

    toplevel = new WaylandXDGSurfaceToplevel(xdg->registry);
    toplevel->width = this->frame.width();
    toplevel->height = this->frame.height();
    toplevel->set_title(title);
    toplevel->commit();

    this->egl_window = wl_egl_window_create(toplevel->wl_surface, toplevel->width, toplevel->height);
    app->egl_provider.setup(app->display);
    this->egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }

    if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    this->on_resize(this->toplevel->width, this->toplevel->height);
}

void Window::on_resize(int width, int height)
{
    this->frame = SkRect::MakeXYWH(0, 0, width, height);

    Application *app = Application::getInstance();
    wl_egl_window_resize(this->egl_window, toplevel->width, toplevel->height, 0, 0);

    if (this->surface != nullptr)
    {
        std::cout << "delete surface\n";
        delete this->surface;
        this->surface = nullptr;
    }

    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0; // assume default framebuffer
    // We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
    // framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
    framebufferInfo.fFormat = GL_RGBA8;

    SkColorType colorType;
    colorType = kRGBA_8888_SkColorType;
    GrBackendRenderTarget backendRenderTarget(toplevel->width, toplevel->height,
                                              0, // sample count
                                              0, // stencil bits
                                              framebufferInfo);

    SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

    this->surface = SkSurface::MakeFromBackendRenderTarget(
                        app->getSkiaContext(),       // context
                        backendRenderTarget,         // backend render target
                        kBottomLeft_GrSurfaceOrigin, // surface origin
                        kN32_SkColorType,            // color type
                        SkColorSpace::MakeSRGB(),    // color space
                        &surface_properties,         // surface properties
                        nullptr,                     // release proc
                        nullptr                      // release context
                        )
                        .release();
}

void Window::draw()
{
    if (this->toplevel->resized)
    {
        this->on_resize(this->toplevel->width, this->toplevel->height);
        this->toplevel->resized = false;
        this->needsRepaint = true;
    }

    if (this->needsRepaint)
    {
        Application *app = Application::getInstance();

        app->getSkiaContext()->flushAndSubmit();

        if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
        {
            std::cerr << eglGetError() << " Failed to swap buffers\n";
        }

        this->needsRepaint = false;
    }
}
