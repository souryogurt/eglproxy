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
