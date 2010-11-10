Import("../../PxfBuild.lua")

library = NewLibrary("protobuffers")
library:AddIncludeDirectory("sdk/src")
library:AddSourceDirectory("sdk/src/google/protobuf/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/io/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/stubs/*.cc")
