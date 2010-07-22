path_prefix = PathDir(ModuleFilename())

function Intermediate_Output(settings, input)
    basepath = Path(PathJoin(path_prefix, "Build/Object"))
    MakeDirectory(Path(basepath .. "/" .. PathDir(input)))
    print(input)
    return Path(basepath .. "/" .. PathJoin(PathDir(input), PathBase(PathFilename(input)) .. settings.config_name))
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
        settings.cc.flags:Add("/EHsc")
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
        objs = Compile(library_settings, source_files)
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
        table.insert(self.required_libraries, library)
    end
     
    module.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, Path(PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir)))
    end
    
    module.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, Path(PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir)))
    end
    
    module.Build = function(self, project, settings, baked_exe)
        local module_settings = settings:Copy()
        local source_files = {}
        for i,m in ipairs(self.include_directories) do
            module_settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            table.insert(source_files, Collect(m))
        end
        
        objs = Compile(module_settings, source_files)
        
        if not baked_exe then
            -- SharedLibrary needs to link with self.required_libs
            libs = {}
            for i, l in ipairs(self.required_libraries) do
                module_settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(project.dep_libraries[l].name))
                lib = project.dep_libraries[l]:Build(self, module_settings)
                table.insert(libs, lib)
                table.insert(project.built_libs, lib)
                project.built_list[l] = l
                
                for i, l in ipairs(project.dep_libraries[l].system_libraries) do
                    module_settings.dll.libs:Add(l)
                end
                
                for i, l in ipairs(project.dep_libraries[l].system_frameworks) do
                    module_settings.dll.frameworks:Add(l)
                end
            end
            return SharedLibrary(module_settings, self.name, objs, libs)
        else
            -- Link to static library
            return StaticLibrary(module_settings, self.name, objs)
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
        table.insert(self.required_libraries, library)
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
        
        print("Preparing to build...")
        
        for i,m in ipairs(self.include_directories) do
            print("Include directory: ", m)
            settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            print("Source directory: ", m)
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
            self.dep_libraries = {}
            
            -- Collect libraries
            for i,n in ipairs(CollectDirs(path_prefix .. "/Libraries/")) do
                Import(n .. "/" .. PathFilename(n) .. ".lua")
                self.dep_libraries[library.name] = library
                for j, incdir in ipairs(library.include_directories) do
                    settings.cc.includes:Add(incdir)
                end
                for j, def in ipairs(library.defines) do
                    settings.cc.defines:Add(def)
                end
            end
            
            -- Collect modules
            for i,m in ipairs(self.required_modules) do
                Import(path_prefix .. "/Modules/" .. m .. "/" .. m .. ".lua")
                dep_modules[module.name] = module
                for j, incdir in ipairs(module.include_directories) do
                    settings.cc.includes:Add(incdir)
                end
            end
            
            self.built_libs = {}
            self.built_mods = {}
            self.built_dlls = {}
            self.built_list = {}
            
            -- Build modules
            for i, m in ipairs(self.required_modules) do
                settings.cc.defines:Add("CONF_WITH_MODULE_"..string.upper(dep_modules[m].name))
                module = dep_modules[m]:Build(self, settings, baked_exe)
                if baked_exe == true then -- Compile modules as static libraries instead of shared libraries
                    table.insert(self.built_mods, module)
                else
                    table.insert(self.built_dlls, module)
                end
            end
            
            for i, l in ipairs(self.required_libraries) do
                if self.built_list[l] == nil then
                    settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(self.dep_libraries[l].name))
                    table.insert(self.built_libs, self.dep_libraries[l]:Build(self, settings))
                end
                
                -- Add system libraries
                for i, l in ipairs(self.dep_libraries[l].system_libraries) do
                    settings.link.libs:Add(l)
                end
                
                -- Add system frameworks (macosx)
                for i, l in ipairs(self.dep_libraries[l].system_frameworks) do
                    settings.link.frameworks:Add(l)
                end
                
            end
            
            -- Pxf source files
            pxf_source_files = Collect(PathJoin(path_prefix, "Source/*.cpp")
                                      ,PathJoin(path_prefix, "Source/Base/*.cpp"))
        
            if family == "unix" then
                settings.link.libs:Add("dl")
                settings.link.libs:Add("pthread")
            end
        
            -- Then build the project
            project = Compile(settings, source_files, pxf_source_files)
            project_exe = Link(settings, self.name, project, self.built_libs, self.built_mods)
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
