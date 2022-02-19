#include <iostream>
#include <vector>
#include <GL/gl.h>

#define SK_GL 1
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <include/gpu/gl/GrGLAssembleInterface.h>

#include "egl.hpp"
#include "WaylandRegistry.hpp"

class EventResponder
{
public:
    EventResponder *next;

    virtual void respond()
    {
        std::cout << "default\n";
    };
};

class UILayerDelegate
{
public:
    virtual void draw(SkSurface *surface)
    {
    }
};

class UILayer
{

public:
    SkSurface *backingSurface;
    UILayerDelegate *delegate;
    SkRect frame;

    UILayer()
    {
    }

    virtual void render()
    {
        delegate->draw(backingSurface);
    };
};

class UIView : public EventResponder, public UILayerDelegate
{
public:
    UILayer layer;
    std::vector<UIView *> children{};
    SkColor backgroundColor;
    SkRect frame;
    SkRect bounds;

    UIView()
    {
        this->layer = UILayer();
        this->layer.delegate = this;
    }

    virtual void render()
    {
        std::cout << "render\n";
    };

    void addSubView(UIView *view)
    {
        this->children.push_back(view);
        view->next = this;
    }

    virtual void draw(SkSurface *surface)
    {
        std::cout << "Drawing canvas\n";
        SkCanvas *canvas = surface->getCanvas();
        canvas->clear(SK_ColorWHITE);

        SkPaint paint;
        paint.setColor(SK_ColorBLUE);
        paint.setAlpha(255 / 2);
        canvas->drawCircle(SkPoint::Make(frame.width() / 2, frame.height() / 2), 50, paint);

        surface->flush();
    }
};

class MyView : public UIView
{
public:
    void respond()
    {
        this->next->respond();
    }
};

class UIApplication
{
private:
    UIApplication()
    {
        std::cout << "we are here\n";
        display = wl_display_connect(nullptr);
        std::cout << "we are also here\n";
    }

public:
    sk_sp<const GrGLInterface> interface;
    GrDirectContext *context;
    struct wl_display *display;
    EGLProvider egl_provider;
    static UIApplication &getSharedInstance();

    GrDirectContext *getSkiaContext()
    {
        if (this->interface == nullptr && this->context == nullptr)
        {
            std::cout << this->interface << " " << this->context << "\n";
            std::cout << "Allocate skia\n";
            GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
            {
                return eglGetProcAddress(name);
            };
            this->interface = GrGLMakeAssembledGLESInterface(this, get_proc);
            this->context = GrDirectContext::MakeGL(interface).release();
        }

        return context;
    }
};

UIApplication &UIApplication::getSharedInstance()
{
    static UIApplication sharedInstance;
    return sharedInstance;
}

class UIWindow
{
public:
    EGLSurface egl_surface;
    wl_egl_window *egl_window;
    SkSurface *surface;

    WaylandXDG *xdg;
    WaylandXDGSurfaceToplevel *toplevel;

    UIApplication *application;

    UIWindow(UIApplication *app)
    {
        this->application = app;
        std::cout << "in window\n";

        xdg = new WaylandXDG(application->display);
        wl_display_dispatch(application->display);

        toplevel = new WaylandXDGSurfaceToplevel(xdg->registry);
        toplevel->width = 500;
        toplevel->height = 500;

        toplevel->set_title("Hello World");
        toplevel->commit();
        xdg->registry->round_trip();

        this->egl_window = wl_egl_window_create(toplevel->wl_surface, toplevel->height, toplevel->width);

        application->egl_provider.setup(application->display);

        this->egl_surface = eglCreateWindowSurface(application->egl_provider.egl_display, application->egl_provider.egl_config, this->egl_window, nullptr);

        if (this->egl_surface == EGL_NO_SURFACE)
        {
            std::cerr << "Failed to create EGL window surface\n";
            return;
        }

        if (eglMakeCurrent(application->egl_provider.egl_display, this->egl_surface, this->egl_surface, application->egl_provider.egl_context) == EGL_FALSE)
        {
            std::cerr << eglGetError() << " Cannot make current\n";
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
                            application->getSkiaContext(), // context
                            backendRenderTarget,           // backend render target
                            kBottomLeft_GrSurfaceOrigin,   // surface origin
                            kN32_SkColorType,              // color type
                            SkColorSpace::MakeSRGB(),      // color space
                            &surface_properties,           // surface properties
                            nullptr,                       // release proc
                            nullptr                        // release context
                            )
                            .release();

        this->surface->getCanvas()->clear(SK_ColorGRAY);
        application->getSkiaContext()->flushAndSubmit();

        this->on_resize(0, 0);
    }

    void on_resize(int width, int height)
    {
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
                            application->getSkiaContext(), // context
                            backendRenderTarget,           // backend render target
                            kBottomLeft_GrSurfaceOrigin,   // surface origin
                            kN32_SkColorType,              // color type
                            SkColorSpace::MakeSRGB(),      // color space
                            &surface_properties,           // surface properties
                            nullptr,                       // release proc
                            nullptr                        // release context
                            )
                            .release();
    }

    void render(UIView *view)
    {
        this->surface->getCanvas()->clear(SK_ColorGRAY);

        if (view != nullptr)
        {
            view->layer.backingSurface->draw(this->surface->getCanvas(), 0, 0);
        }

        application->getSkiaContext()->flushAndSubmit();

        if (eglSwapBuffers(application->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
        {
            std::cerr << eglGetError() << " brokeyyy\n";
        }
    }
};

int main()
{
    UIApplication application = UIApplication::getSharedInstance();
    std::cout << "before window\n";
    UIWindow window(&application);
    window.render(nullptr);

    UIView *view1 = new UIView;
    MyView *view2 = new MyView;
    view2->frame = SkRect::MakeXYWH(0, 0, 100, 100);
    view2->layer.frame = SkRect::MakeXYWH(0, 0, 100, 100);

    const SkImageInfo info = SkImageInfo::MakeN32(100, 100, kPremul_SkAlphaType);
    std::cout << "get skia context\n";
    view2->layer.backingSurface = SkSurface::MakeRenderTarget(application.getSkiaContext(), SkBudgeted::kYes, info).release();
    view2->backgroundColor = SK_ColorBLUE;
    view2->layer.render();

    while (true)
    {
        wl_display_dispatch(application.display);
        if (window.toplevel->resized)
        {
            window.on_resize(window.toplevel->width, window.toplevel->height);
            window.render(view2);
            window.toplevel->resized = false;
        }
    }
}