workspace "RaylibImGui3DSnake"
    architecture "x64"
    configurations { "Debug", "Release" }

    project "RaylibImGui3DSnake"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        targetdir "bin/%{cfg.buildcfg}"

        files {
            "main.cpp",
            "vendor/imgui/*.cpp",
            "vendor/imgui/*.h",
            "vendor/rlImGui/rlImGui.cpp",
            "vendor/rlImGui/rlImGui.h"
        }

        includedirs {
            "vendor/raylib/include",
            "vendor/imgui",
            "vendor/rlImGui"
        }

        libdirs {
            "vendor/raylib/lib"
        }

        links {
            "raylib",
            "winmm",
            "gdi32",
            "user32",
            "shell32"
        }

        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

        filter "configurations:Release"
            defines { "NDEBUG" }
            optimize "On"
