derp_components = {}
derp_components.aux = {name = "Auxiliary Blocks"}
derp_components.output = {name = "Output Blocks"}
derp_components.render = {name = "Render Blocks"}
derp_components.postprocess = {name = "Post-Process Blocks"}
--derp_components.postprocess = {}
--derp_components.output = {}


resources_counter = 0
net._send_file = net.send_file
function net.send_file(filepath)
  resources_counter = resources_counter + 1
  return net._send_file(filepath)
end


function path_abs2rel(abspath)
    cwd = app.getcwd()
    relpath = string.gsub(abspath, cwd, "")
    print(cwd)
    print(abspath)
    print(relpath)
    --return relpath -- this does not work. need a proper abs2rel
    return abspath
end

-------------------------------------------------------------------------------
-- Output::Simple
derp_components.output.simple = { name = "Ouput: Simple"
                                , tooltip = "Create a block that can render one texture on a remote machine."
                                }
function derp_components.output.simple:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 100, group = "output", type = "simple", output_type = "none", inputs = 1, outputs = {}, connections_in = {} }
  
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
		derp.active_workspace.profiling_data = {}
      
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
            print("got imgdata for block: '" .. block .. output .. "'")
            derp.active_workspace.preview_data[block .. output] = gfx.rawtexture(128, w,h,c, imgdata)
				derp.active_workspace.profiling_data[block] = indata:get_object(6)

            --self.client:disconnect()
          elseif indata.id == "rlog" then
            local sys, what, msg = indata:get_object(0), indata:get_object(1), indata:get_object(2)
            --print(sys, what, msg)
            msg = string.gsub(msg, "%d+", "^(1,0.6,0.6){%1}")
            print("[^(1,0.6,0.6){" .. tostring(sys) .."}] " .. tostring(msg))
            
          elseif indata.id == "ackres" then
            local remote_counter = indata:get_object(0)
            if (resources_counter <= remote_counter) then
              -- sending json to server
              print("Server has all resources, send JSON data.")
              print("JSON data to send: " .. self.final_pipeline)
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
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
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
  return derp:create_texturedinspector(component_data)
end



-------------------------------------------------------------------------------
-- PostProcess::Invert
derp_components.postprocess.invert = { name = "Post Process: Invert Colors"
                                , tooltip = "Create a block that inverts the colors of a texture."
                                }
function derp_components.postprocess.invert:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 140, h = 60, group = "postprocess", type = "invert", output_type = "texture", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  block.slidervalue = 0
  return block
end

function derp_components.postprocess.invert:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,1)
  
  function slider_update(self, value)
    self.parent.parent.parent.data.slidervalue = value
  end
  
  local sliderw = derp:create_slider(5,5,175,20,0,1, slider_update)
  sliderw:setvalue(component_data.slidervalue)
  wid.sliderw = sliderw
  wid:addwidget(sliderw)
  
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
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
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
                      float sv = ]] .. tostring(component_data.slidervalue+0.0001) .. [[;
                      vec4 o = texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                      vec4 i = vec4(1.0) - o;
                      vec4 c = i*sv + o*(1.0-sv); 
                      c.a = 1.0;
                    	gl_FragData[0] = c;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.invert:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end

-------------------------------------------------------------------------------
-- PostProcess::Sepia
derp_components.postprocess.sepia = { name = "Post Process: Sepia Filter"
                                , tooltip = "Create a block that applies a sepia filer to a given texture."
                                }
function derp_components.postprocess.sepia:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 140, h = 60, group = "postprocess", type = "sepia", output_type = "texture", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  return block
end

function derp_components.postprocess.sepia:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,1)
  return wid
end

function derp_components.postprocess.sepia:generate_json(component_data)
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
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
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
                      vec4 orig = texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                      vec4 c = vec4(0.0);
                      c.r = dot(orig, vec3(.393, .769, .189));
                      c.g = dot(orig, vec3(.349, .686, .168));
                      c.b = dot(orig, vec3(.272, .534, .131));
                      gl_FragData[0] = c;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.sepia:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end

-------------------------------------------------------------------------------
-- PostProcess::Blend
derp_components.postprocess.blend = { name = "Post Process: Blend Textures"
                                , tooltip = "Create a block that blend between two textures."
                                }
function derp_components.postprocess.blend:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 140, h = 60, group = "postprocess", type = "blend", output_type = "texture", inputs = 2, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  block.slidervalue = 0
  return block
end

function derp_components.postprocess.blend:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,1)
  
  function slider_update(self, value)
    self.parent.parent.parent.data.slidervalue = value
  end
  
  local sliderw = derp:create_slider(5,5,245,20,0,1, slider_update)
  sliderw:setvalue(component_data.slidervalue)
  wid.sliderw = sliderw
  wid:addwidget(sliderw)
  
  return wid
