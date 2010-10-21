derp_components = {}
derp_components.aux = {name = "Auxiliary Blocks"}
derp_components.output = {name = "Output Blocks"}
derp_components.render = {name = "Render Blocks"}
--derp_components.postprocess = {}
--derp_components.output = {}


resources_counter = 0
net._send_file = net.send_file
function net.send_file(filepath)
  resources_counter = resources_counter + 1
  return net._send_file(filepath)
end


-------------------------------------------------------------------------------
-- Output::Simple
derp_components.output.simple = { name = "Ouput: Simple"
                                , tooltip = "Create a block that can render one texture on a remote machine."
                                }
function derp_components.output.simple:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 100, group = "output", type = "simple", inputs = 1, outputs = {}, connections_in = {} }
  
  -- specific values
  block.remotehost = "localhost"
  block.remoteport = 7005
  
  return block
end

function derp_components.output.simple:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,0)
  
  wid.client = net.createclient()
  
  -- host input
  function host_changed(self)
    self.parent.parent.parent.data.remotehost = self.value
    print("host changed to: " .. self.value)
    derp:push_active_workspace()
  end
  local hostlabel = gui:create_labelpanel(5,10,8*6,20,"Host:")
  local hostinput = gui:create_textinput(10+8*6,10,150-8*6,false,component_data.remotehost,host_changed)
  wid.hostinput = hostinput
  wid:addwidget(hostlabel)
  wid:addwidget(hostinput)
  
  -- port input
  function port_changed(self)
    self.parent.parent.parent.data.remoteport = tonumber(self.value)
    print("port changed to: " .. self.value)
    derp:push_active_workspace()
  end
  local portlabel = gui:create_labelpanel(5,35,8*6,20,"Port:")
  local portinput = gui:create_textinput(10+8*6,35,150-8*6,false,tostring(component_data.remoteport),port_changed)
  wid.portinput = portinput
  wid:addwidget(portlabel)
  wid:addwidget(portinput)
  
  -- function that generates the pipeline-json-data
  function render_func(self,mx,my,button)

    -- connect to server
    local connect_fail = self.parent.parent.parent.client:connect(self.parent.parent.parent.data.remotehost, tonumber(self.parent.parent.parent.data.remoteport))
    if connect_fail then
      spawn_error_dialog({"Failed to connect to '" .. tostring(self.parent.parent.parent.data.remotehost) .. "'.",
                          "Reason; '" .. connect_fail .. "'"})
    else
      -- clear resources_counter
      resources_counter = 0
      
      -- clear previews
      derp.active_workspace.preview_data = {}
      
      -- get json for the tree
      local output_blocks_json = derp_components.output.simple:generate_json(self.parent.parent.parent.data)

      if not output_blocks_json then
        return
      end

      -- add pipeline specific data
      table.insert(output_blocks_json, [[{"blockName" : "PipelineTree",
         "blockType" : "PipelineTree",
         "blockData" : { "root" : "]] .. tostring(self.parent.parent.parent.data.id) .. [[" }
        }]])

      -- concat and return!
      local final_json = "[" .. table.concat(output_blocks_json, ",") .. "]"
      print("json data to send: " .. final_json)
      
      self.parent.parent.parent.final_pipeline = final_json

      --return final_json
    
      --net.send_texture("data/guitheme.png")

      -- send our pipeline
      --local new_packet = net.create_packet("", "pipeline" )
      --self.parent.parent.parent.client:send("pipeline", final_json)
    
    end

  
    
  end
  
  wid.superupdate = wid.update
  function wid:update()
      self:superupdate()
      if (self.client:connected()) then
        local indata = self.client:recv_noblock(0)
        if indata then
          print("got packet: " .. tostring(indata.id))
          if (indata.id == "imgdata") then
            
            
            local block,output = indata:get_object(0), indata:get_object(1)
            local w,h,c = indata:get_object(2), indata:get_object(3), indata:get_object(4)
            local imgdata = indata:get_object(5, true)
            --self.previewtex = gfx.rawtexture(128, w,h,c, imgdata)
            --spawn_preview_window(self.previewtex, w,h)
            print("got imgdata for block: " .. block .. " and output: " .. output)
            derp.active_workspace.preview_data[block .. output] = gfx.rawtexture(128, w,h,c, imgdata)
            
            --self.client:disconnect()
          elseif indata.id == "rlog" then
            local sys, what, msg = indata:get_object(0), indata:get_object(1), indata:get_object(2)
            --print(sys, what, msg)
            msg = string.gsub(msg, "%d+", "^(1,0.6,0.6){%1}")
            print("[^(1,0.6,0.6){" .. tostring(sys) .."}] " .. tostring(msg))
            
          elseif indata.id == "ackres" then
            local remote_counter = indata:get_object(0)
            if (resources_counter == remote_counter) then
              -- sending json to server
              self.client:send("pipeline", self.final_pipeline)
            else
              print("Server hasn't got all resources yet (" .. tostring(remote_counter) .. " / " .. tostring(resources_counter) .. ").")
            end
          else
            print("Unknown data packet recieved:")
            for k,v in pairs(indata) do
              print(k,v)
            end
          end
        end
      end
  end
  
  wid.suuuuuuupahdraw = wid.draw
  function wid:draw(force)
    self:suuuuuuupahdraw(force)
    
    --[[for k,v in pairs(self.data.outputs) do
      print(k,v)
    end]]
    local preview = derp.active_workspace.preview_data[self.data.id]
    if (preview ~= nil) then
      gfx.translate(self.drawbox.x,self.drawbox.y-100)
      preview:draw(0,0,64,0,64,64,0,64)
      gfx.translate(-(self.drawbox.x),-(self.drawbox.y-100))
    end
  end
  
  -- render button
  local renderbutton = gui:create_labelbutton(10,70,150,30,"Render", render_func)
  wid.renderbutton = renderbutton
  wid:addwidget(renderbutton)
  
  return wid
