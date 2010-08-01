path_prefix = PathDir(ModuleFilename())

function lockglobalenvironment()
    local function _n(t, i, k) 
        error("writing to new global variables is disabled.", 2);
    end

    local function _i(t, i)
        error("reading to new global variables is disabled.", 2);
    end

    -- work with the global variable table ('_G')
    local mt = { __newindex = _n, __index = _i };
    setmetatable(_G, mt);
end

-- this reverts the global variable lock.
function unlockglobalenvironment()
    local mt = getmetatable(_G);
    mt.__index = nil;
    mt.__newindex = nil;
end

function Intermediate_Output(settings, input)
    basepath = Path(PathJoin(path_prefix, "Build/Object"))
    MakeDirectory(Path(basepath .. "/" .. PathDir(input)))
    print(input)
    return Path(basepath .. "/" .. PathJoin(PathDir(input), PathBase(PathFilename(input)) .. settings.config_name))
end

function LoadLibrary(name)
     Import(path_prefix .. "/Libraries/" .. name .. "/" .. name .. ".lua")
     return library;
end

function NewLibrary(name)
    local library = {}
    library.name = name
    
    -- Libraries/Frameworks required for each system
    library.system_libraries = {}
    library.system_frameworks = {}
    library.include_directories = {}
    library.source_directories = {}
    library.defines = {}
    
    library.AddDefine = function(self, def)
        table.insert(self.defines, def)
    end
    
    library.AddSystemLibrary = function(self, lib)
        table.insert(self.system_libraries, lib)
    end
    
    library.AddSystemFramework = function(self, framework)
        if platform == "macosx" then
            table.insert(self.system_frameworks, framework)
        end
    end
    
    library.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, Path(PathJoin(PathJoin(path_prefix, "Libraries/"..self.name), dir)))
    end
    
    library.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, Path(PathJoin(PathJoin(path_prefix, "Libraries/"..self.name), dir)))
    end
    
    library.Build = function(self, project, settings)
        local library_settings = settings:Copy()
        local source_files = {}
        
        for i,d in ipairs(self.defines) do
            library_settings.cc.defines:Add(d)
        end
        
        for i,m in ipairs(self.include_directories) do
            library_settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            for j, p in ipairs(Collect(m)) do
                table.insert(source_files, p)
            end
        end
        
        -- Also handle other modules and libs
        local objs = Compile(library_settings, source_files)
        -- Link to static library
        return StaticLibrary(library_settings, self.name, objs)
    end
    
    return library
end

function NewModule(name)
    local module = {}
    module.name = name
    module.required_libraries = {}
    module.required_modules = {}
    module.include_directories = {}
    module.source_directories = {}
    
--    module.RequireModule = function(self, module)
--        table.insert(self.required_modules, module)
--    end
    
    module.RequireLibrary = function(self, library)
        if self.required_libraries[library] == nil then
            table.insert(self.required_libraries, library)
        end
    end
     
    module.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, Path(PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir)))
    end
    
    module.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, Path(PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir)))
    end
    
    module.Build = function(self, project, settings, frameworkobjs, baked_exe)
        local module_settings = settings:Copy()
        local source_files = {}
        for i,m in ipairs(self.include_directories) do
            module_settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            for j, p in ipairs(Collect(m)) do
                table.insert(source_files, p)
            end
        end
       
        if not baked_exe then
            -- SharedLibrary needs to link with self.required_libs
            libs = {}
            for i, l in ipairs(self.required_libraries) do
                if not project.built_list[l] then
                    library = LoadLibrary(l)
                    module_settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(library.name))
                    lib = library:Build(self, module_settings)
                    table.insert(libs, lib)
                    table.insert(project.built_libs, lib)
                    project.built_list[l] = true
                else
                    -- TODO: Else what?
                end
                
                for i, l in ipairs(library.system_libraries) do
                    module_settings.dll.libs:Add(l)
                end
                
                for i, l in ipairs(library.system_frameworks) do
                    module_settings.dll.frameworks:Add(l)
                end
            end
            local objs = Compile(module_settings, source_files)
            return SharedLibrary(module_settings, self.name, objs, frameworkobjs, libs)
        else
            -- Return compiled objects instead of creating a static library.
            local objs = Compile(module_settings, source_files)
            return objs;
        end
    end
    
    return module
end

