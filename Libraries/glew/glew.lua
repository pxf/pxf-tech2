Import("../../PxfBuild.lua")

library = NewLibrary("glew")
library:AddDefine("GLEW_STATIC")
library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/source/*.c")
