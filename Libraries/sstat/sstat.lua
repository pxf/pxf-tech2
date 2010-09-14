Import("../../PxfBuild.lua")

library = NewLibrary("sstat")
library:AddSystemLibrary("Psapi")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.cpp")
