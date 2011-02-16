Import("../../PxfBuild.lua")

library = NewLibrary("protobuffers")

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

library:AddIncludeDirectory("sdk/src")
library:AddSourceDirectory("sdk/src/google/protobuf/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/io/*.cc")
library:AddSourceDirectory("sdk/src/google/protobuf/stubs/*.cc")
