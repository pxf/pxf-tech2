Import("../../PxfBuild.lua")

library = NewLibrary("portmidi")

local platform_src = ""

if family == "unix" then
    if platform == "macosx" then
        library:AddSystemFramework("CoreAudio");
        library:AddSystemFramework("CoreFoundation");
        library:AddSystemFramework("CoreMIDI");
        library:AddSystemFramework("CoreServices");
        platform_src = "source_osx"
    else
        library:AddSystemLibrary("asound")
        platform_src = "source_alsa"
    end
elseif family == "windows" then
        library:AddSystemLibrary("winmm")
        library:AddSystemLibrary("advapi32")
        platform_src = "source_win"
end

library:AddIncludeDirectory("sdk/include/")
library:AddSourceDirectory("sdk/source/*.c")
library:AddSourceDirectory("sdk/" .. platform_src .. "/*.c")
