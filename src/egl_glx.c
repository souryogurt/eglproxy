/**
 * @file egl_glx.c
 * Implementation of EGL using GLX.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <GL/glx.h>
#include <EGL/egl.h>

#ifndef GLX_ARB_create_context
#define GLX_ARB_create_context 1
#define GLX_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB     0x2092
#define GLX_CONTEXT_FLAGS_ARB             0x2094
typedef GLXContext ( *PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *dpy,
        GLXFBConfig config, GLXContext share_context, Bool direct,
        const int *attrib_list);
#endif /* GLX_ARB_create_context */

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

typedef struct EGLProxyConfigEntry {
    EGLProxyConfig *config;
    EGLint caveat;
    EGLint buffer_type;
    EGLint n_colorbits;
} EGLProxyConfigEntry;

typedef struct EGLConfigQuery {
    EGLint buffer_size;
    EGLint red_size;
    EGLint green_size;
    EGLint blue_size;
    EGLint luminance_size;
    EGLint alpha_size;
    EGLint alpha_mask_size;
    EGLint bind_to_texture_rgb;
    EGLint bind_to_texture_rgba;
    EGLint color_buffer_type;
    EGLint config_caveat;
    EGLint config_id;
    EGLint conformant;
    EGLint depth_size;
    EGLint level;
    EGLint match_native_pixmap;
    EGLint max_swap_interval;
    EGLint min_swap_interval;
    EGLint native_renderable;
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
    EGLint native_visual_id;
    EGLint max_pbuffer_width;
    EGLint max_pbuffer_height;
    EGLint max_pbuffer_pixels;
} EGLConfigQuery;

static const EGLConfigQuery default_query = {
    0, /* EGL_BUFFER_SIZE */
    0, /* EGL_RED_SIZE */
    0, /* EGL_GREEN_SIZE */
    0, /* EGL_BLUE_SIZE */
    0, /* EGL_LUMINANCE_SIZE */
    0, /* EGL_ALPHA_SIZE */
    0, /* EGL_ALPHA_MASK_SIZE */
    EGL_DONT_CARE, /* EGL_BIND_TO_TEXTURE_RGB */
    EGL_DONT_CARE, /* EGL_BIND_TO_TEXTURE_RGBA */
    EGL_RGB_BUFFER, /* EGL_COLOR_BUFFER_TYPE */
    EGL_DONT_CARE, /* EGL_CONFIG_CAVEAT */
    EGL_DONT_CARE, /* EGL_CONFIG_ID */
    0, /* EGL_CONFORMANT */
    0, /* EGL_DEPTH_SIZE */
    0, /* EGL_LEVEL */
    EGL_NONE, /* EGL_MATCH_NATIVE_PIXMAP */
    EGL_DONT_CARE, /* EGL_MAX_SWAP_INTERVAL */
    EGL_DONT_CARE, /* EGL_MIN_SWAP_INTERVAL */
    EGL_DONT_CARE, /* EGL_NATIVE_RENDERABLE */
    EGL_DONT_CARE, /* EGL_NATIVE_VISUAL_TYPE */
    EGL_OPENGL_ES_BIT, /* EGL_RENDERABLE_TYPE */
    0, /* EGL_SAMPLE_BUFFERS */
    0, /* EGL_SAMPLES */
    0, /* EGL_STENCIL_SIZE */
    EGL_WINDOW_BIT, /* EGL_SURFACE_TYPE */
    EGL_NONE, /* EGL_TRANSPARENT_TYPE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_RED_VALUE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_GREEN_VALUE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_BLUE_VALUE */
    EGL_DONT_CARE, /* EGL_NATIVE_VISUAL_ID */
    EGL_DONT_CARE, /* EGL_MAX_PBUFFER_WIDTH */
    EGL_DONT_CARE, /* EGL_MAX_PBUFFER_HEIGHT */
    EGL_DONT_CARE /* EGL_MAX_PBUFFER_PIXELS */
};

typedef struct ContextAttributes {
    EGLint major_version;
    EGLint minor_version;
    EGLint profile_mask;
    EGLint debug;
    EGLint forward;
    EGLint robust_access;
    EGLint reset_strategy;
} ContextAttributes;

static const ContextAttributes default_context_attributes = {
    1, /* EGL_CONTEXT_MAJOR_VERSION */
    0, /* EGL_CONTEXT_MINOR_VERSION */
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, /* EGL_CONTEXT_OPENGL_PROFILE_MASK */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_DEBUG */
    EGL_FALSE, /* EGL_CONTEXT_FORWARD_COMPATIBLE */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_ROBUST_ACCESS */
    EGL_NO_RESET_NOTIFICATION /* EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY */
};

typedef struct EGLProxyContext {
    void *platform;
    struct EGLProxyContext *next;
} EGLProxyContext;

typedef struct EGLProxySurface {
    void *platform;
    EGLNativeWindowType window;
    struct EGLProxySurface *next;
} EGLProxySurface;

typedef struct PlatformDisplay {
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
    Display *x11_display; /**< Connection to X11 display */
    int screen; /**< Screen number where UGL where created */
    int glx_major; /**< Major version of glx */
    int glx_minor; /**< Minor version of glx */
    int is_modern; /**< Is we have modern GLX version (GLX > 1.2) */
    int is_arb_context_profile; /**< Is GLX_ARB_create_context_profile there*/
    int is_ext_visual_rating; /**< Is GLX_EXT_visual_rating there */
    int is_ext_visual_info; /**< Is GLX_EXT_visual_info there */
    int is_arb_multisample; /**< Is GLX_ARB_multisample there */
} PlatformDisplay;

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

static EGLProxyDisplay *displays = NULL;

static EGLenum CurrentAPI = EGL_NONE; /*TODO: Should be in TLS */

#define CHECK_EGLCONFIG(dpy, config) { \
if (((EGLProxyConfig*)(config) < ((EGLProxyDisplay*)(dpy))->configs) || \
    ((EGLProxyConfig*)(config) >= &((EGLProxyDisplay*)(dpy))->configs[((EGLProxyDisplay*)(dpy))->n_configs])){ \
    eglSetError (EGL_BAD_PARAMETER); \
    return EGL_FALSE; \
} \
}while(0)

#define UNUSED(x) (void)(x)

