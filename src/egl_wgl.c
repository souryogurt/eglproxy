#define EGLAPI __declspec(dllexport)
#include <EGL/egl.h>

EGLBoolean EGLAPIENTRY eglBindAPI (EGLenum api)
{
    switch (api) {
        case EGL_OPENGL_API:
            return EGL_FALSE;
        default:
            return EGL_FALSE;
    }
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglChooseConfig (EGLDisplay dpy,
                                        const EGLint *attrib_list,
                                        EGLConfig *configs, EGLint config_size,
                                        EGLint *num_config)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLContext EGLAPIENTRY eglCreateContext (EGLDisplay dpy, EGLConfig config,
        EGLContext share_context,
        const EGLint *attrib_list)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_NO_CONTEXT;
}

EGLSurface EGLAPIENTRY eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config,
        EGLNativeWindowType win,
        const EGLint *attrib_list)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_NO_SURFACE;
}

EGLBoolean EGLAPIENTRY eglDestroyContext (EGLDisplay dpy, EGLContext ctx)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglDestroySurface (EGLDisplay dpy, EGLSurface surface)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config,
        EGLint attribute, EGLint *value)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLDisplay EGLAPIENTRY eglGetDisplay (EGLNativeDisplayType display_id)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglInitialize (EGLDisplay dpy, EGLint *major,
                                      EGLint *minor)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglMakeCurrent (EGLDisplay dpy, EGLSurface draw,
                                       EGLSurface read, EGLContext ctx)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

const char *EGLAPIENTRY eglQueryString (EGLDisplay dpy, EGLint name)
{
    return "";
}

EGLBoolean EGLAPIENTRY eglSwapBuffers (EGLDisplay dpy, EGLSurface surface)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLBoolean EGLAPIENTRY eglTerminate (EGLDisplay dpy)
{
    /*TODO: Set last EGL error for this thread */
    return EGL_FALSE;
}

EGLint EGLAPIENTRY eglGetError (void)
{
    return EGL_CONTEXT_LOST;
}
