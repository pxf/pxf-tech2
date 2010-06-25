Import("../../PxfBuild.lua")


module = NewModule("SampleModule")
--module:RequireModule(module)
--module:AddLibraryDependency(library)
module:AddIncludeDirectory("Include/")
module:AddSourceDirectory("Source/*.cpp")
