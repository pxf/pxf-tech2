Import("../../PxfBuild.lua")

library = NewLibrary("SampleLibrary")
library:AddSystemLibrary("ole")
library:AddIncludeDirectory("Include")
library:AddSourceDirectory("Source/*.cpp")