end

function derp_components.output.simple:generate_json(component_data)
  local final_jsondata = {}
  local input_array = {}
  local input_array_shader = {}
  
  for k,v in pairs(component_data.connections_in) do
    table.insert(input_array, [[{"block" : "]] .. tostring(v.block) .. [[", "output" : "]] .. tostring(v.output) .. [["}]])
    
    -- get json for the leaf/input
    local tmpblock = derp.active_workspace:get_block(v.block)
    local tmpdict = derp_components[tmpblock.data.group][tmpblock.data.type]:generate_json(tmpblock.data)
    if (tmpdict) then
      for k2,v2 in pairs(tmpdict) do
        table.insert(final_jsondata, v2)
      end
    else
      return nil
    end
  end
  
  local first_texture = nil
  for k,v in pairs(component_data.connections_in) do
    if (v.type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)
      
    elseif (v.type == "geometry") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)

    elseif (v.type == "invert") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)

    end
  end
  
  if (first_texture == nil) then
    return spawn_error_dialog({"Output block needs at least one input!"})
  end
  
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "Root",
     "blockInput" : []] .. tostring(table.concat(input_array, ",\n")) .. [[],
     "blockData" : {"host" : "]] .. tostring(component_data.remotehost) .. [[",
                    "port" : "4632",
                    "feedback" : true,
                    "realtime" : false,
                    "shaderVert" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    void main()
                    {
                      gl_FragColor = texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                    }",
                    "width" : 512,
                    "height" : 512
                   }
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.output.simple:spawn_inspector(component_data)
  return "LOL TODO"
end



-------------------------------------------------------------------------------
-- PostProcess::Invert
derp_components.postprocess.invert = { name = "Post Process: Invert Colors"
                                , tooltip = "Create a block that inverts the colors of a texture."
                                }
function derp_components.postprocess.invert:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 140, h = 60, group = "postprocess", type = "invert", inputs = 1, outputs = {derp:gen_new_outputname()}, connections_in = {} }
  
  return block
