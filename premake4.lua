solution "eglproxy"
	configurations { "Debug", "Release" }
	project "eglproxy"
		kind "SharedLib"
		language "C"
		files { "inc/eglproxy.h",
			"inc/EGL/egl.h",
			"inc/EGL/eglext.h",
			"inc/EGL/eglplatform.h",
			"inc/KHR/khrplatform.h",
			"src/egl_proc.c",
			"src/eglproxy.c" }
		includedirs { "inc" }

		if os.is("windows") then
			files { "src/egl_wgl.c"}
			links { "OpenGL32" }
		elseif os.is("linux") or os.is("bsd") then
			files { "src/egl_glx.c"}
			links { "GL" }
		end

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }
		configuration "Release"
			defines { "NDEBUG"}
			flags { "Optimize" }

