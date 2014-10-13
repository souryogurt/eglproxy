/**
 * @file egl_wgl.c
 * Implementation of EGL using WGL.
 */
#include "eglproxy.h"
#include <windows.h>
#include <stdlib.h>

#ifndef WGL_ARB_extensions_string
#define WGL_ARB_extensions_string 1
typedef const char * (WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);
#endif /* WGL_ARB_extensions_string */
#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context 1
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB       0x2093
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define ERROR_INVALID_VERSION_ARB         0x2095
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC,
        HGLRC hShareContext, const int *attribList);
#endif /* WGL_ARB_create_context */

#ifndef WGL_ARB_create_context_profile
#define WGL_ARB_create_context_profile 1
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define ERROR_INVALID_PROFILE_ARB         0x2096
#endif /* WGL_ARB_create_context_profile */
#ifndef WGL_ARB_pixel_format
#define WGL_ARB_pixel_format 1
#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_DRAW_TO_BITMAP_ARB            0x2002
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_NEED_PALETTE_ARB              0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB       0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB        0x2006
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_NUMBER_OVERLAYS_ARB           0x2008
#define WGL_NUMBER_UNDERLAYS_ARB          0x2009
#define WGL_TRANSPARENT_ARB               0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB     0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB   0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB    0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB   0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB   0x203B
#define WGL_SHARE_DEPTH_ARB               0x200C
#define WGL_SHARE_STENCIL_ARB             0x200D
#define WGL_SHARE_ACCUM_ARB               0x200E
#define WGL_SUPPORT_GDI_ARB               0x200F
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_STEREO_ARB                    0x2012
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_RED_BITS_ARB                  0x2015
#define WGL_RED_SHIFT_ARB                 0x2016
#define WGL_GREEN_BITS_ARB                0x2017
#define WGL_GREEN_SHIFT_ARB               0x2018
#define WGL_BLUE_BITS_ARB                 0x2019
#define WGL_BLUE_SHIFT_ARB                0x201A
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_ALPHA_SHIFT_ARB               0x201C
#define WGL_ACCUM_BITS_ARB                0x201D
#define WGL_ACCUM_RED_BITS_ARB            0x201E
#define WGL_ACCUM_GREEN_BITS_ARB          0x201F
#define WGL_ACCUM_BLUE_BITS_ARB           0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB          0x2021
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_STENCIL_BITS_ARB              0x2023
#define WGL_AUX_BUFFERS_ARB               0x2024
#define WGL_NO_ACCELERATION_ARB           0x2025
#define WGL_GENERIC_ACCELERATION_ARB      0x2026
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_COPY_ARB                 0x2029
#define WGL_SWAP_UNDEFINED_ARB            0x202A
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_TYPE_COLORINDEX_ARB           0x202C
typedef BOOL (WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc,
        int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes,
        int *piValues);
typedef BOOL (WINAPI *PFNWGLGETPIXELFORMATATTRIBFVARBPROC) (HDC hdc,
        int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes,
        FLOAT *pfValues);
#endif /* WGL_ARB_pixel_format */

struct PlatformDisplay {
    HDC hdc;
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
    PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB;
    int is_arb_context_profile; /**< Is WGL_ARB_create_context_profile there*/
};

struct PlatformDisplayAttributes {
    EGLNativeDisplayType native_display;
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
    PlatformDisplay *display = NULL;
    HDC hdcDisplay = attributes->native_display;
    if (hdcDisplay == EGL_DEFAULT_DISPLAY) {
        hdcDisplay = GetDC (NULL);
        if (hdcDisplay == NULL) {
            return NULL;
        }
    }
    display = (PlatformDisplay *) calloc (1, sizeof (PlatformDisplay));
    if (display != NULL) {
        display->hdc = hdcDisplay;
        return display;
    }
    return NULL;
}

