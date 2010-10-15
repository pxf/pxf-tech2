derp_components = {}
derp_components.aux = {name = "Auxiliary Blocks"}
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
-- Aux::float (constant)
derp_components.aux.floatconstant = { name = "Constant: Float"
                                    , tooltip = "Create a block that outputs a constant float value."
                                    }
function derp_components.aux.floatconstant:new_block(workspace,x,y)
  local block = { x = x, y = y, w = 100, h = 100, group = "aux", type = "floatconstant", inputs = 1, outputs = { workspace:gen_new_outputname() } }
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
  local block = { x = x, y = y, w = 100, h = 100, group = "aux", type = "vec2constant", inputs = 0, outputs = { workspace:gen_new_outputname() } }
  
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