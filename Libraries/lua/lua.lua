Import("../../PxfBuild.lua")

library = NewLibrary("lua")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
