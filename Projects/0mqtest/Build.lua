Import("../../PxfBuild.lua")

project = NewProject("0mqtest")

project:RequireLibrary("zeromq")
project:RequireLibrary("vectormathlibrary")
project:RequireLibrary("protobuffers")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()
