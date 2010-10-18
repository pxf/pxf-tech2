derp_components = {}
derp_components.aux = {name = "Auxiliary Blocks"}
derp_components.output = {name = "Output Blocks"}
--derp_components.render = {}
--derp_components.postprocess = {}
--derp_components.output = {}

-- Aux::Script
--[[derp_components.aux.script = { name = "Generic Script Block"
                             , tooltip = "Create a generic Lua script block."
                             }
function derp_components.aux.script:new_block(x,y)
  local block = { x = x, y = y, w = 100, h = 100, group = "aux", type = "script", inputs = {}, outputs = { 0 } }
  return block
end

function derp_components.aux.script:generate_json(component_data)
  return "LOL TODO"
end

function derp_components.aux.script:draw(component_data)
  -- TODO: draw component in workspace
end]]


-------------------------------------------------------------------------------
-- Output::Simple
derp_components.output.simple = { name = "Ouput: Simple"
                                , tooltip = "Create a block that can render one texture on a remote machine."
                                }
function derp_components.output.simple:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 150, h = 80, group = "output", type = "simple", inputs = 1, outputs = {}, connections_in = {} }
  
  -- specific values
  block.remotehost = "localhost"
  
  return block
end

function derp_components.output.simple:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,0)
  
  wid.client = net.createclient()
  
  -- host input
  function host_changed(self)
    self.parent.parent.data.remotehost = self.value
    print("host changed to: " .. self.value)
    derp:push_active_workspace()
  end
  local hostinput = gui:create_textinput(10,10,80,false,component_data.remotehost,host_changed)
  wid.hostinput = hostinput
  wid:addwidget(hostinput)
  
  -- function that generates the pipeline-json-data
  function render_func(self,mx,my,button)
  
    -- get json for the tree
    local output_blocks_json = derp_components.output.simple:generate_json(self.parent.parent.data)
    
    if not output_blocks_json then
      return
    end
    
    -- add pipeline specific data
    table.insert(output_blocks_json, [[{"blockName" : "PipelineTree",
       "blockType" : "PipelineTree",
       "blockData" : { "root" : "]] .. tostring(self.parent.parent.data.id) .. [[" }
      }]])
    
    -- concat and return!
    local final_json = "[" .. table.concat(output_blocks_json, ",") .. "]"
    print(final_json)
    
    -- connect to server
    local connect_fail = self.parent.parent.client:connect(self.parent.parent.data.remotehost, 7005)
    if connect_fail then
      spawn_error_dialog({"Failed to connect to '" .. tostring(self.parent.parent.data.remotehost) .. "'.",
                          "Reason; '" .. connect_fail .. "'"})
    else
      
      -- send our pipeline
      self.parent.parent.client:send("pipeline", final_json)
      
    end
    
    --return final_json
  end
  
  wid.superupdate = wid.update
  function wid:update()
      self:superupdate()
      if (self.client:connected()) then
        print("connected")
        local indata = self.client:recv_noblock(0)
        if indata then
          print("got packet")
          --[[for k,v in pairs(indata) do
            print(k,v)
          end]]
        end
      end
  end
  
  -- render button
  local renderbutton = gui:create_labelbutton(10,40,100,30,"Render", render_func)
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
    for k2,v2 in pairs(tmpdict) do
      table.insert(final_jsondata, v2)
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
  
  table.insert(final_jsondata, jsonstring)
  
  return final_jsondata
end

function derp_components.output.simple:spawn_inspector(component_data)
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
-- Aux::texture (constant)
derp_components.aux.texture = { name = "Texture"
                                , tooltip = "Create a block that outputs static texture."
                              }
function derp_components.aux.texture:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 50, group = "aux", type = "texture", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.texturefilepath = ""
  return block
end

function derp_components.aux.texture:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,2,2)
  
  -- filepath
  local filepathwidget = gui:create_centeredlabelpanel(0,80,170,50,component_data.texturefilepath)
  wid.filepathwidget = filepathwidget
  wid:addwidget(filepathwidget)
  
  function browse_func(self)
    local new_filepath = app.opendialog()
    if (new_filepath) then
      self.parent.parent.filepathwidget.label_text = new_filepath
      self.parent.parent.data.texturefilepath = new_filepath

      derp:push_active_workspace()
    end
  end
  
  -- browse button
  local browsebutton = gui:create_labelbutton(10,10,150,30,"Browse", browse_func)
  wid.browsebutton = browsebutton
  wid:addwidget(browsebutton)
  
  return wid
end

function derp_components.aux.texture:generate_json(component_data)
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "AuxComp",
     "blockData" : {"auxType" : "texture",
                    "filepath" : "]] .. tostring(component_data.texturefilepath) .. [[",
                    "minfilter" : "nearest"
                   },
     "blockOutput" : [{"name" : "]] .. tostring(component_data.outputs[1]) .. [[",
                       "type" : "texture"}]
    }]]
  
  return {jsonstring}
end

function derp_components.aux.texture:spawn_inspector(component_data)
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