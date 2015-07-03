#include <stdio.h>
#include "minunit.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

static const char *
eglGetErrorTest (void)
{
    /* first call of eglGetError returns EGL_SUCCESS */
    mu_assert (eglGetError() == EGL_SUCCESS);
    /* calling eglGetError twice without any other intervening EGL calls will
       always return EGL_SUCCESS on the second call*/
    mu_assert (eglGetError() == EGL_SUCCESS);
    return NULL;
}

int main (void)
{
    mu_run_test (eglGetErrorTest);
    return 0;
}