static void *platform_create_context (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config, ContextAttributes *attributes)
{
    GLXContext context = NULL;
    if (display->is_modern) {
        GLXFBConfig glx_config = (GLXFBConfig) egl_config->platform;
        if (display->glXCreateContextAttribsARB) {
            int context_attribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
            };
            context_attribs[1] = attributes->major_version;
            context_attribs[3] = attributes->major_version;
            /* TODO: other attributes */
            context = display->glXCreateContextAttribsARB (
                          display->x11_display,
                          glx_config,
                          /* TODO: share_context */ NULL,
                          True, context_attribs);
        } else {
            /* If theare no GLX_ARB_create_context extension, then fall back to
             * standard GLX1.3 way */
            context = glXCreateNewContext (display->x11_display, glx_config,
                                           GLX_RGBA_TYPE, NULL, True);
        }
    } else {
        int n_visuals;
        XVisualInfo *visual_info = NULL;
        XVisualInfo info_template;
        info_template.visualid = (VisualID) egl_config->native_visual_id;
        visual_info = XGetVisualInfo (display->x11_display, VisualIDMask,
                                      &info_template, &n_visuals);
        if (visual_info != NULL) {
            context = glXCreateContext (display->x11_display, visual_info,
                                        NULL, True);
            XFree (visual_info);
        }
    }
    return context;
}

static void platform_context_destroy (PlatformDisplay *display,
                                      void *context)
{
    glXDestroyContext (display->x11_display, (GLXContext)context);
}

static void *platform_window_surface_create (PlatformDisplay *display,
        EGLProxyConfig *egl_config, EGLNativeWindowType win)
{
    GLXDrawable result = win;
    if (display->is_modern) {
        GLXFBConfig glx_config = (GLXFBConfig) egl_config->platform;
        result = glXCreateWindow (display->x11_display, glx_config, win, NULL);
    }
    return (void *) result;
}

static void platform_window_surface_destroy (PlatformDisplay *display,
        void *drawable)
{
    if (display->is_modern) {
        glXDestroyWindow (display->x11_display, (GLXDrawable) drawable);
    }
}

static PlatformDisplay *platform_display_create (EGLNativeDisplayType id)
{
    Display *x11_display = NULL;
    int screen = 0;
    int glx_major = 0;
    int glx_minor = 0;
    if (id == EGL_DEFAULT_DISPLAY) {
        x11_display = XOpenDisplay ((char *)NULL);
        if (x11_display == NULL) {
            return NULL;
        }
        screen = DefaultScreen (x11_display);
    } else {
        /* TODO: parse attributes
         * Should set x11_display and screen
         */
        return NULL;
    }
    if (glXQueryVersion (x11_display, &glx_major, &glx_minor) == True) {
        if ((glx_major > 1) || ((glx_major == 1) && (glx_minor >= 2))) {
            PlatformDisplay *display = (PlatformDisplay *) calloc (1,
                                       sizeof (PlatformDisplay));
            if (display != NULL) {
                display->x11_display = x11_display;
                display->screen = screen;
                display->glx_major = glx_major;
                display->glx_minor = glx_minor;
                return display;
            }
        }
    }
    return NULL;
}

static void platform_display_destroy (EGLNativeDisplayType id,
                                      PlatformDisplay *display)
{
    if (id == EGL_DEFAULT_DISPLAY) {
        XCloseDisplay (display->x11_display);
    }
    free (display);
}

static EGLint glx_populate_from_fbconfigs (PlatformDisplay *display,
        EGLProxyConfig **config_list)
{
    int value;
    int n_configs = 0;
    int current_config = 0;
    EGLProxyConfig *egl_config = NULL;
    GLXFBConfig glx_config;
    GLXFBConfig *glx_configs = glXGetFBConfigs (display->x11_display,
                               display->screen, &n_configs);
    EGLProxyConfig *list = (EGLProxyConfig *) calloc ((size_t)n_configs,
                           sizeof (EGLProxyConfig));
    if (list == NULL) {
        return 0;
    }
    *config_list = list;
    while (n_configs > 0) {
        n_configs--;
        glx_config = glx_configs[n_configs];
        egl_config = list;
        egl_config->platform = glx_config;
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_RENDER_TYPE,
                              &value);
        value &= GLX_RGBA_BIT;
        if (!value) {
            continue;
        }
        egl_config->color_buffer_type = EGL_RGB_BUFFER;

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_CONFIG_CAVEAT, &value);
        if (value == GLX_NONE) {
            egl_config->config_caveat = EGL_NONE;
        } else if (value == GLX_SLOW_CONFIG) {
            egl_config->config_caveat = EGL_SLOW_CONFIG;
        } else {
            egl_config->config_caveat = EGL_NON_CONFORMANT_CONFIG;
        }
        egl_config->conformant = EGL_OPENGL_BIT;
        egl_config->renderable_type = EGL_OPENGL_BIT;
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_BUFFER_SIZE,
                              (int *)&egl_config->buffer_size);
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_RED_SIZE,
                              (int *)&egl_config->red_size);
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_GREEN_SIZE,
                              (int *)&egl_config->green_size);
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_BLUE_SIZE,
                              (int *)&egl_config->blue_size);
        egl_config->luminance_size = 0;
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_ALPHA_SIZE,
                              (int *)&egl_config->alpha_size);
        egl_config->alpha_mask_size = 0;
        egl_config->bind_to_texture_rgb = EGL_FALSE;
        egl_config->bind_to_texture_rgba = EGL_FALSE;

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_DOUBLEBUFFER, &value);
        egl_config->double_buffer = (value == True) ? EGL_TRUE : EGL_FALSE;
        egl_config->config_id = current_config + 1;
        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_DEPTH_SIZE, (int *)&egl_config->depth_size);
        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_LEVEL,
                              (int *)&egl_config->level);
        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_MAX_PBUFFER_WIDTH,
                              (int *)&egl_config->max_pbuffer_width);
        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_MAX_PBUFFER_HEIGHT,
                              (int *)&egl_config->max_pbuffer_height);
        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_MAX_PBUFFER_PIXELS,
                              (int *)&egl_config->max_pbuffer_pixels);
        /* TODO: Implement via GLX_EXT_swap_control
         *
         * From GLX_EXT_swap_control specification:
         * The current swap interval and implementation-dependent max swap
         * interval for a particular drawable can be obtained by calling
         * glXQueryDrawable with the attributes GLX_SWAP_INTERVAL_EXT and
         * GLX_MAX_SWAP_INTERVAL_EXT respectively. The value returned by
         * glXQueryDrawable is undefined if the drawable is not a GLXWindow
         * and these attributes are given.
         */
        egl_config->max_swap_interval = 1;
        egl_config->min_swap_interval = 1;

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_X_RENDERABLE, &value);
        egl_config->native_renderable = (value == True) ? EGL_TRUE : EGL_FALSE;

        glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_VISUAL_ID,
                              (int *)&egl_config->native_visual_id);
        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_X_VISUAL_TYPE,
                              (int *)&egl_config->native_visual_type);

        /* Sample buffers are supported only in GLX 1.4 */
        if ((display->glx_minor > 1) ||
                ((display->glx_major == 1) && (display->glx_minor >= 4))) {
            glXGetFBConfigAttrib (display->x11_display, glx_config,
                                  GLX_SAMPLE_BUFFERS,
                                  (int *)&egl_config->sample_buffers);
            glXGetFBConfigAttrib (display->x11_display, glx_config, GLX_SAMPLES,
                                  (int *)&egl_config->samples);
        } else if (display->is_arb_multisample) {
            XVisualInfo info_template;
            int n_visualinfo = 0;
            XVisualInfo *info = NULL;
            info_template.visualid = (VisualID) egl_config->native_visual_id;
            info = XGetVisualInfo (display->x11_display, VisualIDMask, &info_template,
                                   &n_visualinfo);
            if (info ) {
                glXGetConfig (display->x11_display, info, GLX_SAMPLE_BUFFERS_ARB,
                              (int *)&egl_config->sample_buffers);
                glXGetConfig (display->x11_display, info, GLX_SAMPLES_ARB,
                              (int *)&egl_config->samples);
                XFree (info);
            }
        }

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_STENCIL_SIZE, (int *)&egl_config->stencil_size);

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_DRAWABLE_TYPE, &value);
        egl_config->surface_type = 0;
        egl_config->surface_type |= (value & GLX_WINDOW_BIT) ? EGL_WINDOW_BIT : 0;
        egl_config->surface_type |= (value & GLX_PIXMAP_BIT) ? EGL_PIXMAP_BIT : 0;
        egl_config->surface_type |= (value & GLX_PBUFFER_BIT) ? EGL_PBUFFER_BIT : 0;

        glXGetFBConfigAttrib (display->x11_display, glx_config,
                              GLX_TRANSPARENT_TYPE,
                              &value);
        if (value == GLX_TRANSPARENT_RGB) {
            egl_config->transparent_type = EGL_TRANSPARENT_RGB;
            glXGetFBConfigAttrib (display->x11_display, glx_config,
                                  GLX_TRANSPARENT_RED_VALUE,
                                  (int *)&egl_config->transparent_red_value);
            glXGetFBConfigAttrib (display->x11_display, glx_config,
                                  GLX_TRANSPARENT_GREEN_VALUE,
                                  (int *)&egl_config->transparent_green_value);
            glXGetFBConfigAttrib (display->x11_display, glx_config,
                                  GLX_TRANSPARENT_BLUE_VALUE,
                                  (int *)&egl_config->transparent_blue_value);
        } else {
            egl_config->transparent_type = EGL_NONE;
        }
        current_config++;
        list++;
    }
    return current_config;
}

