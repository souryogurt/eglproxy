#include <stdio.h>
#include "minunit.h"
#include <eglproxy.h>
#ifdef _MSC_VER
#pragma warning( disable: 4127 )
#endif /* _MSC_VER */

static const char *
eglGetErrorTest (void)
{
    /* first call of eglGetError returns EGL_SUCCESS */
    mu_assert (eglGetError() == EGL_SUCCESS);

    /* eglGetError() returns current error state */
    eglSetError (EGL_NOT_INITIALIZED);
    mu_assert (eglGetError() == EGL_NOT_INITIALIZED);

    /* calling eglGetError twice without any other intervening EGL calls will
       always return EGL_SUCCESS on the second call*/
    mu_assert (eglGetError() == EGL_SUCCESS);
    return NULL;
}

static const char *
display_bad_platform (void)
{
    const EGLenum bad_platform = 0xBAD;
    EGLDisplay display = eglGetPlatformDisplay (bad_platform, NULL, NULL);
    mu_assert (display == EGL_NO_DISPLAY);
    mu_assert (eglGetError() == EGL_BAD_PARAMETER);
    return NULL;
}

int main (void)
{
    mu_run_test (eglGetErrorTest);
    mu_run_test (display_bad_platform);
    return 0;
}