void platform_display_destroy (PlatformDisplay *display,
                               EGLNativeDisplayType id)
{
    if (id == EGL_DEFAULT_DISPLAY) {
        ReleaseDC (NULL, display->hdc);
    }
    free (display);
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

static EGLint wgl_populate_from_arb_pixel_format (PlatformDisplay *display,
        EGLProxyConfig **config_list)
{
    return 0;
}

static EGLint wgl_populate_default (PlatformDisplay *display,
                                    EGLProxyConfig **config_list)
{
    int max_pixel_format = 0;
    int pixel_format = 1;
    EGLint n_configs = 0;
    EGLProxyConfig *list = NULL;
    EGLProxyConfig *egl_config = NULL;

    max_pixel_format = DescribePixelFormat (display->hdc, pixel_format,
                                            sizeof (PIXELFORMATDESCRIPTOR), NULL);
    list = (EGLProxyConfig *) calloc ((size_t)max_pixel_format,
                                      sizeof (EGLProxyConfig));
    if (list == NULL) {
        return 0;
    }
    *config_list = list;
    for (pixel_format = 1; pixel_format <= max_pixel_format; pixel_format++) {
        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat (display->hdc, pixel_format,
                             sizeof (PIXELFORMATDESCRIPTOR), &pfd);
        egl_config = list;
        if (pfd.iPixelType != PFD_TYPE_RGBA) {
            continue;
        }
        if (! (pfd.dwFlags & PFD_SUPPORT_OPENGL)) {
            continue;
        }
        egl_config->config_id = n_configs + 1;
        egl_config->buffer_size = pfd.cColorBits;
        egl_config->red_size = pfd.cRedBits;
        egl_config->green_size = pfd.cGreenBits;
        egl_config->blue_size = pfd.cBlueBits;
        egl_config->luminance_size = 0;
        egl_config->alpha_size = pfd.cAlphaBits;
        egl_config->alpha_mask_size = 0;
        egl_config->bind_to_texture_rgb = 0;
        egl_config->bind_to_texture_rgba = 0;
        egl_config->double_buffer = (pfd.dwFlags & PFD_DOUBLEBUFFER) ? EGL_TRUE :
                                    EGL_FALSE;
        egl_config->color_buffer_type = EGL_RGB_BUFFER;
        /*TODO: CHECK THIS FLAGS!!!!! */
        egl_config->config_caveat = (pfd.dwFlags & PFD_GENERIC_ACCELERATED) ? EGL_NONE :
                                    EGL_SLOW_CONFIG;
        egl_config->conformant = EGL_OPENGL_BIT;
        egl_config->renderable_type = EGL_OPENGL_BIT;
        egl_config->depth_size = pfd.cDepthBits;
        egl_config->level = 0;
        egl_config->max_pbuffer_width = 0;
        egl_config->max_pbuffer_height = 0;
        egl_config->max_pbuffer_pixels = 0;
        egl_config->max_swap_interval = 1;
        egl_config->min_swap_interval = 1;
        egl_config->native_renderable = (pfd.dwFlags & PFD_SUPPORT_GDI) ? EGL_TRUE :
                                        EGL_FALSE;
        egl_config->native_visual_id = (EGLint) pixel_format;
        egl_config->native_visual_type = 0;
        /* TODO: implement using ARB_Multisample */
        egl_config->sample_buffers = 0;
        egl_config->samples = 0;
        egl_config->stencil_size = pfd.cStencilBits;
        egl_config->surface_type = 0;
        egl_config->surface_type |= (pfd.dwFlags & PFD_DRAW_TO_WINDOW) ? EGL_WINDOW_BIT
                                    : 0;
        egl_config->surface_type |= (pfd.dwFlags & PFD_DRAW_TO_BITMAP) ? EGL_PIXMAP_BIT
                                    : 0;
        egl_config->transparent_type = EGL_NONE;
        n_configs++;
        list++;
    }
    return n_configs;
}

EGLint platform_display_initialize (PlatformDisplay *display,
                                    EGLProxyConfig **config_list)
{
    /*Create Fake window class */
    WNDCLASSEX wc;
    HWND fake_window = NULL;
    HDC hDC = NULL;
    int iPixelFormat = 0;
    HGLRC hRCFake = NULL;
    const char *extensions = NULL;
    int is_arb_pixel_format = 0;
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    HINSTANCE hInstance = GetModuleHandle (NULL);
    memset (&wc, 0, sizeof (wc));
    wc.cbSize = sizeof (WNDCLASSEX);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "MyFakeOpenGLWindowClass";
    wc.style = CS_OWNDC;
    if ( RegisterClassEx (&wc) == 0 ) {
        return 0;
    }

    /* Creating fake context */
    fake_window = CreateWindowEx (0, wc.lpszClassName, "OpenGL Window",
                                  WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0, 0, hInstance, 0);

    hDC = GetDC (fake_window);



    iPixelFormat = ChoosePixelFormat (hDC, &pfd);
    if (iPixelFormat == 0) {
        ReleaseDC (fake_window, hDC);
        DestroyWindow (fake_window);
        UnregisterClass ("MyFakeOpenGLWindowClass", hInstance);
        return 0;
    }

    if (!SetPixelFormat (hDC, iPixelFormat, &pfd)) {
        ReleaseDC (fake_window, hDC);
        DestroyWindow (fake_window);
        UnregisterClass ("MyFakeOpenGLWindowClass", hInstance);
        return 0;
    }

    hRCFake = wglCreateContext (hDC);
    if (hRCFake == NULL) {
        ReleaseDC (fake_window, hDC);
        DestroyWindow (fake_window);
        UnregisterClass ("MyFakeOpenGLWindowClass", hInstance);
        return 0;
    }
    wglMakeCurrent (hDC, hRCFake);
    display->wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
                                         wglGetProcAddress ("wglGetExtensionsStringARB");

    if (display->wglGetExtensionsStringARB == NULL) {
        wglMakeCurrent (NULL, NULL);
        wglDeleteContext (hRCFake);
        ReleaseDC (fake_window, hDC);
        DestroyWindow (fake_window);
        UnregisterClass ("MyFakeOpenGLWindowClass", hInstance);
        return 0;
    }

    extensions = display->wglGetExtensionsStringARB (hDC);
    if (is_extension_supported (extensions, "WGL_ARB_create_context")) {
        display->wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
                                              wglGetProcAddress ("wglCreateContextAttribsARB");
        display->is_arb_context_profile = is_extension_supported (extensions,
                                          "WGL_ARB_create_context_profile");
    }

    if (is_extension_supported (extensions, "WGL_ARB_pixel_format")) {
        display->wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
                                                wglGetProcAddress ("wglGetPixelFormatAttribivARB");
        display->wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)
                                                wglGetProcAddress ("wglGetPixelFormatAttribfvARB");
        is_arb_pixel_format = (display->wglGetPixelFormatAttribivARB != NULL) &&
                              (display->wglGetPixelFormatAttribfvARB != NULL);
    }
    is_arb_pixel_format = 0;
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRCFake);
    ReleaseDC (fake_window, hDC);
    DestroyWindow (fake_window);
    UnregisterClass ("MyFakeOpenGLWindowClass", hInstance);
    if (is_arb_pixel_format) {
        return wgl_populate_from_arb_pixel_format (display, config_list);
    }
    return wgl_populate_default (display, config_list);
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
    PlatformDisplayAttributes *attributes = NULL;
    attributes = (PlatformDisplayAttributes *)calloc (1,
                 sizeof (PlatformDisplayAttributes));
    if (attributes != NULL) {
        attributes->native_display = (EGLNativeDisplayType) native_display;
        return attributes;
    }
    return NULL;
}

int platform_display_has_attributes (const EGLProxyDisplay *display,
                                     const PlatformDisplayAttributes *attributes)
{
    return (display->display_id == attributes->native_display);
}
