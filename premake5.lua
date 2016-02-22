workspace "eglproxy"
    configurations { "Debug", "Release" }

project "eglproxy"
    kind "SharedLib"
    language "C"
    files {
        "inc/**.h",
        "src/eglproxy.c",
        "src/egl_proc.c"
    }
    includedirs { "inc" }
    defines { "eglproxy_EXPORTS" }

    if os.findlib("Gdi32") then
        files { "src/egl_wgl.c" }
        links { "OpenGL32", "Gdi32" }
    elseif os.findlib("X11") then
        files { "src/egl_glx.c" }
        links { "GL", "X11" }
    end

    filter "configurations:Release"
        defines { "NDEBUG" }
        flags {
            "LinkTimeOptimization",
            "NoFramePointer",
            "NoMinimalRebuild"
        }
        optimize "Full"
    filter "configurations:Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
