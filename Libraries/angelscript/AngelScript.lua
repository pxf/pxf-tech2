Import("../../PxfBuild.lua")

library = NewLibrary("angelscript")
library:AddIncludeDirectory("sdk/angelscript/include")
library:AddIncludeDirectory("sdk/angelscript/source")
library:AddSourceDirectory("sdk/angelscript/source/*.cpp")
