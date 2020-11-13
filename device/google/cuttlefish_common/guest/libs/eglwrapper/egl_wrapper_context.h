// Generated Code - DO NOT EDIT !!
// generated by 'emugen'
#ifndef __egl_wrapper_context_t_h
#define __egl_wrapper_context_t_h

#include "egl_server_proc.h"

#include "egl_types.h"


struct egl_wrapper_context_t {

	eglChooseConfig_wrapper_proc_t eglChooseConfig;
	eglCopyBuffers_wrapper_proc_t eglCopyBuffers;
	eglCreateContext_wrapper_proc_t eglCreateContext;
	eglCreatePbufferSurface_wrapper_proc_t eglCreatePbufferSurface;
	eglCreatePixmapSurface_wrapper_proc_t eglCreatePixmapSurface;
	eglCreateWindowSurface_wrapper_proc_t eglCreateWindowSurface;
	eglDestroyContext_wrapper_proc_t eglDestroyContext;
	eglDestroySurface_wrapper_proc_t eglDestroySurface;
	eglGetConfigAttrib_wrapper_proc_t eglGetConfigAttrib;
	eglGetConfigs_wrapper_proc_t eglGetConfigs;
	eglGetCurrentDisplay_wrapper_proc_t eglGetCurrentDisplay;
	eglGetCurrentSurface_wrapper_proc_t eglGetCurrentSurface;
	eglGetDisplay_wrapper_proc_t eglGetDisplay;
	eglGetError_wrapper_proc_t eglGetError;
	eglGetProcAddress_wrapper_proc_t eglGetProcAddress;
	eglInitialize_wrapper_proc_t eglInitialize;
	eglMakeCurrent_wrapper_proc_t eglMakeCurrent;
	eglQueryContext_wrapper_proc_t eglQueryContext;
	eglQueryString_wrapper_proc_t eglQueryString;
	eglQuerySurface_wrapper_proc_t eglQuerySurface;
	eglSwapBuffers_wrapper_proc_t eglSwapBuffers;
	eglTerminate_wrapper_proc_t eglTerminate;
	eglWaitGL_wrapper_proc_t eglWaitGL;
	eglWaitNative_wrapper_proc_t eglWaitNative;
	eglBindTexImage_wrapper_proc_t eglBindTexImage;
	eglReleaseTexImage_wrapper_proc_t eglReleaseTexImage;
	eglSurfaceAttrib_wrapper_proc_t eglSurfaceAttrib;
	eglSwapInterval_wrapper_proc_t eglSwapInterval;
	eglBindAPI_wrapper_proc_t eglBindAPI;
	eglQueryAPI_wrapper_proc_t eglQueryAPI;
	eglCreatePbufferFromClientBuffer_wrapper_proc_t eglCreatePbufferFromClientBuffer;
	eglReleaseThread_wrapper_proc_t eglReleaseThread;
	eglWaitClient_wrapper_proc_t eglWaitClient;
	eglGetCurrentContext_wrapper_proc_t eglGetCurrentContext;
	eglCreateSyncKHR_wrapper_proc_t eglCreateSyncKHR;
	eglDestroySyncKHR_wrapper_proc_t eglDestroySyncKHR;
	eglClientWaitSyncKHR_wrapper_proc_t eglClientWaitSyncKHR;
	eglGetSyncAttribKHR_wrapper_proc_t eglGetSyncAttribKHR;
	eglCreateImageKHR_wrapper_proc_t eglCreateImageKHR;
	eglDestroyImageKHR_wrapper_proc_t eglDestroyImageKHR;
	virtual ~egl_wrapper_context_t() {}

	typedef egl_wrapper_context_t *CONTEXT_ACCESSOR_TYPE(void);
	static void setContextAccessor(CONTEXT_ACCESSOR_TYPE *f);
	int initDispatchByName( void *(*getProc)(const char *name, void *userData), void *userData);
};

#endif
