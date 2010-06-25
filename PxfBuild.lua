--/Include
--/Source
--/Module/ModuleName/Source
--/Module/ModuleName/Include
--/Module/ModuleName/Build.lua
--/Libraries/LibName/Source
--/Libraries/LibName/Include
--/Libraries/LibName/Build.lua
--/Build.lua

path_prefix = PathDir(ModuleFilename())

function NewLibrary()
    local library = {}
    
    -- Libraries/Frameworks required for each system
    library.system_libraries = {}
    library.system_frameworks = {}
    library.include_directories = {}
    library.source_directories = {}
    
    library.AddSystemLibrary = function(self, lib)
        table.insert(self.system_libraries, lib)
    end
    
    library.AddSystemFramework = function(self, framework)
        if platform == "macosx" then
            table.insert(self.system_frameworks, framework)
        end
    end
    
    library.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, dir)
    end
    
    library.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, dir)
    end
    
    library.Build = function(self, project, settings)
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
    
    module.RequireModule = function(self, module)
        table.insert(self.required_modules, module)
    end
    
    module.AddLibraryDependency = function(self, library)
        table.insert(self.required_libraries, dir)
    end
     
    module.AddIncludeDirectory = function(self, dir)
        table.insert(self.include_directories, PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir))
    end
    
    module.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, PathJoin(PathJoin(path_prefix, "Modules/"..self.name), dir))
    end
    
    module.Build = function(self, project, settings)
        print("Building module [".. self.name .. settings.config_ext .. "]")
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
    project.settings = NewSettings()
    project.required_modules = {}
    project.include_directories = {}
    project.source_directories = {}
    
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
        local modules = {}
        local source_files = {}
        
        print("Preparing to build...")
        print("Required modules: ", table.concat(self.required_modules, ", "))
        
        
        for i,m in ipairs(self.include_directories) do
            print("Include directory: ", m)
            self.settings.cc.includes:Add(m)
        end
        
        for i,m in ipairs(self.source_directories) do
            print("Source directory: ", m)
            table.insert(source_files, CollectRecursive(m))
        end
        
        debug_settings = self.settings:Copy()
        debug_settings.config_name = "debug"
        debug_settings.config_ext = "_d"
        debug_settings.debug = 1
        debug_settings.optimize = 0
        debug_settings.cc.defines:Add("CONF_DEBUG")

        release_settings = self.settings:Copy()
        release_settings.config_name = "release"
        release_settings.config_ext = ""
        release_settings.debug = 0
        release_settings.optimize = 1
        release_settings.cc.defines:Add("CONF_RELEASE")
        
        -- Compile Project
        local DoBuild = function(settings, source_files, libraries)
            local modules = {}
            -- Build modules first
            for i,m in ipairs(self.required_modules) do
                -- Import the module build files, and build modules.
                for i,n in ipairs(CollectDirs(path_prefix .. "/Modules/")) do
                    Import(n .. "/Module.lua")
                    table.insert(modules, module:Build(self, settings))
                    for j, incdir in ipairs(module.include_directories) do
                        settings.cc.includes:Add(incdir)
                    end
                end
            end
        
            -- Then build the project
            project = Compile(settings, source_files)
            project_exe = Link(settings, self.name, project, modules, libraries)
            project_target = PseudoTarget(self.name.."_"..settings.config_name, project_exe)
            PseudoTarget(settings.config_name, project_target)
        end
        DoBuild(debug_settings, source_files, {})
        DoBuild(release_settings, source_files,  {})
        
    end

    return project
end
