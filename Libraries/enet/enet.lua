Import("../../PxfBuild.lua")

library = NewLibrary("enet")

if family == "windows" then
    library:AddSystemLibrary("ws2_32")
    library:AddSystemLibrary("winmm")
end

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