end

function derp_components.postprocess.invert:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,1)
  
  wid.suuuuuuupahdraw = wid.draw
  function wid:draw(force)
    self:suuuuuuupahdraw(force)
    
    --[[for k,v in pairs(self.data.outputs) do
      print(k,v)
    end]]
    local preview = derp.active_workspace.preview_data[self.data.id]
    if (preview ~= nil) then
      gfx.translate(self.drawbox.x,self.drawbox.y-100)
      preview:draw(0,0,64,0,64,64,0,64)
      gfx.translate(-(self.drawbox.x),-(self.drawbox.y-100))
    end
  end
  
  return wid
end

function derp_components.postprocess.invert:generate_json(component_data)
  local final_jsondata = {}
  local input_array = {}
  local input_array_shader = {}
  
  for k,v in pairs(component_data.connections_in) do
    table.insert(input_array, [[{"block" : "]] .. tostring(v.block) .. [[", "output" : "]] .. tostring(v.output) .. [["}]])
    
    -- get json for the leaf/input
    local tmpblock = derp.active_workspace:get_block(v.block)
    local tmpdict = derp_components[tmpblock.data.group][tmpblock.data.type]:generate_json(tmpblock.data)
    if (tmpdict) then
      for k2,v2 in pairs(tmpdict) do
        table.insert(final_jsondata, v2)
      end
    else
      return nil
    end
  end
  
  local first_texture = nil
  for k,v in pairs(component_data.connections_in) do
    if (v.type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)
      
    elseif (v.type == "geometry") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)

    elseif (v.type == "invert") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)

    end
  end
  
  if (first_texture == nil) then
    return spawn_error_dialog({"Output block needs at least one input!"})
  end
  
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "Post-Process",
     "blockInput" : []] .. tostring(table.concat(input_array, ",\n")) .. [[],
     "blockData" : {"width" : 512,
                    "height" : 512,
                    "shaderVert" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    uniform float script1;
                    void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    uniform float script1;
                    void main()
                    {
                    	gl_FragData[0] = vec4(1.0) - texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    },]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.invert:spawn_inspector(component_data)
  return "LOL TODO"
end


-------------------------------------------------------------------------------
-- Renderer::geometry
derp_components.render.geometry = { name = "Geometry renderer"
                              , tooltip = "Create a block that inputs geometry and renders to a texture."
                              }
function derp_components.render.geometry:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "render", type = "geometry", inputs = 4, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.modelfilepath = ""
  return block
end

function derp_components.render.geometry:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,2,1)
  
  wid.suuuuuuupahdraw = wid.draw
  function wid:draw(force)
    self:suuuuuuupahdraw(force)
    
    --[[for k,v in pairs(self.data.outputs) do
      print(k,v)
    end]]
    local preview = derp.active_workspace.preview_data[self.data.id]
    if (preview ~= nil) then
      gfx.translate(self.drawbox.x,self.drawbox.y-100)
      preview:draw(0,0,64,0,64,64,0,64)
      gfx.translate(-(self.drawbox.x),-(self.drawbox.y-100))
    end
  end
  
  return wid
end