static EGLint glx_populate_from_visualinfos (PlatformDisplay *display,
        EGLProxyConfig **config_list)
{
    int n_configs = 0;
    int value = 0;
    int current_config = 0;
    XVisualInfo info_template;
    XVisualInfo *infos = NULL;
    EGLProxyConfig *egl_config = NULL;
    info_template.screen = display->screen;
    infos = XGetVisualInfo (display->x11_display, VisualScreenMask,
                            &info_template, &n_configs);
    *config_list = (EGLProxyConfig *) calloc ((size_t)n_configs,
                   sizeof (EGLProxyConfig));
    if (*config_list == NULL) {
        XFree (infos);
        return 0;
    }
    while (n_configs > 0) {
        int err = 0;
        XVisualInfo *info = &infos[n_configs];
        n_configs--;
        egl_config = & (*config_list)[current_config];
        err = glXGetConfig (display->x11_display, info, GLX_USE_GL, &value);
        if (err != 0 || value == False) {
            continue;
        }
        err = glXGetConfig (display->x11_display, info, GLX_BUFFER_SIZE,
                            (int *) &egl_config->buffer_size);
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_RED_SIZE,
                                (int *) &egl_config->red_size);
        }
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_GREEN_SIZE,
                                (int *) &egl_config->green_size);
        }
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_BLUE_SIZE,
                                (int *) &egl_config->blue_size);
        }
        egl_config->luminance_size = 0;
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_ALPHA_SIZE,
                                (int *) &egl_config->alpha_size);
        }
        egl_config->alpha_mask_size = 0;
        egl_config->bind_to_texture_rgb = 0;
        egl_config->bind_to_texture_rgba = 0;
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_DOUBLEBUFFER, &value);
        }
        egl_config->double_buffer = (value == True) ? EGL_TRUE : EGL_FALSE;
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_RGBA, &value);
        }
        if (err != 0 || value != True) {
            continue;
        }
        egl_config->color_buffer_type = EGL_RGB_BUFFER;
        egl_config->config_caveat = EGL_NONE;
        if ((display->is_ext_visual_rating)
                && (glXGetConfig (display->x11_display, info, GLX_VISUAL_CAVEAT_EXT,
                                  &value) == 0)) {
            if (value == GLX_NONE_EXT) {
                egl_config->config_caveat = EGL_NONE;
            } else if (value == GLX_SLOW_VISUAL_EXT) {
                egl_config->config_caveat = EGL_SLOW_CONFIG;
            } else {
                egl_config->config_caveat = EGL_NON_CONFORMANT_CONFIG;
            }
        }
        egl_config->config_id = current_config + 1;
        egl_config->conformant = EGL_OPENGL_BIT;
        egl_config->renderable_type = EGL_OPENGL_BIT;
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_DEPTH_SIZE,
                                (int *) &egl_config->depth_size);
        }
        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_LEVEL,
                                (int *) &egl_config->level);
        }
        egl_config->max_pbuffer_width = 0;
        egl_config->max_pbuffer_height = 0;
        egl_config->max_pbuffer_pixels = 0;

        /* TODO: Implement via GLX_EXT_swap_control
         *
         * From GLX_EXT_swap_control specification:
         * The current swap interval and implementation-dependent max swap
         * interval for a particular drawable can be obtained by calling
         * glXQueryDrawable with the attributes GLX_SWAP_INTERVAL_EXT and
         * GLX_MAX_SWAP_INTERVAL_EXT respectively. The value returned by
         * glXQueryDrawable is undefined if the drawable is not a GLXWindow
         * and these attributes are given.
         */
        egl_config->max_swap_interval = 1;
        egl_config->min_swap_interval = 1;

        /* Native rendering is supported, since config is based on XVisualInfo */
        egl_config->native_renderable = EGL_TRUE;
        egl_config->native_visual_id = (EGLint) info->visualid;

        if ((display->is_ext_visual_info)
                && (glXGetConfig (display->x11_display, info, GLX_X_VISUAL_TYPE_EXT,
                                  &value) == 0)) {
            egl_config->native_visual_type = value;
        }

        if (display->is_arb_multisample) {
            glXGetConfig (display->x11_display, info, GLX_SAMPLE_BUFFERS_ARB,
                          (int *)&egl_config->sample_buffers);
            glXGetConfig (display->x11_display, info, GLX_SAMPLES_ARB,
                          (int *)&egl_config->samples);
        }

        if (err == 0) {
            err = glXGetConfig (display->x11_display, info, GLX_STENCIL_SIZE,
                                (int *) &egl_config->stencil_size);
        }
        egl_config->surface_type = EGL_WINDOW_BIT | EGL_PIXMAP_BIT;

        egl_config->transparent_type = EGL_NONE;
        if ((display->is_ext_visual_info)
                && (glXGetConfig (display->x11_display, info, GLX_TRANSPARENT_TYPE_EXT,
                                  &value) == 0)) {
            if (value == GLX_TRANSPARENT_RGB_EXT) {
                egl_config->transparent_type = EGL_TRANSPARENT_RGB;
                glXGetConfig (display->x11_display, info, GLX_TRANSPARENT_RED_VALUE_EXT,
                              (int *)&egl_config->transparent_red_value);
                glXGetConfig (display->x11_display, info, GLX_TRANSPARENT_GREEN_VALUE_EXT,
                              (int *)&egl_config->transparent_green_value);
                glXGetConfig (display->x11_display, info, GLX_TRANSPARENT_BLUE_VALUE_EXT,
                              (int *)&egl_config->transparent_blue_value);
            }
        }
        if (err != 0 ) {
            continue;
        }
        current_config++;
    }
    XFree (infos);
    return current_config;
}

