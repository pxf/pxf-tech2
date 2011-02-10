Import("../../PxfBuild.lua")

library = NewLibrary("vectormathlibrary")

library:AddIncludeDirectory("sdk/include/vectormath/cpp/")
library:AddIncludeDirectory("sdk/include/vectormath")

if arch == "ppc" then
	library:AddDefine("ARCH=ppu");
	--library:AddIncludeDirectory("sdk/include/vectormath/ppu/cpp")
elseif arch == "ia32" or arch == "ia64" or arch == "amd64" then
	project:AddDefine("ARCH=SSE")
	--library:AddIncludeDirectory("sdk/include/vectormath/SSE/cpp")
else
	project:AddDefine("ARCH=scalar")
	--library:AddIncludeDirectory("sdk/include/vectormath/cpp")
end

library:AddSourceDirectory("sdk/src/*.cpp")
