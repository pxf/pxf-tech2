Import("../../PxfBuild.lua")

library = NewLibrary("box2d")

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.cpp")
