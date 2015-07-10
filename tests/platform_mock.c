#include "eglproxy.h"
#include <stdlib.h>

#define UNUSED(x) (void)(x)

struct PlatformDisplay {
    int dummy;
};

struct PlatformDisplayAttributes {
    int dummy;
};

void *platform_create_context (PlatformDisplay *display,
                               EGLProxyConfig *egl_config,
                               ContextAttributes *attributes)
{
    UNUSED (display);
    UNUSED (egl_config);
    UNUSED (attributes);
    return NULL;
}

void platform_context_destroy (PlatformDisplay *display, void *context)
{
    UNUSED (display);
    UNUSED (context);
}

int window_is_match_config (PlatformDisplay *display, EGLNativeWindowType win,
                            EGLProxyConfig *config)
{
    UNUSED (display);
    UNUSED (win);
    UNUSED (config);
    return 0;
}

void *platform_window_surface_create (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config,
                                      EGLNativeWindowType win)
{
    UNUSED (display);
    UNUSED (egl_config);
    UNUSED (win);
    return NULL;
}

void platform_window_surface_destroy (PlatformDisplay *display, void *drawable)
{
    UNUSED (display);
    UNUSED (drawable);
}

PlatformDisplayAttributes *platform_display_attributes_create (EGLenum platform,
        void *native_display, const EGLAttrib *attrib_list)
{
    UNUSED (platform);
    UNUSED (native_display);
    UNUSED (attrib_list);
    eglSetError (EGL_BAD_PARAMETER);
    return NULL;
}

int platform_display_has_attributes (const EGLProxyDisplay *display,
                                     const PlatformDisplayAttributes *attributes)
{
    UNUSED (display);
    UNUSED (attributes);
    return 0;
}

PlatformDisplay *platform_display_create (const PlatformDisplayAttributes
        *attributes)
{
    UNUSED (attributes);
    return NULL;
}

void platform_display_destroy (PlatformDisplay *display,
                               EGLNativeDisplayType id)
{
    UNUSED (display);
    UNUSED (id);
}

EGLint platform_display_initialize (PlatformDisplay *display,
                                    EGLProxyConfig **config_list)
{
    UNUSED (display);
    UNUSED (config_list);
    return 0;
}

EGLBoolean platform_make_current (PlatformDisplay *display,
                                  EGLProxySurface *draw, EGLProxySurface *read,
                                  EGLProxyContext *ctx)
{
    UNUSED (display);
    UNUSED (draw);
    UNUSED (read);
    UNUSED (ctx);
    return EGL_FALSE;
}

EGLBoolean platform_swap_buffers (PlatformDisplay *display,
                                  EGLProxySurface *surface)
{
    UNUSED (display);
    UNUSED (surface);
    return EGL_TRUE;
}

__eglMustCastToProperFunctionPointerType platform_get_proc_address (
    const char *procname)
{
    UNUSED (procname);
    return NULL;
}

EGLBoolean window_is_valid (PlatformDisplay *display, EGLNativeWindowType win)
{
    UNUSED (display);
    UNUSED (win);
    return EGL_TRUE;
}
