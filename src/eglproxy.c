#include "eglproxy.h"
#include <stdlib.h>
#include <string.h>

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

typedef struct EGLProxyFunction {
    const char *const name;
    __eglMustCastToProperFunctionPointerType address;
} EGLProxyFunction;

enum {
    TOTAL_KEYWORDS = 91,
    MIN_WORD_LENGTH = 9,
    MAX_WORD_LENGTH = 37,
    MIN_HASH_VALUE = 9,
    MAX_HASH_VALUE = 166
};

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

static const ContextAttributes default_context_attributes = {
    1, /* EGL_CONTEXT_MAJOR_VERSION */
    0, /* EGL_CONTEXT_MINOR_VERSION */
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, /* EGL_CONTEXT_OPENGL_PROFILE_MASK */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_DEBUG */
    EGL_FALSE, /* EGL_CONTEXT_FORWARD_COMPATIBLE */
    EGL_FALSE, /* EGL_CONTEXT_OPENGL_ROBUST_ACCESS */
    EGL_NO_RESET_NOTIFICATION /* EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY */
};

static const struct EGLProxyFunction lookup_table[] = {
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglWaitGL", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglGetError", (__eglMustCastToProperFunctionPointerType)eglGetError},
    {"", NULL},
    {"eglGetDisplay", (__eglMustCastToProperFunctionPointerType)eglGetDisplay},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglCreateSync", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglMakeCurrent", (__eglMustCastToProperFunctionPointerType)eglMakeCurrent},
    {"eglSignalSyncNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryAPI", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetProcAddress", (__eglMustCastToProperFunctionPointerType)eglGetProcAddress},
    {"eglWaitNative", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglGetCurrentContext", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglSignalSyncKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateWindowSurface", (__eglMustCastToProperFunctionPointerType)eglCreateWindowSurface},
    {"", NULL},
    {"eglQueryString", (__eglMustCastToProperFunctionPointerType)eglQueryString},
    {"eglQueryContext", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateSyncKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePixmapSurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetSystemTimeNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePixmapSurfaceHI", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglChooseConfig", (__eglMustCastToProperFunctionPointerType)eglChooseConfig},
    {"eglGetPlatformDisplay", (__eglMustCastToProperFunctionPointerType)eglGetPlatformDisplay},
    {"eglGetSystemTimeFrequencyNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateStreamProducerSurfaceKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetPlatformDisplayEXT", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePlatformWindowSurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateStreamFromFileDescriptorKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglStreamConsumerGLTextureExternalKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePlatformWindowSurfaceEXT", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglUnlockSurfaceKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePlatformPixmapSurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateContext", (__eglMustCastToProperFunctionPointerType)eglCreateContext},
    {"eglCreatePbufferFromClientBuffer", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePlatformPixmapSurfaceEXT", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglDestroySync", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglFenceNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglWaitSync", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglClientWaitSync", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateStreamKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglWaitSyncKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglDestroySyncNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglDestroyContext", (__eglMustCastToProperFunctionPointerType)eglDestroyContext},
    {"eglReleaseTexImage", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglClientWaitSyncNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateFenceSyncNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglStreamConsumerReleaseKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetConfigs", (__eglMustCastToProperFunctionPointerType)eglGetConfigs},
    {"eglSwapBuffers", (__eglMustCastToProperFunctionPointerType)eglSwapBuffers},
    {"eglQuerySurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateStreamSyncNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryNativePixmapNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryNativeDisplayNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCopyBuffers", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglSwapInterval", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreateDRMImageMESA", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglStreamConsumerAcquireKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglWaitClient", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglBindTexImage", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetSyncAttrib", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryStreamKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglCreatePbufferSurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL},
    {"eglSurfaceAttrib", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglSwapBuffersWithDamageEXT", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetSyncAttribNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetStreamFileDescriptorKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglBindAPI", (__eglMustCastToProperFunctionPointerType)eglBindAPI},
    {"eglReleaseThread", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglDestroySurface", (__eglMustCastToProperFunctionPointerType)eglDestroySurface},
    {"eglDestroyImageKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetSyncAttribKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglExportDRMImageMESA", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryNativeWindowNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglPostSubBufferNV", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglGetCurrentSurface", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglQueryStreamTimeKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglDestroySyncKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglInitialize", (__eglMustCastToProperFunctionPointerType)eglInitialize},
    {"eglDestroyStreamKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglClientWaitSyncKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglTerminate", (__eglMustCastToProperFunctionPointerType)eglTerminate},
    {"eglCreateSync64KHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL},
    {"eglCreateImageKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglLockSurfaceKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL},
    {"eglQuerySurface64KHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglQueryStreamu64KHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglGetCurrentDisplay", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglSetBlobCacheFuncsANDROID", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglQuerySurfacePointerANGLE", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglGetConfigAttrib", (__eglMustCastToProperFunctionPointerType)eglGetConfigAttrib},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglStreamAttribKHR", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL}, {"", NULL},
    {"eglSwapBuffersRegionNOK", (__eglMustCastToProperFunctionPointerType)NULL},
    {"eglSwapBuffersRegion2NOK", (__eglMustCastToProperFunctionPointerType)NULL},
    {"", NULL},
    {"eglDupNativeFenceFDANDROID", (__eglMustCastToProperFunctionPointerType)NULL}
};

static EGLProxyDisplay *displays = NULL;

static EGLenum CurrentAPI = EGL_NONE; /*TODO: Should be in TLS */

static EGLint last_error = EGL_SUCCESS; /*TODO: Should be in TLS */

#define CHECK_EGLCONFIG(dpy, config) { \
if (((EGLProxyConfig*)(config) < ((EGLProxyDisplay*)(dpy))->configs) || \
    ((EGLProxyConfig*)(config) >= &((EGLProxyDisplay*)(dpy))->configs[((EGLProxyDisplay*)(dpy))->n_configs])){ \
    eglSetError (EGL_BAD_PARAMETER); \
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
        if (egl_surface->window == win) {
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

static unsigned int hash (register const char *str, register unsigned int len)
{
    static const unsigned char asso_values[] = {
        167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
        167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
        167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
        167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
        167, 167, 167, 167, 167, 167, 167, 167, 167, 167,
        167, 167,  50, 167, 167, 167, 167, 167, 167, 167,
        167, 167, 167, 167, 167,  40,  35,   5,   5,  25,
        35,   0,  10,  40, 167,  45,  50,   5,   5, 167,
        5,   5,  30,   0,  55,  15,   5,   0, 167, 167,
        167, 167, 167, 167, 167, 167, 167,  35,  60,   0,
        30,   5,  50,  10,   5,  60, 167, 167,  45,   0,
        40,   0,   0, 167,   0, 167,  10,  40, 167,   0,
        20,  30, 167, 167, 167, 167, 167, 167
    };
    register unsigned int hval = len;

    switch (hval) {
        default:
            hval += asso_values[ (unsigned char)str[17]];
        /*FALLTHROUGH*/
        case 17:
        case 16:
        case 15:
            hval += asso_values[ (unsigned char)str[14]];
        /*FALLTHROUGH*/
        case 14:
        case 13:
        case 12:
        case 11:
        case 10:
            hval += asso_values[ (unsigned char)str[9]];
        /*FALLTHROUGH*/
        case 9:
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
            hval += asso_values[ (unsigned char)str[3]];
            break;
    }
    return hval;
}

__eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress (
    const char *procname)
{
    size_t len = strlen (procname);
    if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH) {
        register unsigned int key = hash (procname, (unsigned int)len);

        if (key <= MAX_HASH_VALUE) {
            register const char *s = lookup_table[key].name;

            if (*procname == *s && !strcmp (procname + 1, s + 1)) {
                return lookup_table[key].address;
            }
        }
    }
    return platform_get_proc_address (procname);
}