function derp_components.render.geometry:generate_json(component_data)

  local final_jsondata = {}
  local input_array = {}
  local input_array_shader = {}

  for k,v in pairs(component_data.connections_in) do
    table.insert(input_array, [[{"block" : "]] .. tostring(v.block) .. [[", "output" : "]] .. tostring(v.output) .. [["}]])
  
    -- get json for the leaf/input
    local tmpblock = derp.active_workspace:get_block(v.block)
    local tmpdict = derp_components[tmpblock.data.group][tmpblock.data.type]:generate_json(tmpblock.data)
    if (tmpdict) then
      for k2,v2 in pairs(tmpdict) do
        table.insert(final_jsondata, v2)
      end
    else
      return nil
    end
  end

  local first_texture = nil
  for k,v in pairs(component_data.connections_in) do
    if (v.type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)
    end
  end

  if (first_texture == nil) then
    return spawn_error_dialog({"Geometry block needs at least one texture!"})
  end

  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "Render",
     "blockInput" : []] .. tostring(table.concat(input_array, ",\n")) .. [[],
     "blockData" : {"width" : 512,
                    "height" : 512,
  						"cameraPosition" : "]] .. tostring(component_data.connections_in[1].output) .. [[",
  						"cameraLookAt" : "]] .. tostring(component_data.connections_in[2].output) .. [[",
  						"cameraFov" : 45.0,
                    "shaderVert" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
  						      void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  							      n = gl_Normal;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
                    void main()
                    {
  							      gl_FragColor = texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.render.geometry:spawn_inspector(component_data)
  return "LOL TODO"
end


-------------------------------------------------------------------------------
-- Aux::model (constant)
derp_components.aux.model = { name = "CTM Model"
                              , tooltip = "Create a block that outputs model geometry."
                              }
function derp_components.aux.model:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "aux", type = "model", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.modelfilepath = ""
  return block
end

function derp_components.aux.model:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,0,1)
  
  -- filepath
  local filepathwidget = gui:create_centeredlabelpanel(0,5,170,50,component_data.modelfilepath)
  wid.filepathwidget = filepathwidget
  wid:addwidget(filepathwidget)
  
  function browse_func(self)
    local new_filepath = app.opendialog()
    if (new_filepath) then
      self.parent.parent.parent.filepathwidget.label_text = new_filepath
      self.parent.parent.parent.data.modelfilepath = new_filepath

      derp:push_active_workspace()
    end
  end
  
  -- browse button
  local browsebutton = gui:create_labelbutton(10,30,150,30,"Browse", browse_func)
  wid.browsebutton = browsebutton
  wid:addwidget(browsebutton)
  
  return wid
end

function derp_components.aux.model:generate_json(component_data)
  --print("should send: " .. component_data.modelfilepath)
  local new_filename = net.send_file(component_data.modelfilepath)
  --print("sent file, got new filename: " .. new_filename)
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "AuxComp",
     "blockData" : {"auxType" : "model",
                    "filepath" : "]] .. tostring(new_filename) .. [[",
                    "minfilter" : "nearest"
                   },
     "blockOutput" : [{"name" : "]] .. tostring(component_data.outputs[1]) .. [[",
                       "type" : "geometry"}]
    }]]
    
  if (component_data.texturefilepath == "") then
    return spawn_error_dialog({"Missing model filepath in block '" .. component_data.id .. "'!"})
  end
  
  return {escape_backslashes(jsonstring)}
end

function derp_components.aux.model:spawn_inspector(component_data)
  return "LOL TODO"
end

-------------------------------------------------------------------------------
-- Aux::texture (constant)
derp_components.aux.texture = { name = "Texture"
                                , tooltip = "Create a block that outputs static texture."
                              }
function derp_components.aux.texture:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "aux", type = "texture", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.texturefilepath = ""
  return block
end

function derp_components.aux.texture:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,0,1)
  
  -- filepath
  local filepathwidget = gui:create_centeredlabelpanel(0,5,170,50,component_data.texturefilepath)
  wid.filepathwidget = filepathwidget
  wid:addwidget(filepathwidget)
  
  function browse_func(self)
    local new_filepath = app.opendialog()
    if (new_filepath) then
      self.parent.parent.parent.filepathwidget.label_text = new_filepath
      self.parent.parent.parent.data.texturefilepath = new_filepath

      derp:push_active_workspace()
    end
  end
  
  -- browse button
  local browsebutton = gui:create_labelbutton(10,30,150,30,"Browse", browse_func)
  wid.browsebutton = browsebutton
  wid:addwidget(browsebutton)
  
  return wid
end

