function basic_serialize(val,ident)
	if type(val) == "number" or type(val) == "boolean" then
		return tostring(val)
		
	elseif type(val) == "string" then
    return string.format("%q", val)	
	
	elseif type(val) == "table" then
		-- new table
		local ret = "{\n"
		local ident_str = ""
		
		for i = 0,ident do
			ident_str = ident_str .. "    "
		end
	
		for k,v in pairs(val) do
			ret = ret .. ident_str .. "[" .. basic_serialize(k) .. "] = " .. basic_serialize(v,ident+1) .. ",\n"
		end
		
		ret = ret .. "\n}"
		
		return ret
	else
		error("cannot serialize type: " .. type(val))
	end
end

function new_settings_handler(filename, defaults)
  local settings = {data = defaults, filename = filename}
  
  function settings:save()
    local serialdata = basic_serialize(self.data, 0)
  	local file = io.output(self.filename, "w")
  	io.write(serialdata)
  	file:close()
  	--for k,v in pairs(self.data) do
  	-- print(k,v)
  	--end
  	--print("Saved settings to: " .. self.filename)
  end
  
  function settings:load()
    local file = io.open(self.filename, "r")
    if file then
      local indata = nil
    	local str = file:read("*a")
    	indata = loadstring("return" .. str)()
    	for k,v in pairs(indata) do
    	 if self.data[k] then
    	   self.data[k] = v
  	   end
    	end
    	file:close()
    end
    print("Loaded settings from: " .. self.filename)
  end
  
  return settings
end