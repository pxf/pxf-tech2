Import("../../PxfBuild.lua")


module = NewModule("snd")
module:RequireLibrary("rtaudio")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
