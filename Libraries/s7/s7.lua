Import("../../PxfBuild.lua")

library = NewLibrary("s7")

library:AddIncludeDirectory("sdk/")
library:AddSourceDirectory("sdk/*.c")
