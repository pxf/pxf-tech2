Import("../../PxfBuild.lua")

library = NewLibrary("zthread")

if family == "windows" then
    --library:AddSystemLibrary("ws2_32")
    --library:AddIncludeDirectory("sdk/src/win32")
    --library:AddSourceDirectory("sdk/src/win32/*.cxx")
    library:AddDefine("ZT_WIN32")
else
    library:AddCompilerFlag("-fpermissive") -- gcc
    if platform == "macosx" then
        --library:AddIncludeDirectory("sdk/src/macos")
        --library:AddSourceDirectory("sdk/src/macos/*.cxx")
        library:AddSystemLibrary("pthread")
        library:AddSystemFramework("Carbon")
        library:AddDefine("ZT_MACOS")
    else
        --library:AddIncludeDirectory("sdk/src/linux")
        --library:AddSourceDirectory("sdk/src/linux/*.cxx")
        library:AddSystemLibrary("pthread")
        library:AddDefine("HAVE_ATOMIC_LINUX")
        library:AddDefine("ZT_LINUX")
    end
end

library:AddSourceDirectory("sdk/src/*.c")
library:AddSourceDirectory("sdk/src/*.cxx")
library:AddIncludeDirectory("sdk/include")
library:AddIncludeDirectory("sdk/src")
