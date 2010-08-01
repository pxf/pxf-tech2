Import("../../PxfBuild.lua")

project = NewProject("SampleProject")

print(table.concat(CollectDirs("./*"), ", "))
print(table.concat(CollectDirs("../"), ", "))

--project.settings.debug = True
--project.settings.static = True
project:RequireModule("pri")
project:RequireModule("img")
project:AddIncludeDirectory("Include/")
project:AddSourceDirectory("Source/*.cpp")
project:Build()