/** Check that extension is in a list
 * @param ext_string list of extensions separated by a space
 * @param ext the extension to check in a list
 * @returns 1 if extension is present in extensions list, 0 otherwise
 */
static int is_extension_supported (const char *ext_string, const char *ext)
{
    const char *start = ext_string;
    const char *where = NULL;
    size_t ext_length = strlen (ext);
    while ((where = strstr (start, ext)) != NULL) {
        const char *end = where + ext_length;
        /* Check that is single word */
        if (((*end == ' ') || (*end == '\0'))
                && ((where == start) || * (where - 1) == ' ')) {
            return 1;
        }
        start = end;
    }
    return 0;
}

static EGLint platform_display_initialize (PlatformDisplay *display,
        EGLProxyConfig **config_list)
{
    const char *extensions = NULL;
    display->is_modern = ((display->glx_major == 1) && (display->glx_minor > 2))
                         || (display->glx_major > 1);
    /* Check available GLX extensions */
    extensions = glXQueryExtensionsString (display->x11_display,
                                           display->screen);

    if (is_extension_supported (extensions, "GLX_ARB_create_context")) {
        display->glXCreateContextAttribsARB =
            (PFNGLXCREATECONTEXTATTRIBSARBPROC)
            glXGetProcAddressARB ((const GLubyte *)"glXCreateContextAttribsARB");
        display->is_arb_context_profile = is_extension_supported (extensions,
                                          "GLX_ARB_create_context_profile");
    }
    display->is_ext_visual_rating = is_extension_supported (
                                        extensions,
                                        "GLX_EXT_visual_rating");
    display->is_ext_visual_info = is_extension_supported (
                                      extensions,
                                      "GLX_EXT_visual_info");
    display->is_arb_multisample = is_extension_supported (
                                      extensions,
                                      "GLX_ARB_multisample");
    if (display->is_modern) {
        return glx_populate_from_fbconfigs (display, config_list);
    } else {
        return glx_populate_from_visualinfos (display, config_list);
    }
}

static EGLBoolean platform_make_current (PlatformDisplay *display,
        EGLProxySurface *draw, EGLProxySurface *read, EGLProxyContext *ctx)
{
    GLXDrawable x11_draw = draw ? (GLXDrawable) draw->platform : (GLXDrawable)NULL;
    GLXDrawable x11_read = read ? (GLXDrawable) read->platform : (GLXDrawable)NULL;
    GLXContext x11_context = ctx ? (GLXContext) ctx->platform : (GLXContext) NULL;
    if (display->is_modern) {
        return glXMakeContextCurrent (display->x11_display, x11_draw, x11_read,
                                      x11_context) == True ? EGL_TRUE : EGL_FALSE;
    }
    return glXMakeCurrent (display->x11_display, x11_draw,
                           x11_context) == True ? EGL_TRUE : EGL_FALSE;
}

static EGLBoolean platform_swap_buffers (PlatformDisplay *display,
        EGLProxySurface *surface)
{
    glXSwapBuffers (display->x11_display, (GLXDrawable) surface->platform);
    return EGL_TRUE;
}

static void eglSetError (EGLint error)
{
    UNUSED (error);
}

EGLBoolean EGLAPIENTRY eglBindAPI (EGLenum api)
{
    if (api == EGL_OPENGL_API) {
        /* TODO: set API for current THREAD */
        CurrentAPI = api;
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
    }
    eglSetError (EGL_BAD_PARAMETER);
    return EGL_FALSE;
}

static int config_comparator (const void *lvalue, const void *rvalue)
{
    const EGLProxyConfigEntry *lcfg = (const EGLProxyConfigEntry *) lvalue;
    const EGLProxyConfigEntry *rcfg = (const EGLProxyConfigEntry *) rvalue;
    if (lcfg->caveat < rcfg->caveat) {
        return +1;
    } else if (lcfg->caveat > rcfg->caveat) {
        return -1;
    }
    if (lcfg->buffer_type < rcfg->buffer_type) {
        return +1;
    } else if (lcfg->buffer_type > rcfg->buffer_type) {
        return -1;
    }
    if (lcfg->n_colorbits < rcfg->n_colorbits) {
        return +1;
    } else if (lcfg->n_colorbits > rcfg->n_colorbits) {
        return -1;
    }
    if (lcfg->config->buffer_size > rcfg->config->buffer_size) {
        return +1;
    } else if (lcfg->config->buffer_size < rcfg->config->buffer_size) {
        return -1;
    }

    /* WARN: This rule is not present in EGL specification and added here
     * because we can't specify single/double buffered surface in
     * eglCreateWindowSurface call as requrired by specification. So
     * implementation will just prefere doublebuffered configs like GLX do for
     * window configs */
    if ((lcfg->config->double_buffer == EGL_TRUE)
            && (rcfg->config->double_buffer == EGL_FALSE)) {
        return -1;
    } else if ((lcfg->config->double_buffer == EGL_FALSE)
               && (rcfg->config->double_buffer == EGL_TRUE)) {
        return +1;
    }

    if (lcfg->config->sample_buffers > rcfg->config->sample_buffers) {
        return +1;
    } else if (lcfg->config->sample_buffers < rcfg->config->sample_buffers) {
        return -1;
    }
    if (lcfg->config->samples > rcfg->config->samples) {
        return +1;
    } else if (lcfg->config->samples < rcfg->config->samples) {
        return -1;
    }
    if (lcfg->config->depth_size > rcfg->config->depth_size) {
        return +1;
    } else if (lcfg->config->depth_size < rcfg->config->depth_size) {
        return -1;
    }
    if (lcfg->config->stencil_size > rcfg->config->stencil_size) {
        return +1;
    } else if (lcfg->config->stencil_size < rcfg->config->stencil_size) {
        return -1;
    }
    if (lcfg->config->alpha_mask_size > rcfg->config->alpha_mask_size) {
        return +1;
    } else if (lcfg->config->alpha_mask_size < rcfg->config->alpha_mask_size) {
        return -1;
    }
    if (lcfg->config->native_visual_type > rcfg->config->native_visual_type) {
        return +1;
    } else if (lcfg->config->native_visual_type <
               rcfg->config->native_visual_type) {
        return -1;
    }
    if (lcfg->config->config_id > rcfg->config->config_id) {
        return +1;
    } else if (lcfg->config->config_id < rcfg->config->config_id) {
        return -1;
    }
    return 0;
}

