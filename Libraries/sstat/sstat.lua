Import("../../PxfBuild.lua")

library = NewLibrary("sstat")

if family == "windows" then
    library:AddSystemLibrary("psapi")
end

if family == "unix" then
	library:AddIncludeDirectory("/usr/include/gtk-2.0/")
	library:AddIncludeDirectory("/usr/include/glib-2.0/")
	library:AddIncludeDirectory("/usr/lib/glib-2.0/include/")
	library:AddIncludeDirectory("/usr/include/cairo/")
	library:AddIncludeDirectory("/usr/include/pango-1.0/")
	library:AddIncludeDirectory("/usr/lib/gtk-2.0/include/")
	library:AddIncludeDirectory("/usr/include/gdk-pixbuf-2.0/")
	library:AddIncludeDirectory("/usr/include/atk-1.0/")
end

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.cpp")