function derp_components.aux.texture:generate_json(component_data)
  --print("should send: " .. component_data.texturefilepath)
  local new_filename = net.send_file(component_data.texturefilepath)
  --print("sent file, got new filename: " .. new_filename)
  
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "AuxComp",
     "blockData" : {"auxType" : "texture",
                    "filepath" : "]] .. tostring(new_filename) .. [[",
                    "minfilter" : "nearest"
                   },
     "blockOutput" : [{"name" : "]] .. tostring(component_data.outputs[1]) .. [[",
                       "type" : "texture"}]
    }]]
	 
    
  if (component_data.texturefilepath == "") then
    return spawn_error_dialog({"Missing texture filepath in block '" .. component_data.id .. "'!"})
  end
  
  return {escape_backslashes(jsonstring)}
end

function derp_components.aux.texture:spawn_inspector(component_data)
  return "LOL TODO"
end


-------------------------------------------------------------------------------
-- Aux::float (constant)
derp_components.aux.floatconstant = { name = "Constant: Float"
                                    , tooltip = "Create a block that outputs a constant float value."
                                    }
function derp_components.aux.floatconstant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 100, h = 40, group = "aux", type = "floatconstant", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.constvalue = 0.0
  return block
end

function derp_components.aux.floatconstant:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  local textinput = gui:create_textinput(10,10,80,false)
  wid.textinput = textinput
  textinput.widget_type = "comp text input"
  wid:addwidget(textinput)
  
  return wid
end

function derp_components.aux.floatconstant:generate_json(component_data)
  return "LOL TODO"
end

function derp_components.aux.floatconstant:spawn_inspector(component_data)
  return "LOL TODO"
end


-------------------------------------------------------------------------------
-- Aux::float (constant)
derp_components.aux.vec2constant = { name = "Constant: Vec2"
                                    , tooltip = "Create a block that outputs a constant vec2 value."
                                    }
function derp_components.aux.vec2constant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 100, h = 30, group = "aux", type = "vec2constant", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.constvalue = {0.0, 0.0}
  
  return block
end

function derp_components.aux.vec2constant:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
                                             
  
  return wid
end

function derp_components.aux.vec2constant:generate_json(component_data)
  return "LOL TODO"
end

function derp_components.aux.vec2constant:spawn_inspector(component_data)
  return "LOL TODO"
end

-------------------------------------------------------------------------------
-- Aux::vec3 (constant)
derp_components.aux.vec3constant = { name = "Script: Vec3"
                                    , tooltip = "Create a block that outputs a script that returns a vec3 value."
                                    }
function derp_components.aux.vec3constant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 200, h = 30, group = "aux", type = "vec3constant", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.script = ""
  
  return block
end

function derp_components.aux.vec3constant:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  
  -- script input
  function script_changed(self)
    self.parent.parent.parent.data.script = self.value
    print("script changed to: " .. self.value)
    derp:push_active_workspace()
  end
  local scriptlabel = gui:create_labelpanel(5,10,8*8,20,"Script:")
  local scriptinput = gui:create_textinput(10+8*8,10,180-8*8,false,component_data.script,script_changed)
  wid.scriptinput = scriptinput
  wid:addwidget(scriptlabel)
  wid:addwidget(scriptinput)            
  
  return wid
end

function derp_components.aux.vec3constant:generate_json(component_data)
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "AuxComp",
     "blockData" : {"auxType" : "script",
                    "src" : "]] .. tostring(component_data.script) .. [["
                   },
     "blockOutput" : [{"name" : "]] .. tostring(component_data.outputs[1]) .. [[",
                       "type" : "vec3"}]
    }]]
	 
    
  if (component_data.texturefilepath == "") then
    return spawn_error_dialog({"Missing texture filepath in block '" .. component_data.id .. "'!"})
  end
  
  return {escape_backslashes(jsonstring)}
end

function derp_components.aux.vec3constant:spawn_inspector(component_data)
  return "LOL TODO"
end
