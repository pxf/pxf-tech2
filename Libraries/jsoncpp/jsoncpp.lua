Import("../../PxfBuild.lua")

library = NewLibrary("jsoncpp")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/source/*.cpp")
