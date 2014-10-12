/**
 * @file egl_wgl.c
 * Implementation of EGL using WGL.
 */
#include "eglproxy.h"
#include <Windows.h>
#include <stdlib.h>

struct PlatformDisplay {
    HDC hdc;
};

struct PlatformDisplayAttributes {
    int unused;
};

void *platform_create_context (PlatformDisplay *display,
                               EGLProxyConfig *egl_config,
                               ContextAttributes *attributes)
{
    return NULL;
}

void platform_context_destroy (PlatformDisplay *display, void *context)
{

}

void *platform_window_surface_create (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config,
                                      EGLNativeWindowType win)
{
    return NULL;
}

void platform_window_surface_destroy (PlatformDisplay *display,
                                      void *drawable)
{

}

PlatformDisplay *platform_display_create (const PlatformDisplayAttributes
        *attributes)
{
    return NULL;
}

void platform_display_destroy (PlatformDisplay *display,
                               EGLNativeDisplayType id)
{

}

EGLint platform_display_initialize (PlatformDisplay *display,
                                    EGLProxyConfig **config_list)
{
    return 0;
}

EGLBoolean platform_make_current (PlatformDisplay *display,
                                  EGLProxySurface *draw,
                                  EGLProxySurface *read,
                                  EGLProxyContext *ctx)
{
    return EGL_FALSE;
}

EGLBoolean platform_swap_buffers (PlatformDisplay *display,
                                  EGLProxySurface *surface)
{
    return EGL_FALSE;
}

PlatformDisplayAttributes *platform_display_attributes_create (EGLenum platform,
        void *native_display, const EGLAttrib *attrib_list)
{
    return NULL;
}

int platform_display_has_attributes (const EGLProxyDisplay *display,
                                     const PlatformDisplayAttributes *attributes)
{
    return 0;
}