Import("../../Build.lua")

project = NewProject()

--project.settings.debug = True
--project.settings.static = True

project:RequireModule("SampleModule")
project:Build()