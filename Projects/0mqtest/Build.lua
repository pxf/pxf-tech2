Import("../../PxfBuild.lua")

project = NewProject("0mqtest")

project:RequireLibrary("zeromq")
project:RequireLibrary("protobuffers")
project:RequireLibrary("zthread")
project:RequireLibrary("vectormathlibrary")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
