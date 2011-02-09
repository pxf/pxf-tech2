Import("../../PxfBuild.lua")

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

library = NewLibrary("soil")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