static EGLint select_config (EGLProxyDisplay *egl_display,
                             EGLProxyConfigEntry *selected,
                             const EGLConfigQuery *query)
{
    EGLint n_selected = 0;
    EGLint i;
    for (i = 0; (i < egl_display->n_configs); i++) {
        EGLProxyConfig *cfg = &egl_display->configs[i];
        selected[n_selected].n_colorbits = 0;
        if ((query->config_id != EGL_DONT_CARE)
                && (cfg->config_id != query->config_id)) {
            continue;
        }
        if ((query->buffer_size != EGL_DONT_CARE)
                && (cfg->buffer_size < query->buffer_size)) {
            continue;
        }
        if (query->red_size != EGL_DONT_CARE) {
            if (cfg->red_size < query->red_size) {
                continue;
            }
            selected[n_selected].n_colorbits += cfg->red_size;
        }
        if (query->green_size != EGL_DONT_CARE) {
            if (cfg->green_size < query->green_size) {
                continue;
            }
            selected[n_selected].n_colorbits += cfg->green_size;
        }
        if (query->blue_size != EGL_DONT_CARE) {
            if (cfg->blue_size < query->blue_size) {
                continue;
            }
            selected[n_selected].n_colorbits += cfg->blue_size;
        }
        if ((query->luminance_size != EGL_DONT_CARE)
                && (cfg->luminance_size < query->luminance_size)) {
            continue;
        }
        if (query->alpha_size != EGL_DONT_CARE) {
            if (cfg->alpha_size < query->alpha_size) {
                continue;
            }
            selected[n_selected].n_colorbits += cfg->alpha_size;
        }
        if ((query->alpha_mask_size != EGL_DONT_CARE)
                && (cfg->alpha_mask_size < query->alpha_mask_size)) {
            continue;

        }
        if ((query->bind_to_texture_rgb != EGL_DONT_CARE)
                && (cfg->bind_to_texture_rgb != (EGLBoolean)
                    query->bind_to_texture_rgb)) {
            continue;
        }
        if ((query->bind_to_texture_rgba != EGL_DONT_CARE)
                && (cfg->bind_to_texture_rgba != (EGLBoolean)
                    query->bind_to_texture_rgba)) {
            continue;
        }
        if ((query->color_buffer_type != EGL_DONT_CARE)
                && (cfg->color_buffer_type != query->color_buffer_type)) {
            continue;
        }
        if ((query->config_caveat != EGL_DONT_CARE)
                && (cfg->config_caveat != query->config_caveat)) {
            continue;
        }
        if ((query->conformant != EGL_DONT_CARE)
                && ((cfg->conformant & query->conformant) != query->conformant)) {
            continue;
        }
        if ((query->depth_size != EGL_DONT_CARE)
                && (cfg->depth_size < query->depth_size)) {
            continue;
        }
        if (cfg->level != query->level) {
            continue;
        }

        /* TODO: EGL_MATCH_NATIVE_PIXMAP is ignored */


        if ((query->max_swap_interval != EGL_DONT_CARE)
                && (cfg->max_swap_interval != query->max_swap_interval)) {
            continue;
        }
        if ((query->min_swap_interval != EGL_DONT_CARE)
                && (cfg->min_swap_interval != query->min_swap_interval)) {
            continue;
        }
        if ((query->native_renderable != EGL_DONT_CARE)
                && (cfg->native_renderable != (EGLBoolean)query->native_renderable)) {
            continue;
        }
        if ((query->renderable_type != EGL_DONT_CARE)
                && ((cfg->renderable_type & query->renderable_type) !=
                    query->renderable_type)) {
            continue;
        }
        if ((query->sample_buffers != EGL_DONT_CARE)
                && (cfg->sample_buffers < query->sample_buffers)) {
            continue;
        }
        if ((query->samples != EGL_DONT_CARE)
                && (cfg->samples < query->samples)) {
            continue;
        }
        if ((query->stencil_size != EGL_DONT_CARE)
                && (cfg->stencil_size < query->stencil_size)) {
            continue;
        }
        if ((query->surface_type != EGL_DONT_CARE)
                && ((cfg->surface_type & query->surface_type) !=
                    query->surface_type)) {
            continue;
        }
        if ((query->surface_type & EGL_WINDOW_BIT)
                && (query->native_visual_type != EGL_DONT_CARE)
                && (cfg->native_visual_type != query->native_visual_type)) {
            continue;
        }
        if ((query->transparent_type != EGL_DONT_CARE)
                && (cfg->transparent_type != query->transparent_type)) {
            continue;
        }
        if ((query->transparent_red_value != EGL_DONT_CARE)
                && (cfg->transparent_type != EGL_NONE)
                && (cfg->transparent_red_value != query->transparent_red_value)) {
            continue;
        }
        if ((query->transparent_green_value != EGL_DONT_CARE)
                && (cfg->transparent_type != EGL_NONE)
                && (cfg->transparent_green_value != query->transparent_green_value)) {
            continue;
        }
        if ((query->transparent_blue_value != EGL_DONT_CARE)
                && (cfg->transparent_type != EGL_NONE)
                && (cfg->transparent_blue_value != query->transparent_blue_value)) {
            continue;
        }
        switch (cfg->config_caveat) {
            case EGL_NONE:
                selected[n_selected].caveat = 3;
                break;
            case EGL_SLOW_CONFIG:
                selected[n_selected].caveat = 2;
                break;
            case EGL_NON_CONFORMANT_CONFIG:
                selected[n_selected].caveat = 1;
                break;
            default:
                /* No other variants actually */
                selected[n_selected].caveat = 0;
                break;
        }
        if (cfg->color_buffer_type == EGL_RGB_BUFFER) {
            selected[n_selected].buffer_type = 1;
        } else {
            selected[n_selected].buffer_type = 0;
        }

        selected[n_selected].config = cfg;
        n_selected++;
    }
    return n_selected;
}

