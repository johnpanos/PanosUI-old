#include <GL/gl.h>
#include "UIWindow.hpp"
#include "UIApplication.hpp"
#include "UIAnimation.hpp"
using namespace UI;

void Window::on_mouse_motion(int x, int y)
{
    WaylandPointerDelegate::on_mouse_motion(x, y);

    if (this->clicked_view != nullptr)
    {
        SkPoint delta = SkPoint::Make(x - global_clicked_at.x(), y - global_clicked_at.y());
        this->clicked_view->on_mouse_drag(delta);
        return;
    }

    UI::View *new_hovered_view = this->root_view->hit_test(SkPoint::Make(x, y));
    if (new_hovered_view != nullptr)
    {
        if (new_hovered_view != hovered_view)
        {
            if (hovered_view != nullptr)
            {
                hovered_view->on_mouse_exit();
            }
            new_hovered_view->on_mouse_enter();
            hovered_view = new_hovered_view;
        }
    }
}

void Window::on_mouse_click()
{
    // std::cout << "Mouse click listener\n";
    // std::cout << "X: " << this->x << "Y: " << this->y << "\n";
    clicked_view = this->root_view->hit_test(SkPoint::Make(this->x, this->y));
    global_clicked_at = SkPoint::Make(this->x, this->y);
    local_clicked_at = this->root_view->convert(SkPoint::Make(this->x, this->y), clicked_view);
    if (clicked_view != nullptr)
    {
        // std::cout << "View found " << (clicked_view == this->root_view) << "\n";
        std::cout << "clicked at " << global_clicked_at.x() << " " << global_clicked_at.y() << "\n";
        std::cout << "clicked at " << local_clicked_at.x() << " " << local_clicked_at.y() << "\n";
        clicked_view->on_mouse_click();
    }
    else
    {
        // std::cout << "No view found";
    }
}

void Window::on_mouse_up()
{
    if (clicked_view != nullptr)
    {
        SkPoint translated_point = this->root_view->convert(SkPoint::Make(this->x, this->y), clicked_view);
        this->clicked_view->on_mouse_up(translated_point.x(), translated_point.y());

        this->global_clicked_at.set(0, 0);
        this->local_clicked_at.set(0, 0);

        clicked_view = nullptr;
    }
}

void Window::on_mouse_scroll(bool up)
{
    if (hovered_view != nullptr)
    {
        hovered_view->on_mouse_scroll(up ? 32 : -32);
    }
    else
    {
        // std::cout << "No view found";
    }
}

Window::Window(const char *title, SkRect frame)
{
    this->needsRepaint = true;
    this->needs_layout = true;
    this->surface = nullptr;
    this->frame = frame;

    Application *app = Application::getInstance();
    xdg = new WaylandXDG(app->display);
    wl_display_dispatch(app->display);

    seat = new WaylandSeat(xdg->registry->wl_seat);

    this->pointer = new WaylandPointer(xdg->registry);
    this->pointer->delegate = this;
    seat->add_listener(pointer);

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

    // std::cout << "width: " << this->frame.width() << " | height: " << this->frame.width() << "\n";

    Application *app = Application::getInstance();
    wl_egl_window_resize(this->egl_window, toplevel->width, toplevel->height, 0, 0);

    if (this->surface != nullptr)
    {
        // std::cout << "delete surface\n";
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