end

function derp_components.postprocess.blend:generate_json(component_data)
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
  local second_texture = nil
  local counter = 0
  for k,v in pairs(component_data.connections_in) do
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      if counter == 0 then
        first_texture = tostring(v.output)
      else
        second_texture = tostring(v.output)
      end
      counter = counter + 1
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
                      float sv = ]] .. tostring(component_data.slidervalue+0.0001) .. [[;
                      vec4 t1 = texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                      vec4 t2 = texture2D(]] .. tostring(second_texture) .. [[, gl_TexCoord[0].st);
                      vec4 c = t1*sv + t2*(1.0-sv); 
                      c.a = 1.0;
                      gl_FragData[0] = c;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.blend:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end

-------------------------------------------------------------------------------
-- PostProcess::Gaussian Blur
derp_components.postprocess.gaussianblur = { name = "Post Process: Gaussian Blur"
                                , tooltip = "Create a block that performs a gaussian blur filter on a texture."
                                }
function derp_components.postprocess.gaussianblur:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 150, h = 60, group = "postprocess", type = "gaussianblur", output_type = "texture", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  return block
end

function derp_components.postprocess.gaussianblur:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1,1)
	local valtable = { { 	name = "3x3",
							kernel_size = 9, 
							offset = { "vec2(-step_w, -step_h)", "vec2(0.0, -step_h)", "vec2(step_w, -step_h)",  
										"vec2(-step_w, 0.0)", "vec2(0.0, 0.0)", "vec2(step_w, 0.0)", 
										"vec2(-step_w, step_h)", "vec2(0.0, step_h)", "vec2(step_w, step_h)" },
							kernel = { 1.0/16.0, 2.0/16.0, 1.0/16.0,
										2.0/16.0, 4.0/16.0, 2.0/16.0,
										1.0/16.0, 2.0/16.0, 1.0/16.0}},
						{ 	name = "5x5",
							kernel_size = 25, 
							offset = {"vec2(-2*step_w,-2*step_h)", "vec2(-step_w,-2*step_h)","vec2(0.0,-2*step_h)","vec2(step_w,-2*step_h)","vec2(2*step_w,-2*step_h)",
										"vec2(-2*step_w,-step_h)", "vec2(-step_w,-step_h)","vec2(0.0,-step_h)","vec2(step_w,-step_h)","vec2(2*step_w,-step_h)",
										"vec2(-2*step_w,0.0)", "vec2(-step_w,0.0)","vec2(0.0,0.0)","vec2(step_w,0.0)","vec2(2*step_w,0.0)",
										"vec2(-2*step_w,step_h)", "vec2(-step_w,step_h)","vec2(0.0,step_h)","vec2(step_w,step_h)","vec2(2*step_w,step_h)",
										"vec2(-2*step_w,2*step_h)", "vec2(-step_w,2*step_h)","vec2(0.0,2*step_h)","vec2(step_w,2*step_h)","vec2(2*step_w,2*step_h)"}, 
							kernel = {1.0/256.0,4.0/256.0 ,6.0/256.0 ,4.0/256.0 ,1.0/256.0,
												4.0/256.0,16.0/256.0,24.0/256.0,16.0/256.0,4.0/256.0,
												6.0/256.0,24.0/256.0,36.0/256.0,24.0/256.0,6.0/256.0,
												4.0/256.0,16.0/256.0,24.0/256.0,16.0/256.0,4.0/256.0,
												1.0/256.0,4.0/256.0 ,6.0/256.0 ,4.0/256.0 ,1.0/256.0 } },
						{ 	name = "7x7",
							kernel_size = 49, 
							offset = {"vec2(-3*step_w,-3*step_h)","vec2(-2*step_w,-3*step_h)","vec2(-1*step_w,-3*step_h)","vec2(0.0,-3*step_h)","vec2(step_w,-3*step_h)","vec2(2*step_w,-3*step_h)","vec2(3*step_w,-3*step_h)",
										"vec2(-3*step_w,-2*step_h)","vec2(-2*step_w,-2*step_h)","vec2(-1*step_w,-2*step_h)","vec2(0.0,-2*step_h)","vec2(step_w,-2*step_h)","vec2(2*step_w,-2*step_h)","vec2(3*step_w,-2*step_h)",
										"vec2(-3*step_w,-1*step_h)","vec2(-2*step_w,-1*step_h)","vec2(-1*step_w,-1*step_h)","vec2(0.0,-1*step_h)","vec2(step_w,-1*step_h)","vec2(2*step_w,-1*step_h)","vec2(3*step_w,-1*step_h)",
										"vec2(-3*step_w,0.0)","vec2(-2*step_w,0.0)","vec2(-1*step_w,0.0)","vec2(0.0,0.0)","vec2(step_w,0.0)","vec2(2*step_w,0.0)","vec2(3*step_w,0.0)",
										"vec2(-3*step_w,step_h)","vec2(-2*step_w,step_h)","vec2(-1*step_w,step_h)","vec2(0.0,step_h)","vec2(step_w,step_h)","vec2(2*step_w,step_h)","vec2(3*step_w,step_h)",
										"vec2(-3*step_w,2*step_h)","vec2(-2*step_w,2*step_h)","vec2(-1*step_w,2*step_h)","vec2(0.0,2*step_h)","vec2(step_w,2*step_h)","vec2(2*step_w,2*step_h)","vec2(3*step_w,2*step_h)",
										"vec2(-3*step_w,3*step_h)","vec2(-2*step_w,3*step_h)","vec2(-1*step_w,3*step_h)","vec2(0.0,3*step_h)","vec2(step_w,3*step_h)","vec2(2*step_w,3*step_h)","vec2(3*step_w,3*step_h)"
										}, 
							kernel = { 1/4096,6/4096,15/4096,20/4096,15/4096,6/4096,1/4096,
												6/4096,36/4096,90/4096,120/4096,90/4096,36/4096,6/4096,
												15/4096,90/4096,225/4096,300/4096,225/4096,90/4096,15/4096,
												20/4096,120/4096,300/4096,400/4096,300/4096,120/4096,20/4096,
												15/4096,90/4096,225/4096,300/4096,225/4096,90/4096,15/4096,
												6/4096,36/4096,90/4096,120/4096,90/4096,36/4096,6/4096,
												1/4096,6/4096,15/4096,20/4096,15/4096,6/4096,1/4096}},
					}
  function slider_update(self,value)
	local blur = valtable[value + 1]
	self.parent.parent.parent.data.blurdesc = blur
  end
  
  local sliderw = derp:create_slider(5,5,150,20,0,2,slider_update,true)
  
  sliderw:setvalue(0)
  wid.data.blurdesc = valtable[1]
  
  wid.sliderw = sliderw
  wid:addwidget(sliderw)
  
  sliderw.mouseover = 
	function () 
			local x,y = inp.getmousepos()
			
			gui:set_tooltip(wid.data.blurdesc.name,x,y) 
	end
  
  return wid
end

function derp_components.postprocess.gaussianblur:generate_json(component_data)
  local final_jsondata = {}
  local input_array = {}
  local input_array_shader = {}
  
	function build_blursource(tex) 
		local str = ""

		--for k,v in pairs(component_data.blurdesc.offset) do
		for i = 1,#component_data.blurdesc.offset do
			str = str .. "sum += texture2D(" .. tex .. ",uv + " .. component_data.blurdesc.offset[i] .. ") * " .. component_data.blurdesc.kernel[i] .. ";\n"
		end

		return str
	end
	
	function build_fromtable(tbl)
		str = ""
		--for k,v in pairs(tbl) do
		for i = 1, #tbl do
			if i == #tbl then
				str = str .. tbl[i]
			else
				str = str .. tbl[i] .. ","
			end
		end
		
		return str
	end
  
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
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
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
                    "shaderVert" : "#version 120
					]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    uniform float script1;
                    void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
					
										
					float step_w = 1.0 / 512.0;
					float step_h = 1.0 / 512.0;
					
					#define KERNEL_SIZE ]] .. tostring(component_data.blurdesc.kernel_size) .. [[
					
					vec2 offset[KERNEL_SIZE] = { ]] .. build_fromtable(component_data.blurdesc.offset) .. [[ };
					
					float kernel[KERNEL_SIZE] = float[] ( ]] .. build_fromtable(component_data.blurdesc.kernel) .. [[ ); 

					void main()
					{
						vec4 sum = vec4(0.0);
						vec2 uv = gl_TexCoord[0].st;
						
						]] .. build_blursource(tostring(first_texture)) .. [[
									
						gl_FragData[0] = sum;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.gaussianblur:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)

end

-------------------------------------------------------------------------------
-- PostProcess::ToneMap
derp_components.postprocess.tonemap = { name = "Post Process: Tone Mapping"
                                      , tooltip = "Create a block that performs tone mapping using a LUT texture."
                                      }
function derp_components.postprocess.tonemap:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 140, h = 60, group = "postprocess", type = "tonemap", output_type = "texture", inputs = 2, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  return block
end

function derp_components.postprocess.tonemap:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,2,1)
  wid.input_aliases = {"LUT texture"}
  --wid.output_aliases = {"diffuse", "normals", "depth"}
  
  return wid
