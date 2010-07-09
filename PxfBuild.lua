path_prefix = PathDir(ModuleFilename())

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
        
        for i, l in ipairs(self.system_libraries) do
            settings.link.libs:Add(l)
        end
        
        for i, l in ipairs(self.system_frameworks) do
            settings.link.frameworks:Add(l)
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
    
    module.Build = function(self, project, settings)
        local module_settings = settings:Copy()
        local source_files = {}
        for i,m in ipairs(self.include_directories) do
            module_settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            table.insert(source_files, CollectRecursive(m))
        end
        
        -- Also handle other modules and libs
        objs = Compile(module_settings, source_files)
        -- Link to static library
        return StaticLibrary(module_settings, self.name, objs)
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
        
        debug_settings = settings:Copy()
        debug_settings.config_name = "debug"
        debug_settings.config_ext = "_d"
        debug_settings.debug = 1
        debug_settings.optimize = 0
        debug_settings.cc.defines:Add("CONF_DEBUG")

        release_settings = settings:Copy()
        release_settings.config_name = "release"
        release_settings.config_ext = ""
        release_settings.debug = 0
        release_settings.optimize = 1
        release_settings.cc.defines:Add("CONF_RELEASE")
        
        -- Compile Project
        local DoBuild = function(settings, source_files, libraries)
            local dep_modules = {}
            local dep_libraries = {}
            
            -- Collect libraries
            for i,n in ipairs(CollectDirs(path_prefix .. "/Libraries/")) do
                Import(n .. "/" .. PathFilename(n) .. ".lua")
                dep_libraries[library.name] = library
                for j, incdir in ipairs(library.include_directories) do
                    settings.cc.includes:Add(incdir)
                end
                for j, def in ipairs(library.defines) do
                    settings.cc.defines:Add(def)
                end
            end
            
            -- Collect modules
            for i,m in ipairs(self.required_modules) do
                -- Import the module build files, and build modules.
                for i,n in ipairs(CollectDirs(path_prefix .. "/Modules/")) do
                    Import(n .. "/" .. PathFilename(n) .. ".lua")
                    dep_modules[module.name] = module
                    for j, incdir in ipairs(module.include_directories) do
                        settings.cc.includes:Add(incdir)
                    end
                end
            end
            
            local built_libs = {}
            local built_mods = {}
            local built_list = {}
            
            -- Build modules
            for i, m in ipairs(self.required_modules) do
                for i, l in ipairs(dep_modules[m].required_libraries) do
                    table.insert(built_libs, dep_libraries[l]:Build(self, settings))
                    settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(dep_libraries[l].name))
                    built_list[l] = l
                end
                table.insert(built_mods, dep_modules[m]:Build(self, settings))
                settings.cc.defines:Add("CONF_WITH_MODULE_"..string.upper(dep_modules[m].name))
            end
            
            for i, l in ipairs(self.required_libraries) do
                if built_list[l] == nil then
                    table.insert(built_libs, dep_libraries[l]:Build(self, settings))
                    settings.cc.defines:Add("CONF_WITH_LIBRARY_"..string.upper(dep_libraries[l].name))
                end
            end
        
            -- Then build the project
            project = Compile(settings, source_files)
            project_exe = Link(settings, self.name, project, built_libs, built_mods)
            project_target = PseudoTarget(self.name.."_"..settings.config_name, project_exe)
            PseudoTarget(settings.config_name, project_target)
        end
        DoBuild(debug_settings, source_files, {})
        DoBuild(release_settings, source_files,  {})
        
    end

    return project
end
