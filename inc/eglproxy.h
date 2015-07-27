#ifndef EGLPROXY_H
#define EGLPROXY_H

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4668 )
#pragma warning( disable: 4820 )
#pragma warning( disable: 4255 )
#endif /* _MSC_VER */

#if defined(_WIN32) && !defined(__SCITECH_SNAP__)
#   if defined(eglproxy_EXPORTS)
#       define EGLAPI __declspec(dllexport)
#   else
#       define EGLAPI
#   endif
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>
#ifdef _MSC_VER
#pragma warning( pop )
#endif /* _MSC_VER */

typedef struct EGLProxyConfig {
    EGLint buffer_size;
    EGLint alpha_size;
    EGLint blue_size;
    EGLint green_size;
    EGLint red_size;
    EGLint depth_size;
    EGLint stencil_size;
    EGLint config_caveat;
    EGLint config_id;
    EGLint level;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_pixels;
    EGLint max_pbuffer_width;
    EGLint native_renderable;
    EGLint native_visual_id;
    EGLint native_visual_type;
    EGLint reserved;
    EGLint samples;
    EGLint sample_buffers;
    EGLint surface_type;
    EGLint transparent_type;
    EGLint transparent_blue_value;
    EGLint transparent_green_value;
    EGLint transparent_red_value;
    EGLint none;
    EGLint bind_to_texture_rgb;
    EGLint bind_to_texture_rgba;
    EGLint min_swap_interval;
    EGLint max_swap_interval;
    EGLint luminance_size;
    EGLint alpha_mask_size;
    EGLint color_buffer_type;
    EGLint renderable_type;
    EGLint native_pixmap;
    EGLint conformant;
    EGLint match_format_khr;
    void *platform;
} EGLProxyConfig;

typedef struct ContextAttributes {
    EGLint major_version;
    EGLint minor_version;
    EGLint profile_mask;
    EGLint debug;
    EGLint forward;
    EGLint robust_access;
    EGLint reset_strategy;
} ContextAttributes;

typedef struct EGLProxyContext {
    void *platform;
    struct EGLProxyContext *next;
} EGLProxyContext;

typedef struct SurfaceAttributes {
    EGLint gl_colorspace;
    EGLint vg_colorspace;
    EGLint vg_alpha_format;
    union {
        struct PBufferSurfaceAttributes {
            EGLint width;
            EGLint height;
            EGLint largest_pbuffer;
            EGLint texture_format;
            EGLint texture_target;
            EGLint mipmap_texture;
        } pbuffer;
        struct WindowSurfaceAttributes {
            EGLNativeWindowType id;
            EGLint render_buffer;
        } window;
    } specific;
} SurfaceAttributes;

enum SurfaceType {
    ST_Invalid,
    ST_Window,
    ST_PBuffer
};

typedef struct EGLProxySurface {
    enum SurfaceType type;
    void *platform;
    SurfaceAttributes attributes;
    struct EGLProxySurface *next;
} EGLProxySurface;

typedef struct PlatformDisplay PlatformDisplay;
typedef struct PlatformDisplayAttributes PlatformDisplayAttributes;

typedef struct EGLProxyDisplay {
    struct EGLProxyDisplay *next;
    PlatformDisplayAttributes *attributes;
    EGLProxyConfig *configs;
    EGLProxyContext *contexts;
    EGLProxySurface *surfaces;
    PlatformDisplay *platform;
    EGLNativeDisplayType display_id;
    EGLint n_configs;
    EGLBoolean initialized;
} EGLProxyDisplay;

#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

void eglSetError (EGLint error);
void *platform_create_context (PlatformDisplay *display,
                               EGLProxyConfig *egl_config,
                               ContextAttributes *attributes);
void platform_context_destroy (PlatformDisplay *display, void *context);
void *platform_window_surface_create (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config,
                                      SurfaceAttributes *attributes);
void *platform_pbuffer_surface_create (PlatformDisplay *display,
                                       EGLProxyConfig *egl_config,
                                       SurfaceAttributes *attributes);
void platform_window_surface_destroy (PlatformDisplay *display,
                                      void *drawable);
void platform_pbuffer_surface_destroy (PlatformDisplay *display,
                                       void *drawable);
PlatformDisplay *platform_display_create (const PlatformDisplayAttributes
        *attributes);
void platform_display_destroy (PlatformDisplay *display,
                               EGLNativeDisplayType id);
EGLint platform_display_initialize (PlatformDisplay *display,
                                    EGLProxyConfig **config_list);
EGLBoolean platform_make_current (PlatformDisplay *display,
                                  EGLProxySurface *draw,
                                  EGLProxySurface *read,
                                  EGLProxyContext *ctx);
EGLBoolean platform_swap_buffers (PlatformDisplay *display,
                                  EGLProxySurface *surface);

PlatformDisplayAttributes *platform_display_attributes_create (EGLenum platform,
        void *native_display, const EGLAttrib *attrib_list);

int platform_display_has_attributes (const EGLProxyDisplay *display,
                                     const PlatformDisplayAttributes *attributes);
__eglMustCastToProperFunctionPointerType platform_get_proc_address (
    const char *procname);
int window_is_match_config (PlatformDisplay *display, EGLNativeWindowType win,
                            EGLProxyConfig *config);
EGLBoolean window_is_valid (PlatformDisplay *display, EGLNativeWindowType win);
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif

#endif /* end of include guard: EGLPROXY_H */
