#include "Skia.hpp"

using namespace UI;

GrDirectContext *Skia::get_context()
{
    if (this->interface == nullptr && this->context == nullptr)
    {
        GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
        {
            return eglGetProcAddress(name);
        };
        this->interface = GrGLMakeAssembledGLESInterface(this, get_proc);
        this->context = GrDirectContext::MakeGL(interface).release();
    }

    return context;
}

void Skia::setup(int width, int height)
{
    std::cout << "Creating surface with " << width << " " << height << "\n";
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
    GrBackendRenderTarget backendRenderTarget(width, height,
                                              0, // sample count
                                              0, // stencil bits
                                              framebufferInfo);

    SkSurfaceProps surface_properties(0, kUnknown_SkPixelGeometry);

    this->surface = SkSurface::MakeFromBackendRenderTarget(
                        this->get_context(),         // context
                        backendRenderTarget,         // backend render target
                        kBottomLeft_GrSurfaceOrigin, // surface origin
                        kN32_SkColorType,            // color type
                        SkColorSpace::MakeSRGB(),    // color space
                        &surface_properties,         // surface properties
                        nullptr,                     // release proc
                        nullptr                      // release context
                        )
                        .release();

    assert(this->surface != nullptr);
}

Skia::~Skia()
{
    if (this->surface != nullptr)
    {
        delete this->surface;
    }

    if (this->interface != nullptr)
    {
        this->context->releaseResourcesAndAbandonContext();
    }

    if (this->context != nullptr)
    {
        delete this->context;
    }
}