end

function derp_components.postprocess.tonemap:generate_json(component_data)
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
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
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
                    void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    void main()
                    {
                      vec4 c = texture2D(]] .. tostring(component_data.connections_in[2].output) .. [[, gl_TexCoord[0].st);
                      vec2 lut_coord = vec2(c.r / 16.0 + (floor(c.b * 16.0) / 16.0), c.g);
                      vec4 lut_c = texture2D(]] .. tostring(component_data.connections_in[1].output) .. [[, lut_coord.st);
                      lut_c.a = c.a;
                    	gl_FragData[0] = lut_c;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.postprocess.tonemap:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end


-------------------------------------------------------------------------------
-- Renderer::geometry
derp_components.render.geometry = { name = "Geometry renderer - Blinn-Phong"
                              , tooltip = "Create a block that inputs geometry and renders to a texture."
                              }
function derp_components.render.geometry:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "render", type = "geometry", output_type = "texture", inputs = 6, outputs = { workspace:gen_new_outputname() }, connections_in = {}}
  -- specific values
  block.modelfilepath = ""
  return block
end

function derp_components.render.geometry:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1000,1)
  wid.input_aliases = {"cameraPos", "cameraLookAt", "lightPos", "lightColor"}
  wid.output_aliases = {"diffuse"}
  
  function toggle_wireframe(self,state)
	if state then
		wid.data.draw_wireframe = 1
	else
		wid.data.draw_wireframe = 0
	end
  end
  
  local checkbox = gui:create_checkbox(20,20,20,"wireframe",toggle_wireframe)
  wid.data.draw_wireframe = 0
  
  wid:addwidget(checkbox)
  
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
  local first_geometry = nil
  for k,v in pairs(component_data.connections_in) do
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)
    elseif (tdata.output_type == "vec3") then
      table.insert(input_array_shader, "uniform vec3 " .. tostring(v.output) .. ";")
    elseif (tdata.output_type == "geometry") then
      first_geometry = tostring(v.output)
    end
  end

  if (first_texture == nil) then
    return spawn_error_dialog({"Geometry block needs at least one texture!"})
  end
  
  if (first_geometry == nil) then
    return spawn_error_dialog({"Geometry block needs at least one geometry block!"})
  end
  
  local light_pos = tostring(component_data.connections_in[3].output)
  local light_color = tostring(component_data.connections_in[4].output)

  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "Render",
     "blockInput" : []] .. tostring(table.concat(input_array, ",\n")) .. [[],
     "blockData" : {"drawMode" : ]] .. tostring(component_data.draw_wireframe) .. [[,
					"width" : 512,
                    "height" : 512,
  						"cameraPosition" : "]] .. tostring(component_data.connections_in[1].output) .. [[",
  						"cameraLookAt" : "]] .. tostring(component_data.connections_in[2].output) .. [[",
  						"cameraFov" : 45.0,
                    "shaderVert" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
                    varying vec3 lightdir, epos;
  						      void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  							      n = gl_NormalMatrix * gl_Normal;
  							      epos = -vec4(gl_ModelViewMatrix * gl_Vertex).xyz;
  							      lightdir = vec4(gl_ModelViewMatrix * vec4(]] .. light_pos .. [[, 1.0)).xyz + epos;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
                    varying vec3 lightdir, epos;
                    void main()
                    {
                      float NdotL = max(dot(normalize(n), normalize(lightdir)), 0.0);
                      vec4 color;
                      color = NdotL * vec4(]] .. light_color .. [[, 1.0) * texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                      if (NdotL > 0.0)
                      {
                        vec3 halvvec = normalize(normalize(epos) + normalize(lightdir));
                        float NdotHV = max(dot(normalize(n),halvvec),0.0);
                        
                        color = color + pow(NdotHV, 124.0) * vec4(]] .. light_color .. [[, 1.0);
                      }
                      color.a = 1.0;
  							      gl_FragData[0] = color;
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.render.geometry:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end

-------------------------------------------------------------------------------
-- Renderer::geometryadvanced
derp_components.render.geometryadvanced = { name = "Geometry renderer - Advanced"
                              , tooltip = "Create a block that inputs geometry and renders to a texture, also outputs depth and normal textures."
                              }
function derp_components.render.geometryadvanced:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "render", type = "geometryadvanced", output_type = "texture", inputs = 6, outputs = { workspace:gen_new_outputname(), workspace:gen_new_outputname(), workspace:gen_new_outputname() }, connections_in = {}}
  -- specific values
  block.modelfilepath = ""
  return block
end

function derp_components.render.geometryadvanced:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data,1000,3)
  wid.input_aliases = {"cameraPos", "cameraLookAt", "lightPos", "lightColor"}
  wid.output_aliases = {"diffuse", "normals", "depth"}
  
  function toggle_wireframe(self,state)
	if state then
		wid.data.draw_wireframe = 1
	else
		wid.data.draw_wireframe = 0
	end
  end
  
  local checkbox = gui:create_checkbox(20,20,20,"wireframe",toggle_wireframe)
  wid.data.draw_wireframe = 0
  
  wid:addwidget(checkbox)
  
  return wid
end

function derp_components.render.geometryadvanced:generate_json(component_data)

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
  local first_geometry = nil
  for k,v in pairs(component_data.connections_in) do
    local tdata = derp.active_workspace:get_block(v.block).data
    if (tdata.output_type == "texture") then
      table.insert(input_array_shader, "uniform sampler2D " .. tostring(v.output) .. ";")
      first_texture = tostring(v.output)
    elseif (tdata.output_type == "vec3") then
      table.insert(input_array_shader, "uniform vec3 " .. tostring(v.output) .. ";")
    elseif (tdata.output_type == "geometry") then
      first_geometry = tostring(v.output)
    end
  end

  if (first_texture == nil) then
    return spawn_error_dialog({"Geometry block needs at least one texture!"})
  end
  
  if (first_geometry == nil) then
    return spawn_error_dialog({"Geometry block needs at least one geometry block!"})
  end
  
  local light_pos = tostring(component_data.connections_in[3].output)
  local light_color = tostring(component_data.connections_in[4].output)

  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "Render",
     "blockInput" : []] .. tostring(table.concat(input_array, ",\n")) .. [[],
     "blockData" : {"drawMode" : ]] .. tostring(component_data.draw_wireframe) .. [[,
					"width" : 512,
                    "height" : 512,
  						"cameraPosition" : "]] .. tostring(component_data.connections_in[1].output) .. [[",
  						"cameraLookAt" : "]] .. tostring(component_data.connections_in[2].output) .. [[",
  						"cameraFov" : 45.0,
                    "shaderVert" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
                    varying vec3 lightdir, epos;
  						      void main(void)
                    {
                    	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  							      n = gl_NormalMatrix * gl_Normal;
  							      epos = -vec4(gl_ModelViewMatrix * gl_Vertex).xyz;
  							      lightdir = vec4(gl_ModelViewMatrix * vec4(]] .. light_pos .. [[, 1.0)).xyz + epos;
                    	gl_TexCoord[0] = gl_MultiTexCoord0;
                    }",
                    "shaderFrag" : "]] .. tostring(table.concat(input_array_shader, "\n")) .. [[
                    varying vec3 n;
                    varying vec3 lightdir, epos;
                    void main()
                    {
  							      float NdotL = max(dot(normalize(n), normalize(lightdir)), 0.0);
                      vec4 color;
                      color = NdotL * vec4(]] .. light_color .. [[, 1.0) * texture2D(]] .. tostring(first_texture) .. [[, gl_TexCoord[0].st);
                      if (NdotL > 0.0)
                      {
                        vec3 halvvec = normalize(normalize(epos) + normalize(lightdir));
                        float NdotHV = max(dot(normalize(n),halvvec),0.0);
                        
                        color = color + pow(NdotHV, 124.0) * vec4(]] .. light_color .. [[, 1.0);
                      }
                      color.a = 1.0;
  							      gl_FragData[0] = color;
  							      gl_FragData[1] = vec4(n, 1.0);
  							      gl_FragData[2] = vec4(0,0,0, 1.0);
  							      
                    }"
                   },
     "blockOutput" : [ {"name" : "]] .. tostring(component_data.outputs[1]) .. [[", "type" : "texture"},
                       {"name" : "]] .. tostring(component_data.outputs[2]) .. [[", "type" : "texture"},
                       {"name" : "]] .. tostring(component_data.outputs[3]) .. [[", "type" : "texture"}]
    }]]
  
  table.insert(final_jsondata, escape_backslashes(jsonstring))
  
  return final_jsondata
