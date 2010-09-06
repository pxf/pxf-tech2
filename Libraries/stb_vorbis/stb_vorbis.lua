Import("../../PxfBuild.lua")

library = NewLibrary("stb_vorbis")

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/source/*.c")
