Import("../../PxfBuild.lua")

library = NewLibrary("enet")

if family == "windows" then
    library:AddSystemLibrary("ws2_32")
    library:AddSystemLibrary("winmm")
    library:AddSourceDirectory("sdk/src/win32/*.c")
else
    library:AddSourceDirectory("sdk/src/unix/*.c")
end

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