end

function derp_components.render.geometryadvanced:spawn_inspector(component_data)
  return derp:create_texturedinspector(component_data)
end



-------------------------------------------------------------------------------
-- Aux::model (constant)
derp_components.aux.model = { name = "CTM Model"
                              , tooltip = "Create a block that outputs model geometry."
                              }
function derp_components.aux.model:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "aux", type = "model", output_type = "geometry", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
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
      self.parent.parent.parent.data.modelfilepath = path_abs2rel(new_filepath)

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
  return nil
end

-------------------------------------------------------------------------------
-- Aux::texture (constant)
derp_components.aux.texture = { name = "Texture"
                                , tooltip = "Create a block that outputs static texture."
                              }
function derp_components.aux.texture:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 170, h = 60, group = "aux", type = "texture", output_type = "texture", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
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
      self.parent.parent.parent.filepathwidget.label_text = path_abs2rel(new_filepath)
      self.parent.parent.parent.data.texturefilepath = path_abs2rel(new_filepath)

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
  return nil
end


-------------------------------------------------------------------------------
-- Aux::float (constant)
derp_components.aux.floatconstant = { name = "Constant: Float"
                                    , tooltip = "Create a block that outputs a constant float value."
                                    }
function derp_components.aux.floatconstant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 150, h = 40, group = "aux", type = "floatconstant", output_type = "float", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
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
  return nil
