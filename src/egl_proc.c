#include "eglproxy.h"
#include <string.h>

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
