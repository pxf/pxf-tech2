Import("../../PxfBuild.lua")

library = NewLibrary("zeromq")

if family == "windows" then
    library:AddSystemLibrary("ws2_32")
    library:AddSystemLibrary("winmm")
    library:AddSystemLibrary("rpcrt4")
    library:AddDefine("ZMQ_HAVE_WINDOWS")
    
    -- Would prefer to not use DLL_EXPORT
    library:AddDefine("DLL_EXPORT")
else
	if platform == "macosx" then
		library:AddDefine("ZMQ_HAVE_OSX")
	else
		library:AddDefine("ZMQ_HAVE_LINUX")
	end
end

library:AddSourceDirectory("sdk/src/*.c")
library:AddSourceDirectory("sdk/src/*.cpp")

library:AddIncludeDirectory("sdk/src/foreign/xmlParser")
library:AddSourceDirectory("sdk/src/foreign/xmlParser/*.cpp")

library:AddIncludeDirectory("sdk/include")
library:AddIncludeDirectory("sdk/src")
library:AddSourceDirectory("sdk/src/*.c")
