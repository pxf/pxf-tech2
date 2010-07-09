Import("../../PxfBuild.lua")

library = NewLibrary("soil")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.c")
