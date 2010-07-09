Import("../../PxfBuild.lua")


module = NewModule("SampleModule")
module:AddLibraryDependency("glfw")
module:AddIncludeDirectory("Include")
module:AddIncludeDirectory("Include")
module:AddSourceDirectory("Source/*.cpp")
