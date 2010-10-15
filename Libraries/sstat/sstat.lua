Import("../../PxfBuild.lua")

library = NewLibrary("sstat")

if family == "windows" then
    library:AddSystemLibrary("psapi")
    library:AddSystemLibrary("comdlg32")
end

if family == "unix" then
  if not (platform == "macosx") then
  	library:AddIncludeDirectory("/usr/include/gtk-2.0/")
  	library:AddIncludeDirectory("/usr/include/glib-2.0/")
  	library:AddIncludeDirectory("/usr/lib/glib-2.0/include/")
  	library:AddIncludeDirectory("/usr/include/cairo/")
  	library:AddIncludeDirectory("/usr/include/pango-1.0/")
  	library:AddIncludeDirectory("/usr/lib/gtk-2.0/include/")
  	library:AddIncludeDirectory("/usr/include/gdk-pixbuf-2.0/")
  	library:AddIncludeDirectory("/usr/include/atk-1.0/")
  	library:AddSystemLibrary("gtk-x11-2.0")
  	library:AddSystemLibrary("gdk-x11-2.0")
  	library:AddSystemLibrary("atk-1.0")
  	library:AddSystemLibrary("gio-2.0")
  	library:AddSystemLibrary("pangoft2-1.0")
  	library:AddSystemLibrary("pangocairo-1.0")
  	library:AddSystemLibrary("cairo")
  	library:AddSystemLibrary("pango-1.0")
  	library:AddSystemLibrary("freetype")
  	library:AddSystemLibrary("fontconfig")
  	library:AddSystemLibrary("gobject-2.0")
  	library:AddSystemLibrary("gmodule-2.0")
  	library:AddSystemLibrary("gthread-2.0")
  	library:AddSystemLibrary("rt")
  	library:AddSystemLibrary("glib-2.0")
  end
end

library:AddIncludeDirectory("sdk/include")
library:AddSourceDirectory("sdk/src/*.cpp")
