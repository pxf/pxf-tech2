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
  local block = { x = x, y = y, w = 120, h = 150, group = "output", type = "simple", inputs = 1, outputs = {}, connections_in = {} }
  
  -- specific values
  block.remotehost = "localhost"
  
  return block
end

function derp_components.output.simple:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  
  -- host input
  local hostinput = gui:create_textinput(10,70,100,false,component_data.remotehost)
  wid.hostinput = hostinput
  wid:addwidget(hostinput)
  
  -- render button
  local renderbutton = gui:create_labelbutton(10,100,100,30,"Render", function(...) print(...) end)
  wid.renderbutton = renderbutton
  wid:addwidget(renderbutton)
  
  return wid
end

function derp_components.output.simple:generate_json(component_data)
  return "LOL TODO"
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
  local block = { x = x, y = y, w = 100, h = 100, group = "aux", type = "floatconstant", inputs = 1, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
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
  local block = { x = x, y = y, w = 170, h = 150, group = "aux", type = "texture", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  -- specific values
  block.texturefilepath = ""
  return block
end

function derp_components.aux.texture:create_widget(component_data)
  local wid = derp:create_basecomponentblock(component_data)
  
  -- filepath
  local filepathwidget = gui:create_centeredlabelpanel(0,70,170,50,component_data.texturefilepath)
  wid.filepathwidget = filepathwidget
  wid:addwidget(filepathwidget)
  
  -- browse button
  local browsebutton = gui:create_labelbutton(10,100,150,30,"Browse", function(self)
                                                                        local new_filepath = app.opendialog()
                                                                        if (new_filepath) then
                                                                          local shortname = string.sub(new_filepath,-15)
                                                                          if (#shortname < #new_filepath) then
                                                                            shortname = "..." .. shortname
                                                                          end
                                                                          self.parent.filepathwidget.label_text = shortname
                                                                          self.parent.data.texturefilepath = new_filepath
                                                                        end
                                                                      end)
  wid.browsebutton = browsebutton
  wid:addwidget(browsebutton)
  
  return wid
end

function derp_components.aux.texture:generate_json(component_data)
  return "LOL TODO"
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
  local block = { x = x, y = y, w = 100, h = 100, group = "aux", type = "vec2constant", inputs = 0, outputs = { workspace:gen_new_outputname() }, connections_in = {} }
  
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