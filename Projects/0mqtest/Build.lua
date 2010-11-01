Import("../../PxfBuild.lua")

project = NewProject("0mqtest")

project:RequireLibrary("zeromq")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()