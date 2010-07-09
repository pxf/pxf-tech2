Import("../../PxfBuild.lua")

library = NewLibrary("rtaudio")

if family == "unix" then
    if platform == "macosx" then
        library:AddSystemFramework("AudioToolbox")
        library:AddSystemFramework("CoreAudio")
        library:AddSystemFramework("AudioUnit")
    else
        library:AddSystemLibrary("asound")
        library:AddSystemLibrary("pthread")
    end
elseif family == "windows" then
        library:AddSystemLibrary("dsound")
        library:AddSystemLibrary("user32")
end

library:AddIncludeDirectory("sdk/")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/*.cpp")