end

function derp_components.aux.floatconstant:spawn_inspector(component_data)
  return nil
end


-------------------------------------------------------------------------------
-- Aux::float (constant)
derp_components.aux.vec2constant = { name = "Constant: Vec2"
                                    , tooltip = "Create a block that outputs a constant vec2 value."
                                    }
function derp_components.aux.vec2constant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 100, h = 30, group = "aux", type = "vec2constant", output_type = "vec2", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.constvalue = {0.0, 0.0}
  
  return block
end

function derp_components.aux.vec2constant:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
                                             
  
  return wid
end

function derp_components.aux.vec2constant:generate_json(component_data)
  return nil
end

function derp_components.aux.vec2constant:spawn_inspector(component_data)
  return nil
end

-------------------------------------------------------------------------------
-- Aux::float (slider)
derp_components.aux.floatslider = { name = "Slider: float"
                                    , tooltip = "Create a block that outputs a value depending on slider value"
                                    }
function derp_components.aux.floatslider:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 100, h = 60, group = "aux", type = "floatslider", output_type = "float", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.constvalue = "0.0"
  
  return block
end

function derp_components.aux.floatslider:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  
  function on_change_textinput(self, value)
    self.parent.parent.parent.data.constvalue = value
	self.parent.parent.parent.valslider:setvalue(tonumber(value))
	self.parent.parent.parent.valinput.value = tostring(self.parent.parent.parent.valslider.value)
  end
  
  function on_change_slider(self, value)
    self.parent.parent.parent.valinput.value = string.sub(tostring(value), 1, 4)
	--self.parent.parent.parent.data.constvalue = value	
  end
  
  local vallabel = gui:create_labelpanel(5      , 10 , 8*8     , 20, "Value:")
  local valinput = gui:create_textinput (10+8*7 , 10 , 100-8*7 , false, component_data.constvalue, on_change_textinput)
  local valslider   = derp:create_slider(5      , 35 , 110     , 20, 0 , 1, on_change_slider)
  wid.vallabel = vallabel
  wid.valinput = valinput
  wid.valslider = valslider
  wid:addwidget(vallabel)
  wid:addwidget(valinput)
  wid:addwidget(valslider)
  
  valinput.value = tostring(valslider.value)
  
  return wid
