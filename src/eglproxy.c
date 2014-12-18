#include "eglproxy.h"
#include <stdlib.h>

typedef struct EGLProxyConfigEntry {
    EGLProxyConfig *config;
    EGLint caveat;
    EGLint buffer_type;
    EGLint n_colorbits;
} EGLProxyConfigEntry;

static const EGLProxyConfig default_query = {
    0, /* EGL_BUFFER_SIZE */
    0, /* EGL_ALPHA_SIZE */
    0, /* EGL_BLUE_SIZE */
    0, /* EGL_GREEN_SIZE */
    0, /* EGL_RED_SIZE */
    0, /* EGL_DEPTH_SIZE */
    0, /* EGL_STENCIL_SIZE */
    EGL_DONT_CARE, /* EGL_CONFIG_CAVEAT */
    EGL_DONT_CARE, /* EGL_CONFIG_ID */
    0, /* EGL_LEVEL */
    EGL_DONT_CARE, /* EGL_MAX_PBUFFER_HEIGHT */
    EGL_DONT_CARE, /* EGL_MAX_PBUFFER_PIXELS */
    EGL_DONT_CARE, /* EGL_MAX_PBUFFER_WIDTH */
    EGL_DONT_CARE, /* EGL_NATIVE_RENDERABLE */
    EGL_DONT_CARE, /* EGL_NATIVE_VISUAL_ID */
    EGL_DONT_CARE, /* EGL_NATIVE_VISUAL_TYPE */
    0, /* RESERVED */
    0, /* EGL_SAMPLES */
    0, /* EGL_SAMPLE_BUFFERS */
    EGL_WINDOW_BIT, /* EGL_SURFACE_TYPE */
    EGL_NONE, /* EGL_TRANSPARENT_TYPE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_BLUE_VALUE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_GREEN_VALUE */
    EGL_DONT_CARE, /* EGL_TRANSPARENT_RED_VALUE */
    0, /* EGL_NONE */
    EGL_DONT_CARE, /* EGL_BIND_TO_TEXTURE_RGB */
    EGL_DONT_CARE, /* EGL_BIND_TO_TEXTURE_RGBA */
    EGL_DONT_CARE, /* EGL_MIN_SWAP_INTERVAL */
    EGL_DONT_CARE, /* EGL_MAX_SWAP_INTERVAL */
    0, /* EGL_LUMINANCE_SIZE */
    0, /* EGL_ALPHA_MASK_SIZE */
    EGL_RGB_BUFFER, /* EGL_COLOR_BUFFER_TYPE */
    EGL_OPENGL_ES_BIT, /* EGL_RENDERABLE_TYPE */
    EGL_NONE, /* EGL_MATCH_NATIVE_PIXMAP */
    0, /* EGL_CONFORMANT */
    0, /* EGL_MATCH_FORMAT_KHR */
    NULL /* platform */
};

static const ContextAttributes default_context_attributes = {
    1, /* EGL_CONTEXT_MAJOR_VERSION */
    0, /* EGL_CONTEXT_MINOR_VERSION */
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, /* EGL_CONTEXT_OPENGL_PROFILE_MASK */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_DEBUG */
    EGL_FALSE, /* EGL_CONTEXT_FORWARD_COMPATIBLE */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_ROBUST_ACCESS */
    EGL_NO_RESET_NOTIFICATION /* EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY */
};

static EGLProxyDisplay *displays = NULL;

static EGLenum CurrentAPI = EGL_NONE; /*TODO: Should be in TLS */

static EGLint last_error = EGL_SUCCESS; /*TODO: Should be in TLS */

#define CHECK_EGLCONFIG(dpy, config) { \
if (((EGLProxyConfig*)(config) < ((EGLProxyDisplay*)(dpy))->configs) || \
    ((EGLProxyConfig*)(config) >= &((EGLProxyDisplay*)(dpy))->configs[((EGLProxyDisplay*)(dpy))->n_configs])){ \
    eglSetError (EGL_BAD_CONFIG); \
    return EGL_FALSE; \
} \
}while(0)

#define UNUSED(x) (void)(x)

