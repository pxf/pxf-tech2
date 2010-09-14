Import("../../PxfBuild.lua")

library = NewLibrary("sstat")

if family == "windows" then
    library:AddSystemLibrary("psapi")
end

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.cpp")