function NewProject(name)
    local project = {}
    project.name = name
    project.required_modules = {}
    project.required_libraries = {}
    project.include_directories = {}
    project.source_directories = {}
    
    project.RequireLibrary = function(self, library)
        if self.required_libraries[library] == nil then
            table.insert(self.required_libraries, library)
        end
    end
    
    project.RequireModule = function(self, module)
        table.insert(self.required_modules, module)
    end
     
    project.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, dir)
    end
    
    project.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, dir)
    end
        
    project.Build = function(self)
        local settings = NewSettings()
        local source_files = {}
        
        for i,m in ipairs(self.include_directories) do
            settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            table.insert(source_files, CollectRecursive(m))
        end
        
        --settings.cc.Output = Intermediate_Output
        if family == "windows" then
            settings.cc.flags:Add("/EHsc")
        end
        
        -- Add framework include dir
        settings.cc.includes:Add(PathJoin(path_prefix, "Include"))
        
        debug_settings = settings:Copy()
        debug_settings.cc.defines:Add("CONF_DEBUG")
        debug_settings.config_name = "debug"
        debug_settings.config_ext = "_d"
        debug_settings.debug = 1
        debug_settings.optimize = 0
        debug_settings_dll = debug_settings:Copy()
        debug_settings_dll.config_name = "debug_dll"
        debug_settings_dll.config_ext = "_ddyn"
        debug_settings_dll.cc.defines:Add("CONF_MODULAR")

        release_settings = settings:Copy()
        release_settings.cc.defines:Add("CONF_RELEASE")
        release_settings.config_name = "release"
        release_settings.config_ext = ""
        release_settings.debug = 0
        release_settings.optimize = 1
        release_settings_dll = release_settings:Copy()
        release_settings_dll.config_name = "release_dll"
        release_settings_dll.config_ext = "_s"
        release_settings_dll.cc.defines:Add("CONF_MODULAR")
        
        -- Compile Project
        local DoBuild = function(settings, source_files, baked_exe, libraries)
            local dep_modules = {}
            local req_libraries = {}
            
            self.built_libs = {}
            self.built_mods = {}
            self.built_dlls = {}
            self.built_list = {}
            
            for i,l in ipairs(self.required_libraries) do
                req_libraries[l] = true
            end
            
            if baked_exe == false then
                if family == "windows" then
                    settings.cc.defines:Add("PXFEXPORT=\"extern \\\"C\\\" __declspec(dllexport)\"")
                else
                    settings.cc.defines:Add("PXFEXPORT=\"extern \\\"C\"\\\"")
                end
            else
                settings.cc.defines:Add("PXFEXPORT=\"\"")
            end
           
            -- Collect modules
            for i,m in ipairs(self.required_modules) do
                Import(path_prefix .. "/Modules/" .. m .. "/" .. m .. ".lua")
                dep_modules[module.name] = module
                for j, incdir in ipairs(module.include_directories) do
                    settings.cc.includes:Add(incdir)
                end
                for j, lib in ipairs(module.required_libraries) do
                    library = LoadLibrary(lib)
                    
                    for j, incdir in ipairs(library.include_directories) do
                        settings.cc.includes:Add(incdir)
                    end
                    
                    for j, def in ipairs(library.defines) do
                        settings.cc.defines:Add(def)
                    end
                end
            end
            
            -- Build framework
            -- Pxf source files
            framework_settings = settings:Copy()
            pxf_source_files = Collect(PathJoin(path_prefix, "Source/*.cpp")
                                      ,PathJoin(path_prefix, "Source/Base/*.cpp")
                                      ,PathJoin(path_prefix, "Source/Graphics/*.cpp")
                                      ,PathJoin(path_prefix, "Source/Resource/*.cpp"))
        
            if family == "unix" then
                settings.link.libs:Add("dl")
                settings.link.libs:Add("pthread")
            end
            
            pxf_objs = Compile(framework_settings, pxf_source_files)
            
            -- Build modules
            for i, m in ipairs(self.required_modules) do
                settings.cc.defines:Add("CONF_WITH_MODULE_"..string.upper(dep_modules[m].name))
                module = dep_modules[m]:Build(self, settings, pxf_objs, baked_exe)
                -- Build with embedded modules
                if baked_exe == true then
                    table.insert(self.built_mods, module)
                    -- Add required libraries if they aren't already wanted
                    for j, l in ipairs(dep_modules[m].required_libraries) do
                        req_libraries[l] = true
                    end
                -- Build with dynamic modules
                else
                    table.insert(self.built_dlls, module)
                end
            end
            
            for l, i in pairs(req_libraries) do
                library = LoadLibrary(l)
  
                if not self.built_list[l] then
                    settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(library.name))
                    table.insert(self.built_libs, library:Build(self, settings))
                    self.built_list[l] = true
                end
                
                -- Add system libraries
                for i, l in ipairs(library.system_libraries) do
                    settings.link.libs:Add(l)
                end
                
                -- Add system frameworks (macosx)
                for i, l in ipairs(library.system_frameworks) do
                    settings.link.frameworks:Add(l)
                end
                
            end

            -- Then build the project
            project = Compile(settings, source_files)
            project_exe = Link(settings, self.name, project, pxf_objs, self.built_mods, self.built_libs)
            project_target = PseudoTarget(self.name.."_"..settings.config_name, project_exe)
            PseudoTarget(settings.config_name, project_target, self.built_dlls)
            return project_exe
        end
        DefaultTarget(DoBuild(debug_settings, source_files, true, {}))
        DoBuild(release_settings, source_files, true, {})
        DoBuild(debug_settings_dll, source_files, false, {})
        DoBuild(release_settings_dll, source_files, false, {})
        
    end

    return project
end