static int  fill_query (EGLConfigQuery *query, const EGLint *attrib_list)
{
    size_t i = 0;
    if (attrib_list == NULL) {
        return 1;
    }

    for (i = 0; attrib_list[i] != EGL_NONE; i += 2) {
        EGLint value = attrib_list[i + 1];
        switch (attrib_list[i]) {
            case EGL_BUFFER_SIZE:
                query->buffer_size = value;
                break;
            case EGL_RED_SIZE:
                query->red_size = value;
                break;
            case EGL_GREEN_SIZE:
                query->green_size = value;
                break;
            case EGL_BLUE_SIZE:
                query->blue_size = value;
                break;
            case EGL_LUMINANCE_SIZE:
                query->luminance_size = value;
                break;
            case EGL_ALPHA_SIZE:
                query->alpha_size = value;
                break;
            case EGL_ALPHA_MASK_SIZE:
                query->alpha_mask_size = value;
                break;
            case EGL_BIND_TO_TEXTURE_RGB:
                query->bind_to_texture_rgb = value;
                break;
            case EGL_BIND_TO_TEXTURE_RGBA:
                query->bind_to_texture_rgba = value;
                break;
            case EGL_COLOR_BUFFER_TYPE:
                query->color_buffer_type = value;
                break;
            case EGL_CONFIG_CAVEAT:
                query->config_caveat = value;
                break;
            case EGL_CONFIG_ID:
                query->config_id = value;
                break;
            case EGL_CONFORMANT:
                query->conformant = value;
                break;
            case EGL_DEPTH_SIZE:
                query->depth_size = value;
                break;
            case EGL_LEVEL:
                if (value == EGL_DONT_CARE) {
                    return 0;
                }
                query->level = value;
                break;
            case EGL_MATCH_NATIVE_PIXMAP:
                if (value == EGL_DONT_CARE) {
                    return 0;
                }
                query->match_native_pixmap = value;
                break;
            case EGL_MAX_SWAP_INTERVAL:
                query->max_swap_interval = value;
                break;
            case EGL_MIN_SWAP_INTERVAL:
                query->min_swap_interval = value;
                break;
            case EGL_NATIVE_RENDERABLE:
                query->native_renderable = value;
                break;
            case EGL_NATIVE_VISUAL_TYPE:
                query->native_visual_type = value;
                break;
            case EGL_RENDERABLE_TYPE:
                query->renderable_type = value;
                break;
            case EGL_SAMPLE_BUFFERS:
                query->sample_buffers = value;
                break;
            case EGL_SAMPLES:
                query->samples = value;
                break;
            case EGL_STENCIL_SIZE:
                query->stencil_size = value;
                break;
            case EGL_SURFACE_TYPE:
                query->surface_type = value;
                break;
            case EGL_TRANSPARENT_TYPE:
                query->transparent_type = value;
                break;
            case EGL_TRANSPARENT_RED_VALUE:
                query->transparent_red_value = value;
                break;
            case EGL_TRANSPARENT_GREEN_VALUE:
                query->transparent_green_value = value;
                break;
            case EGL_TRANSPARENT_BLUE_VALUE:
                query->transparent_blue_value = value;
                break;
            case EGL_NATIVE_VISUAL_ID:
                query->native_visual_id = value;
                break;
            case EGL_MAX_PBUFFER_WIDTH:
                query->max_pbuffer_width = value;
                break;
            case EGL_MAX_PBUFFER_HEIGHT:
                query->max_pbuffer_height = value;
                break;
            case EGL_MAX_PBUFFER_PIXELS:
                query->max_pbuffer_pixels = value;
                break;
            default:
                return 0;
        }
    }
    return 1;
}

EGLBoolean EGLAPIENTRY eglChooseConfig (EGLDisplay dpy,
                                        const EGLint *attrib_list,
                                        EGLConfig *configs, EGLint config_size,
                                        EGLint *num_config)
{
    EGLConfigQuery query = default_query;
    EGLProxyConfigEntry *selected_configs = NULL;
    EGLint n_selected_configs = 0;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    if (num_config == NULL) {
        eglSetError (EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }

    if (fill_query (&query, attrib_list) == 0) {
        eglSetError (EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }
    selected_configs = (EGLProxyConfigEntry *) calloc ((size_t)
                       egl_display->n_configs,
                       sizeof (EGLProxyConfigEntry));
    n_selected_configs = select_config (egl_display, selected_configs, &query);
    if ((configs == NULL) || (n_selected_configs == 0)) {
        *num_config = n_selected_configs;
        free (selected_configs);
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
    }

    qsort (selected_configs, (size_t) n_selected_configs,
           sizeof (EGLProxyConfigEntry),
           config_comparator);

    for (*num_config = 0; ((*num_config < config_size) &&
                           (*num_config < n_selected_configs)); *num_config += 1) {
        configs[*num_config] = (EGLConfig) selected_configs[*num_config].config;
    }
    free (selected_configs);
    eglSetError (EGL_SUCCESS);
    return EGL_TRUE;
}

static int parse_context_attributes (ContextAttributes *attributes,
                                     const EGLint *attrib_list)
{
    size_t i = 0;
    if (attrib_list == NULL) {
        return 1;
    }
    for (i = 0; attrib_list[i] != EGL_NONE; i += 2) {
        EGLint value = attrib_list[i + 1];
        switch (attrib_list[i]) {
            case EGL_CONTEXT_MAJOR_VERSION:
                attributes->major_version = value;
                break;
            case EGL_CONTEXT_MINOR_VERSION:
                attributes->minor_version = value;
                break;
            case EGL_CONTEXT_OPENGL_PROFILE_MASK:
                if (value & ~ (EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT |
                               EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT)) {
                    return 0;
                }
                attributes->profile_mask = value;
                break;
            case EGL_CONTEXT_OPENGL_DEBUG:
                attributes->debug = value;
                break;
            case EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE:
                attributes->forward = value;
                break;
            case EGL_CONTEXT_OPENGL_ROBUST_ACCESS:
                attributes->robust_access = value;
                break;
            case EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY:
                attributes->reset_strategy = value;
                break;
            default:
                return 0;
        }
    }
    /* TODO: check that version is defined:
     * 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0, 2.1, 3.0, 3.1, 3.2, 4.0, 4.1,
     * 4.2, 4.3, 4.4, and any later versions of OpenGL released by Khronos */
    return 1;
}

EGLContext EGLAPIENTRY eglCreateContext (EGLDisplay dpy, EGLConfig config,
        EGLContext share_context,
        const EGLint *attrib_list)
{
    EGLProxyConfig *egl_config = NULL;
    EGLProxyContext *egl_context = NULL;
    ContextAttributes attributes = default_context_attributes;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_NO_CONTEXT;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_NO_CONTEXT;
    }
    CHECK_EGLCONFIG (dpy, config);
    egl_config = (EGLProxyConfig *)config;
    if (CurrentAPI == EGL_NONE) {
        eglSetError (EGL_BAD_MATCH);
        return EGL_NO_CONTEXT;
    }
    if (share_context != EGL_NO_CONTEXT) {
        /* TODO: generate EGL_BAD_CONTEXT if share_context not a of the same client
         * API type as the newly created context */
        /* TODO: make all shareable data, as defined by client API, to be
         * shared by share_context, all other contexts share_context already
         * shares with, and the newly created context */
        eglSetError (EGL_BAD_CONTEXT);
        return EGL_NO_CONTEXT;
    }
    /* TODO: generate EGL_BAD_MATCH error  if config does not support the
     * requested client API. This includes requesting creation of
     * an OpenGL ES 1.x, 2.0, or 3.0 context when the EGL_RENDERABLE_TYPE
     * attribute of config does not contain EGL_OPENGL_ES_BIT ,
     * EGL_OPENGL_ES2_BIT , or EGL_- OPENGL_ES3_BIT respectively. */
    if (parse_context_attributes (&attributes, attrib_list) == 0) {
        eglSetError (EGL_BAD_ATTRIBUTE);
        return EGL_NO_CONTEXT;
    }
    egl_context = (EGLProxyContext *) malloc (sizeof (EGLProxyContext));
    if (egl_context != NULL) {
        egl_context->platform = platform_create_context (egl_display->platform,
                                egl_config, &attributes);
        if (egl_context->platform != NULL) {
            egl_context->next = egl_display->contexts;
            egl_display->contexts = egl_context;
            eglSetError (EGL_SUCCESS);
            return (EGLContext) egl_context;
        }
        free (egl_context);
    }
    eglSetError (EGL_BAD_ALLOC);
    return EGL_NO_CONTEXT;
}

EGLSurface EGLAPIENTRY eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config,
        EGLNativeWindowType win,
        const EGLint *attrib_list)
{
    EGLProxySurface *egl_surface = NULL;
    EGLProxyConfig *egl_config = NULL;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_NO_SURFACE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_NO_SURFACE;
    }
    CHECK_EGLCONFIG (dpy, config);
    egl_config = (EGLProxyConfig *)config;
    if ((egl_config->surface_type & EGL_WINDOW_BIT) == 0) {
        eglSetError (EGL_BAD_MATCH);
        return EGL_NO_SURFACE;
    }
    UNUSED (attrib_list);
    /* TODO: parse attribute list */
    /* TODO: If the pixel format of native native window does not correspond to
     * the format, type, and size of the color buffers required by config, then
     * EGL_BAD_MATCH error is generated */
    /* TODO: if native_window is not a valid native window handle, then an
     * EGL_BAD_NATIVE_WINDOW error should be generated. */
    for (egl_surface = egl_display->surfaces; egl_surface != NULL;
            egl_surface = egl_surface->next) {
        if (egl_surface->window == (Window)win) {
            eglSetError (EGL_BAD_ALLOC);
            return EGL_NO_SURFACE;
        }
    }
    egl_surface = (EGLProxySurface *) malloc (sizeof (EGLProxySurface));
    if (egl_surface != NULL) {
        egl_surface->platform = platform_window_surface_create (egl_display->platform,
                                egl_config, win);
        if (egl_surface->platform) {
            egl_surface->next = egl_display->surfaces;
            egl_display->surfaces = egl_surface;
            egl_surface->window = (Window) win;
            eglSetError (EGL_SUCCESS);
            return (EGLSurface)egl_surface;
        }
        free (egl_surface);
    }
    eglSetError (EGL_BAD_ALLOC);
    return EGL_NO_SURFACE;
}

