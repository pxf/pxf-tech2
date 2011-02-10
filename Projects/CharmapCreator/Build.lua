Import("../../PxfBuild.lua")

project = NewProject("CharmapCreator")

--project.settings.debug = True
--project.settings.static = True
project:RequireLibrary("glfw")
project:RequireModule("pri")
project:RequireModule("img")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()