Import("../../PxfBuild.lua")

library = NewLibrary("rtaudio")

if family == "unix" then
    if platform == "macosx" then
        library:AddSystemFramework("AudioToolbox")
        library:AddSystemFramework("CoreAudio")
        library:AddSystemFramework("AudioUnit")
        library:AddDefine("__MACOSX_CORE__")
    else
        library:AddSystemLibrary("asound")
        library:AddSystemLibrary("pthread")
        library:AddDefine("__LINUX_ALSA__")
    end
elseif family == "windows" then
        library:AddSystemLibrary("dsound")
        library:AddSystemLibrary("user32")
        library:AddDefine("__WINDOWS_DS__")
end

library:AddIncludeDirectory("sdk/")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/*.cpp")
