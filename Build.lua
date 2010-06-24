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
    
    library.AddSystemLibrary = function(self, library)
        table.insert(self.system_libraries, library)
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
        table.insert(self.include_directories, dir)
    end
    
    module.AddSourceDirectory = function(self, dir)
        table.insert(self.source_directories, dir)
    end
    
    module.Build = function(self)
        print("Building module [".. self.name .."]")
    end
    
    return module
end

function NewProject()
    local project = {}
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
        print("Preparing to build...")
        
        for i,m in ipairs(self.required_modules) do
            print("Required module: ", m)
            
            -- Import the module build files, and build modules.
            for i,n in ipairs(CollectDirs(path_prefix .. "/Modules/")) do
                Import(n .. "/Build.lua")
                table.insert(modules, module)
                module:Build()
            end
        end
        
        for i,m in ipairs(self.include_directories) do
            print("Include directory: ", m)
        end
        
        for i,m in ipairs(self.source_directories) do
            print("Source directory: ", m)
        end
    end
    
    return project
end
