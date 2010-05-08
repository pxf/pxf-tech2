--/Include
--/Source
--/Module/ModuleName/Source
--/Module/ModuleName/Include
--/Module/ModuleName/Build.lua
--/Libraries/LibName/Source
--/Libraries/LibName/Include
--/Libraries/LibName/Build.lua
--/Build.lua

function NewModule(path)
    module = {}
    
    module.AddLibraryDependency = function(self, library)

    end
    
    module.RequireModule = RequireModule
    
    return module
end

function NewLibrary()
    library = {}
    
    -- Libraries/Frameworks required for each system
    library.system_libraries = {}
    
    library.AddSystemLibrary = function(self, library)
        -- if system == OSX then framework else otherlulz
    end
    
    return library
end

function NewProject()
    project = {}
    project.required_modules = {}
    
    project.RequireModule = function(self, module)
        
    end
    
    project.Build = function(self)
        
        -- Build
        print("test")
        print(self)
            
    end
    
    return project
end
