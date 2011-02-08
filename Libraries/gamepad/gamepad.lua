Import("../../PxfBuild.lua")

library = NewLibrary("gamepad")

local platform_src = ""

if family == "unix" then
    if platform == "macosx" then
        library:AddSystemFramework("IOKit")
        library:AddSystemFramework("CoreFoundation")
        platform_src = "source_osx"
    else
        library:AddSystemLibrary("m")
        library:AddSystemLibrary("dl")
        platform_src = "source_linux"
    end
elseif family == "windows" then
        library:AddSystemLibrary("winmm")
        library:AddSystemLibrary("advapi32")
        platform_src = "source_win"
end

library:AddIncludeDirectory("sdk/include/")
library:AddSourceDirectory("sdk/source/*.c")
library:AddSourceDirectory("sdk/" .. platform_src .. "/*.c")