end

function derp_components.aux.floatslider:generate_json(component_data)
  return nil
end

function derp_components.aux.floatslider:spawn_inspector(component_data)
  return nil
end

-------------------------------------------------------------------------------
-- Aux::vec3 (constant)
derp_components.aux.vec3script = { name = "Script: Vec3"
                                    , tooltip = "Create a block that outputs a script that returns a vec3 value."
                                    }
function derp_components.aux.vec3script:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 200, h = 30, group = "aux", type = "vec3script", output_type = "vec3", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.script = ""
  
  return block
end

function derp_components.aux.vec3script:create_widget(component_data)
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

function derp_components.aux.vec3script:generate_json(component_data)
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

function derp_components.aux.vec3script:spawn_inspector(component_data)
  return nil
end

-------------------------------------------------------------------------------
-- Aux::vec3 (constant)
derp_components.aux.vec3constant = { name = "Constant: Vec3"
                                    , tooltip = "Create a block that outputs a script that returns a vec3 value."
                                    }
function derp_components.aux.vec3constant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 200, h = 85, group = "aux", type = "vec3constant", output_type = "vec3", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
  -- specific values
  block.vals = {"", "", ""}
  
  return block
end

function derp_components.aux.vec3constant:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  
  -- script input
  function script_changed(self)
  	 self.parent.parent.parent.data.vals[self.ident] = self.value
    print("vec3." .. self.ident .. " changed to: " .. self.value)
    derp:push_active_workspace()
  end
  local val1label = gui:create_labelpanel(5,10,8*8,20,"Val1:")
  local val2label = gui:create_labelpanel(5,35,8*8,20,"Val2:")
  local val3label = gui:create_labelpanel(5,60,8*8,20,"Val3:")
  local val1input = gui:create_textinput(10+8*8,10,180-8*8,false,component_data.vals[1],script_changed)
  local val2input = gui:create_textinput(10+8*8,35,180-8*8,false,component_data.vals[2],script_changed)
  local val3input = gui:create_textinput(10+8*8,60,180-8*8,false,component_data.vals[3],script_changed)
  val1input.ident = 1
  val2input.ident = 2
  val3input.ident = 3

  wid.val1input = val1input
  wid:addwidget(val1label)
  wid:addwidget(val2label)
  wid:addwidget(val3label)
  wid:addwidget(val1input)
  wid:addwidget(val2input)
  wid:addwidget(val3input)
  
  return wid
end

function derp_components.aux.vec3constant:generate_json(component_data)
  local jsonstring = [[{"blockName" : "]] .. tostring(component_data.id) .. [[",
     "blockType" : "AuxComp",
     "blockData" : {"auxType" : "script",
                    "src" : "return ]] .. tostring(table.concat(component_data.vals, ",")) .. [["
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
  return nil
end
