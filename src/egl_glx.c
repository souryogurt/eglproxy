/**
 * @file egl_glx.c
 * Implementation of EGL using GLX.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "eglproxy.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glx.h>

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

struct PlatformDisplay {
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
};

struct PlatformDisplayAttributes {
    EGLNativeDisplayType native_display;
    int screen;
};

void *platform_create_context (PlatformDisplay *display,
                               EGLProxyConfig *egl_config,
                               ContextAttributes *attributes)
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

void platform_context_destroy (PlatformDisplay *display, void *context)
{
    glXDestroyContext (display->x11_display, (GLXContext)context);
}

void *platform_window_surface_create (PlatformDisplay *display,
                                      EGLProxyConfig *egl_config, EGLNativeWindowType win)
{
    GLXDrawable result = win;
    if (display->is_modern) {
        GLXFBConfig glx_config = (GLXFBConfig) egl_config->platform;
        result = glXCreateWindow (display->x11_display, glx_config, win, NULL);
    }
    return (void *) result;
}

void platform_window_surface_destroy (PlatformDisplay *display, void *drawable)
{
    if (display->is_modern) {
        glXDestroyWindow (display->x11_display, (GLXDrawable) drawable);
    }
}

PlatformDisplayAttributes *platform_display_attributes_create (EGLenum platform,
        void *native_display, const EGLAttrib *attrib_list)
{
    int screen = 0;
    PlatformDisplayAttributes *attributes = NULL;
    if (platform != EGL_PLATFORM_X11_KHR) {
        eglSetError (EGL_BAD_PARAMETER);
        return NULL;
    }
    if (attrib_list != NULL) {
        size_t i = 0;
        for (i = 0; attrib_list[i] != EGL_NONE; i += 2) {
            EGLAttrib value = attrib_list[i + 1];
            switch (attrib_list[i]) {
                case EGL_PLATFORM_X11_SCREEN_KHR:
                    screen = (int) value;
                    break;
                default:
                    eglSetError (EGL_BAD_ATTRIBUTE);
                    return NULL;
            }
        }
    }
    attributes = (PlatformDisplayAttributes *)calloc (1,
                 sizeof (PlatformDisplayAttributes));
    if (attributes != NULL) {
        attributes->native_display = (EGLNativeDisplayType) native_display;
        attributes->screen = screen;
        return attributes;
    }
    return NULL;
}

int platform_display_has_attributes (const EGLProxyDisplay *display,
                                     const PlatformDisplayAttributes *attributes)
{
    return (display->display_id == attributes->native_display) &&
           (display->platform->screen == attributes->screen);
}

PlatformDisplay *platform_display_create (const PlatformDisplayAttributes
        *attributes)
{
    Display *x11_display = attributes->native_display;
    int glx_major = 0;
    int glx_minor = 0;
    if (x11_display == EGL_DEFAULT_DISPLAY) {
        x11_display = XOpenDisplay ((char *)NULL);
        if (x11_display == NULL) {
            return NULL;
        }
    }
    if (glXQueryVersion (x11_display, &glx_major, &glx_minor) == True) {
        if ((glx_major > 1) || ((glx_major == 1) && (glx_minor >= 2))) {
            PlatformDisplay *display = (PlatformDisplay *) calloc (1,
                                       sizeof (PlatformDisplay));
            if (display != NULL) {
                display->x11_display = x11_display;
                display->screen = attributes->screen;
                display->glx_major = glx_major;
                display->glx_minor = glx_minor;
                return display;
            }
        }
    }
    return NULL;
}

void platform_display_destroy (PlatformDisplay *display,
                               EGLNativeDisplayType id)
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

EGLint platform_display_initialize (PlatformDisplay *display,
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
    }
    return glx_populate_from_visualinfos (display, config_list);
}

EGLBoolean platform_make_current (PlatformDisplay *display,
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

EGLBoolean platform_swap_buffers (PlatformDisplay *display,
                                  EGLProxySurface *surface)
{
    glXSwapBuffers (display->x11_display, (GLXDrawable) surface->platform);
    return EGL_TRUE;
}
