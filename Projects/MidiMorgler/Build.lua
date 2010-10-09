Import("../../PxfBuild.lua")

project = NewProject("MidiMorgler")

project:RequireLibrary("portmidi")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()