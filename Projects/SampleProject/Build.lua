Import("../../PxfBuild.lua")

project = NewProject("SampleProject")

--project.settings.debug = True
--project.settings.static = True

project:RequireModule("SampleModule")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()