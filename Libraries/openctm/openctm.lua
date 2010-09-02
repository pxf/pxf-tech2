Import("../../PxfBuild.lua")

library = NewLibrary("openctm")
library:AddDefine("OPENCTM_STATIC")
library:AddIncludeDirectory("sdk/src/liblzma")
library:AddIncludeDirectory("sdk/src")
library:AddSourceDirectory("sdk/src/*.c")
library:AddSourceDirectory("sdk/src/liblzma/*.c")