EGLBoolean EGLAPIENTRY eglDestroyContext (EGLDisplay dpy, EGLContext ctx)
{
    EGLProxyContext *item = NULL;
    EGLProxyContext *prev_item = NULL;
    EGLProxyContext *egl_context = NULL;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    egl_context = (EGLProxyContext *) ctx;
    for (item = egl_display->contexts; item != NULL;
            prev_item = item, item = item->next) {
        if (item == egl_context) {
            if (prev_item == NULL) {
                egl_display->contexts = item->next;
            } else {
                prev_item->next = item->next;
            }
            platform_context_destroy (egl_display->platform, item->platform);
            free (item);
            eglSetError (EGL_SUCCESS);
            return EGL_TRUE;
        }
    }
    eglSetError (EGL_BAD_CONTEXT);
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglDestroySurface (EGLDisplay dpy, EGLSurface surface)
{
    EGLProxySurface *item = NULL;
    EGLProxySurface *prev_item = NULL;
    EGLProxySurface *egl_surface = NULL;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    egl_surface = (EGLProxySurface *) surface;
    for (item = egl_display->surfaces; item != NULL;
            prev_item = item, item = item->next) {
        if (item == egl_surface) {
            if (prev_item == NULL) {
                egl_display->surfaces = item->next;
            } else {
                prev_item->next = item->next;
            }
            platform_window_surface_destroy (egl_display->platform, item->platform);
            free (item);
            eglSetError (EGL_SUCCESS);
            return EGL_TRUE;
        }
    }
    eglSetError (EGL_BAD_CONTEXT);
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config,
        EGLint attribute, EGLint *value)
{
    EGLProxyConfig *egl_config = NULL;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    CHECK_EGLCONFIG (dpy, config);
    egl_config = (EGLProxyConfig *)config;

    if (value == NULL) {
        eglSetError (EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }
    switch (attribute) {
        case EGL_BUFFER_SIZE:
            *value = egl_config->buffer_size;
            break;
        case EGL_RED_SIZE:
            *value = egl_config->red_size;
            break;
        case EGL_GREEN_SIZE:
            *value = egl_config->green_size;
            break;
        case EGL_BLUE_SIZE:
            *value = egl_config->blue_size;
            break;
        case EGL_LUMINANCE_SIZE:
            *value = egl_config->luminance_size;
            break;
        case EGL_ALPHA_SIZE:
            *value = egl_config->alpha_size;
            break;
        case EGL_ALPHA_MASK_SIZE:
            *value = egl_config->alpha_mask_size;
            break;
        case EGL_BIND_TO_TEXTURE_RGB:
            * ((EGLBoolean *)value) = egl_config->bind_to_texture_rgb;
            break;
        case EGL_BIND_TO_TEXTURE_RGBA:
            * ((EGLBoolean *)value) = egl_config->bind_to_texture_rgba;
            break;
        case EGL_COLOR_BUFFER_TYPE:
            *value = egl_config->color_buffer_type;
            break;
        case EGL_CONFIG_CAVEAT:
            *value = egl_config->config_caveat;
            break;
        case EGL_CONFIG_ID:
            *value = egl_config->config_id;
            break;
        case EGL_CONFORMANT:
            *value = egl_config->conformant;
            break;
        case EGL_DEPTH_SIZE:
            *value = egl_config->depth_size;
            break;
        case EGL_LEVEL:
            *value = egl_config->level;
            break;
        case EGL_MAX_PBUFFER_WIDTH:
            *value = egl_config->max_pbuffer_width;
            break;
        case EGL_MAX_PBUFFER_HEIGHT:
            *value = egl_config->max_pbuffer_height;
            break;
        case EGL_MAX_PBUFFER_PIXELS:
            *value = egl_config->max_pbuffer_pixels;
            break;
        case EGL_MAX_SWAP_INTERVAL:
            *value = egl_config->max_swap_interval;
            break;
        case EGL_MIN_SWAP_INTERVAL:
            *value = egl_config->min_swap_interval;
            break;
        case EGL_NATIVE_RENDERABLE:
            * ((EGLBoolean *)value) = egl_config->native_renderable;
            break;
        case EGL_NATIVE_VISUAL_ID:
            *value = egl_config->native_visual_id;
            break;
        case EGL_NATIVE_VISUAL_TYPE:
            *value = egl_config->native_visual_type;
            break;
        case EGL_RENDERABLE_TYPE:
            *value = egl_config->renderable_type;
            break;
        case EGL_SAMPLE_BUFFERS:
            *value = egl_config->sample_buffers;
            break;
        case EGL_SAMPLES:
            *value = egl_config->samples;
            break;
        case EGL_STENCIL_SIZE:
            *value = egl_config->stencil_size;
            break;
        case EGL_SURFACE_TYPE:
            *value = egl_config->surface_type;
            break;
        case EGL_TRANSPARENT_TYPE:
            *value = egl_config->transparent_type;
            break;
        case EGL_TRANSPARENT_RED_VALUE:
            *value = egl_config->transparent_red_value;
            break;
        case EGL_TRANSPARENT_GREEN_VALUE:
            *value = egl_config->transparent_green_value;
            break;
        case EGL_TRANSPARENT_BLUE_VALUE:
            *value = egl_config->transparent_blue_value;
            break;
        default:
            eglSetError (EGL_BAD_PARAMETER);
            return EGL_FALSE;
    }
    eglSetError (EGL_SUCCESS);
    return EGL_TRUE;
}

EGLDisplay EGLAPIENTRY eglGetDisplay (EGLNativeDisplayType display_id)
{
    EGLProxyDisplay *display = NULL;
    PlatformDisplay *platform = NULL;
    display = displays;
    while (display != NULL) {
        if (display->display_id == display_id) {
            eglSetError (EGL_SUCCESS);
            return (EGLDisplay) display;
        }
        display = display->next;
    }
    platform = platform_display_create (display_id);
    if (platform != NULL) {
        display = (EGLProxyDisplay *) calloc (1, sizeof (EGLProxyDisplay));
        if (display != NULL) {
            display->display_id = display_id;
            display->platform = platform;
            display->next = displays;
            displays = display;
            eglSetError (EGL_SUCCESS);
            return (EGLDisplay) display;
        }
    }
    platform_display_destroy (display_id, platform);
    return EGL_NO_DISPLAY;
}

EGLBoolean EGLAPIENTRY eglInitialize (EGLDisplay dpy, EGLint *major,
                                      EGLint *minor)
{
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized != EGL_TRUE) {
        egl_display->n_configs = platform_display_initialize (egl_display->platform,
                                 &egl_display->configs);
    }

    if (egl_display->n_configs > 0) {
        *major = 1;
        *minor = 5;
        egl_display->initialized = EGL_TRUE;
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
    }
    eglSetError (EGL_NOT_INITIALIZED);
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglMakeCurrent (EGLDisplay dpy, EGLSurface draw,
                                       EGLSurface read, EGLContext ctx)
{
    EGLProxySurface *egl_draw = (EGLProxySurface *) draw;
    EGLProxySurface *egl_read = (EGLProxySurface *) read;
    EGLProxyContext *egl_context = (EGLProxyContext *) ctx;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (ctx != EGL_NO_CONTEXT || draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE) {
        if (egl_display->initialized == EGL_FALSE) {
            eglSetError (EGL_NOT_INITIALIZED);
            return EGL_FALSE;
        }
    }
    if (ctx == EGL_NO_CONTEXT) {
        if (draw != EGL_NO_SURFACE || read != EGL_NO_SURFACE) {
            eglSetError (EGL_BAD_MATCH);
            return EGL_FALSE;
        }
    } else if (ctx != EGL_NO_CONTEXT) {
        if (draw != EGL_NO_SURFACE && read != EGL_NO_SURFACE) {
            /* TODO: Check context and surfaces */
        } else {
            eglSetError (EGL_BAD_MATCH);
            return EGL_FALSE;
        }
    }
    return platform_make_current (egl_display->platform, egl_draw, egl_read,
                                  egl_context);
}

const char *EGLAPIENTRY eglQueryString (EGLDisplay dpy, EGLint name)
{
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return NULL;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return NULL;
    }
    switch (name) {
        case EGL_CLIENT_APIS:
            return "OpenGL";
        case EGL_VENDOR:
            return "SOURYOGURT";
        case EGL_VERSION:
            return "1.4";
        case EGL_EXTENSIONS:
            return "";
        default:
            return NULL;
    }
    return NULL;
}

EGLBoolean EGLAPIENTRY eglSwapBuffers (EGLDisplay dpy, EGLSurface surface)
{
    EGLProxySurface *egl_surface = (EGLProxySurface *) surface;
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    return platform_swap_buffers (egl_display->platform, egl_surface);
}

EGLBoolean EGLAPIENTRY eglTerminate (EGLDisplay dpy)
{
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    while (egl_display->contexts != NULL) {
        EGLProxyContext *context = egl_display->contexts;
        egl_display->contexts = egl_display->contexts->next;
        platform_context_destroy (egl_display->platform, context->platform);
        free (context);
    }
    while (egl_display->surfaces != NULL) {
        EGLProxySurface *surface = egl_display->surfaces;
        egl_display->surfaces = egl_display->surfaces->next;
        platform_window_surface_destroy (egl_display->platform, surface->platform);
        free (surface);
    }
    free (egl_display->configs);
    egl_display->initialized = EGL_FALSE;
    eglSetError (EGL_SUCCESS);
    return EGL_TRUE;
}

EGLint EGLAPIENTRY eglGetError (void)
{
    return EGL_CONTEXT_LOST;
}

EGLBoolean EGLAPIENTRY eglGetConfigs (EGLDisplay dpy, EGLConfig *configs,
                                      EGLint config_size, EGLint *num_config)
{
    EGLProxyDisplay *egl_display = displays;
    while ((egl_display != NULL) && (egl_display != dpy)) {
        egl_display = egl_display->next;
    }
    if (egl_display == NULL) {
        eglSetError (EGL_BAD_DISPLAY);
        return EGL_FALSE;
    }
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_NOT_INITIALIZED);
        return EGL_FALSE;
    }
    if (num_config == NULL) {
        eglSetError (EGL_BAD_PARAMETER);
        return EGL_FALSE;
    }
    if (configs == NULL) {
        *num_config = egl_display->n_configs;
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
    }
    for (*num_config = 0; (*num_config < config_size)
            && (*num_config < egl_display->n_configs); *num_config += 1) {
        configs[*num_config] = (EGLConfig) &egl_display->configs[*num_config];
    }
    eglSetError (EGL_SUCCESS);
    return EGL_TRUE;
}
