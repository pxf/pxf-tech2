Import("../../PxfBuild.lua")


module = NewModule("SampleModule")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
