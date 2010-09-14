Import("../../PxfBuild.lua")

library = NewLibrary("sstat")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
