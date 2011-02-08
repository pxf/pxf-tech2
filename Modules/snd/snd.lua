Import("../../PxfBuild.lua")


module = NewModule("snd")
module:RequireLibrary("stb_vorbis")
module:RequireLibrary("rtaudio")
module:RequireLibrary("zthread")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
