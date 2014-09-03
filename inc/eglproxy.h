#ifndef EGLPROXY_H
#define EGLPROXY_H

#include <EGL/egl.h>

typedef struct EGLProxyConfig {
    void *platform;
    EGLint buffer_size;
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint luminance_size;
    EGLint alpha_size;
    EGLint alpha_mask_size;
    EGLBoolean bind_to_texture_rgb;
    EGLBoolean bind_to_texture_rgba;
    EGLBoolean double_buffer;
    EGLint color_buffer_type;
    EGLint config_caveat;
    EGLint config_id;
    EGLint conformant;
    EGLint depth_size;
    EGLint level;
    EGLint max_pbuffer_width;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_pixels;
    EGLint max_swap_interval;
    EGLint min_swap_interval;
    EGLBoolean native_renderable;
    EGLint native_visual_id;
    EGLint native_visual_type;
    EGLint renderable_type;
    EGLint sample_buffers;
    EGLint samples;
    EGLint stencil_size;
    EGLint surface_type;
    EGLint transparent_type;
    EGLint transparent_red_value;
    EGLint transparent_green_value;
    EGLint transparent_blue_value;
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

typedef struct EGLProxySurface {
    void *platform;
    EGLNativeWindowType window;
    struct EGLProxySurface *next;
} EGLProxySurface;

typedef struct PlatformDisplay PlatformDisplay;

typedef struct EGLProxyDisplay {
    struct EGLProxyDisplay *next;
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

void *platform_create_context (PlatformDisplay *display,
                               EGLProxyConfig *egl_config,
                               ContextAttributes *attributes);
void platform_context_destroy (PlatformDisplay *display, void *context);
void *platform_window_surface_create (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config,
                                      EGLNativeWindowType win);
void platform_window_surface_destroy (PlatformDisplay *display,
                                      void *drawable);
PlatformDisplay *platform_display_create (EGLNativeDisplayType id);
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

#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif

#endif /* end of include guard: EGLPROXY_H */