void eglSetError (EGLint error)
{
    last_error = error;
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
                             const EGLProxyConfig *query)
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
                && (cfg->bind_to_texture_rgb != query->bind_to_texture_rgb)) {
            continue;
        }
        if ((query->bind_to_texture_rgba != EGL_DONT_CARE)
                && (cfg->bind_to_texture_rgba != query->bind_to_texture_rgba)) {
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
                && (cfg->native_renderable != query->native_renderable)) {
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

static EGLBoolean fill_query (EGLProxyConfig *query, const EGLint *attrib_list)
{
    size_t i = 0;
    if (attrib_list == NULL) {
        return EGL_TRUE;
    }
    for (i = 0; attrib_list[i] != EGL_NONE; i += 2) {
        EGLint attribute = attrib_list[i];
        EGLint value = attrib_list[i + 1];
        if ((attribute >= EGL_BUFFER_SIZE) && (attribute <= EGL_MATCH_FORMAT_KHR)
                && (attribute != 0x3030)) {
            if (((attribute == EGL_LEVEL) || (attribute == EGL_MATCH_NATIVE_PIXMAP))
                    && value == EGL_DONT_CARE) {
                return EGL_FALSE;
            }
            ((EGLint *)query)[attribute - EGL_BUFFER_SIZE] = value;
        } else {
            return EGL_FALSE;
        }
    }
    return EGL_TRUE;
}

EGLBoolean EGLAPIENTRY eglChooseConfig (EGLDisplay dpy,
                                        const EGLint *attrib_list,
                                        EGLConfig *configs, EGLint config_size,
                                        EGLint *num_config)
{
    EGLProxyConfig query = default_query;
    EGLProxyConfigEntry *selected_configs = NULL;
    EGLint n_selected_configs = 0;
    EGLint i = 0;
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

    if (fill_query (&query, attrib_list) == EGL_FALSE) {
        eglSetError (EGL_BAD_ATTRIBUTE);
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
    *num_config = (config_size < n_selected_configs) ? config_size :
                  n_selected_configs;
    for (i = 0; i < *num_config; i++) {
        configs[i] = (EGLConfig) selected_configs[i].config;
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

EGLSurface EGLAPIENTRY eglCreatePlatformWindowSurface (EGLDisplay dpy,
        EGLConfig config, void *native_window, const EGLAttrib *attrib_list)
{
    return eglCreateWindowSurface (dpy, config, (EGLNativeWindowType)native_window,
                                   (const EGLint *)attrib_list);
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
    /* TODO: if native_window is not a valid native window handle, then an
     * EGL_BAD_NATIVE_WINDOW error should be generated. */
    for (egl_surface = egl_display->surfaces; egl_surface != NULL;
            egl_surface = egl_surface->next) {
        if (egl_surface->window == win) {
            eglSetError (EGL_BAD_ALLOC);
            return EGL_NO_SURFACE;
        }
    }
    if (!window_is_match_config (egl_display->platform, win, egl_config)) {
        eglSetError (EGL_BAD_MATCH);
        return EGL_NO_SURFACE;
    }
    egl_surface = (EGLProxySurface *) malloc (sizeof (EGLProxySurface));
    if (egl_surface != NULL) {
        egl_surface->platform = platform_window_surface_create (egl_display->platform,
                                egl_config, win);
        if (egl_surface->platform) {
            egl_surface->next = egl_display->surfaces;
            egl_display->surfaces = egl_surface;
            egl_surface->window = win;
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
    if ((attribute >= EGL_BUFFER_SIZE) && (attribute <= EGL_MATCH_FORMAT_KHR)
            && (attribute != EGL_NONE) && (attribute != 0x3030)
            && (attribute != EGL_MATCH_NATIVE_PIXMAP)) {
        *value = ((EGLint *)egl_config)[attribute - EGL_BUFFER_SIZE];
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
    }
    eglSetError (EGL_BAD_ATTRIBUTE);
    return EGL_FALSE;
}

EGLDisplay EGLAPIENTRY eglGetPlatformDisplay (EGLenum platform,
        void *native_display, const EGLAttrib *attrib_list)
{
    EGLProxyDisplay *display = displays;
    PlatformDisplayAttributes *display_attributes =
        platform_display_attributes_create (platform, native_display, attrib_list);
    if (display_attributes == NULL) {
        return EGL_NO_DISPLAY;
    }

    while (display != NULL) {
        if (platform_display_has_attributes (display, display_attributes)) {
            free (display_attributes);
            eglSetError (EGL_SUCCESS);
            return (EGLDisplay) display;
        }
        display = display->next;
    }
    display = (EGLProxyDisplay *) calloc (1, sizeof (EGLProxyDisplay));
    if (display != NULL) {
        display->attributes = display_attributes;
        display->display_id = (EGLNativeDisplayType)native_display;
        display->next = displays;
        displays = display;
        eglSetError (EGL_SUCCESS);
        return (EGLDisplay) display;
    }
    free (display_attributes);
    return EGL_NO_DISPLAY;
}

EGLDisplay EGLAPIENTRY eglGetDisplay (EGLNativeDisplayType display_id)
{
    return eglGetPlatformDisplay (EGL_PLATFORM_X11_KHR, display_id, NULL);
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
    if (egl_display->initialized == EGL_FALSE) {
        egl_display->platform = platform_display_create (egl_display->attributes);
        if (egl_display->platform != NULL) {
            egl_display->n_configs = platform_display_initialize (
                                         egl_display->platform,
                                         &egl_display->configs);
            if (egl_display->n_configs > 0) {
                egl_display->initialized = EGL_TRUE;
            } else {
                platform_display_destroy (egl_display->platform,
                                          egl_display->display_id);
            }
        }
    }

    if (egl_display->initialized) {
        if ((major != NULL) && (minor != NULL)) {
            *major = 1;
            *minor = 5;
        }
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
    if (dpy != EGL_NO_DISPLAY) {
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
    } else {
        if (name != EGL_EXTENSIONS && name != EGL_VERSION) {
            eglSetError (EGL_BAD_DISPLAY);
            return NULL;
        }
    }
    eglSetError (EGL_SUCCESS);
    switch (name) {
        case EGL_CLIENT_APIS:
            return "OpenGL";
        case EGL_VENDOR:
            return "SOURYOGURT";
        case EGL_VERSION:
            return "1.5";
        case EGL_EXTENSIONS:
            return "";
        default:
            eglSetError (EGL_BAD_PARAMETER);
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
    if (egl_display->initialized == EGL_FALSE) {
        eglSetError (EGL_SUCCESS);
        return EGL_TRUE;
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
    platform_display_destroy (egl_display->platform, egl_display->display_id);
    egl_display->initialized = EGL_FALSE;
    eglSetError (EGL_SUCCESS);
    return EGL_TRUE;
}

EGLint EGLAPIENTRY eglGetError (void)
{
    return last_error;
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
