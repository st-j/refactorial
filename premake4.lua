local function runcmd(cmd)
    local s, _ = string.gsub(os.outputof(cmd), "\n", "")
    return s
end

solution "refactorial"
targetdir   (_WORKING_DIR)
location    (_WORKING_DIR)

configurations { "Debug", "Release" }
    flags { "NoRTTI" }

configuration "Release"
    defines { "NDEBUG" }
    flags { "Optimize" }

configuration {"linux", "gmake"}
    buildoptions { "-std=c++11", "-pedantic", "-Wno-long-long" }
    buildoptions { runcmd("llvm-config --cppflags") }
    includedirs { runcmd("llvm-config --includedir") }
    libdirs { runcmd("llvm-config --libdir") }
    linkoptions { runcmd("llvm-config --ldflags") }
configuration {}

configuration "vs*"
    buildoptions { "/Za", "/Ze" }
configuration{}

platforms { "native", "x32", "x64" }

local libdeps = 

project "refactorial"
    includedirs "./"
    language "C++"
    kind "ConsoleApp"
    files { "*.cpp", "Transforms/*.cpp" }
    links {
        "clangTooling",
        "clangFrontend",
        "clangSerialization",
        "clangParse",
        "clangSema",
        "clangAnalysis",
        "clangEdit",
        "clangRewriteCore",
        "clangDriver",
        "clangAST",
        "clangASTMatchers",
        "clangLex",
        "clangBasic",
        "LLVM-3.4",
        "yaml-cpp",
        "pcrecpp"
    }
