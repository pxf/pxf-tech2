Import("../../PxfBuild.lua")

if family == "windows" then

else
    if platform == "macosx" then
        library:AddSystemLibrary("pthread")
        library:AddDefine("HAVE_PTHREAD")
    else
        library:AddSystemLibrary("pthread")
        library:AddDefine("HAVE_PTHREAD")
    end
end

library = NewLibrary("protobuffers")
library:AddIncludeDirectory("sdk/src")
library:AddSourceDirectory("sdk/src/google/protobuf/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/io/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/stubs/*.cc")
