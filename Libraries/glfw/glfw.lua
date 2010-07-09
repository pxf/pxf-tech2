Import("../../PxfBuild.lua")

if family == "unix" then
    if platform == "macosx" then
        glfw_platform = "macosx"
    else
        glfw_platform = "x11"
    end
elseif family == "windows" then
    glfw_platform = "win32"
end

library = NewLibrary("glfw")

if family == "unix" then
    if platform == "macosx" then
        library:AddSystemFramework("AGL")
        library:AddSystemFramework("OpenGL")
        library:AddSystemFramework("Carbon")
    else
        library:AddSystemLibrary("GLU")
        library:AddSystemLibrary("GL")
    end
elseif family == "windows" then
        library:AddSystemLibrary("opengl32")
        library:AddSystemLibrary("glu32")
        library:AddSystemLibrary("gdi32")
        library:AddSystemLibrary("user32")
        library:AddSystemLibrary("ole32")
end

library:AddIncludeDirectory("Include/")
library:AddIncludeDirectory("Source/")
library:AddIncludeDirectory("Source/" .. glfw_platform)
library:AddSourceDirectory("Source/*.c")
library:AddSourceDirectory("Source/"..glfw_platform.."/*.c")
