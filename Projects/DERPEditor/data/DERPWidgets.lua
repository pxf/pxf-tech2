require("data/DERPComponents")

derp = {}
derp.active_workspace = nil
derp.active_tool = { last = nil, current = nil }
derp.ws_menu = nil

function derp:init()
	self.ws_menu = derp:create_workspace_menu()
	self.ready = true
end

function derp:printstack()
	print("\n-------------\n")
	for k,v in pairs(self.active_workspace.workspace_stack.stack) do
		print("k == " .. k)
		print(v)
		print("\n-------------\n")
	end
end

function derp:print_activedata()
	local ws = basic_serialize(self.active_workspace.component_data,0)
	
	print(ws)
end	

function derp:push_workspace(ws)	
	ws.workspace_stack.counter = ws.workspace_stack.counter + 1
	
	for i = ws.workspace_stack.counter, #ws.workspace_stack.stack do
		self:pop_workspace(ws)
	end
	
	table.insert(ws.workspace_stack.stack,basic_serialize(ws.component_data,0))
end

function derp:pop_workspace(ws)
	-- remove item from tail
	table.remove(ws.workspace_stack.stack)
end

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

function derp:save(filename,workspace)
	-- save current workspace
	local fname = filename-- .. ".derp"
	local file = io.output(fname,"w")
	local data = basic_serialize(workspace.component_data,0)
	
	print("saving to file: " .. fname)
	
	io.write(data)
	
	file:close()
end

function derp:load(filename)
	-- load from file
	local file = io.input(filename .. ".derp","r")
	local retval = nil
	
	if file ~= nil then
		local str = io.read("*all")
		retval = loadstring("return" .. str)()
	else
		error("Unable to open file " .. filename)
	end
	
	file:close()
	
	return retval
end

function derp:set_activetool(tool)
	if tool and not tool.toggle then
		return
	end

	if self.active_tool.current then
		self.active_tool.last = self.active_tool.current
		
		self.active_tool.current.selected = false
		self.active_tool.current:needsredraw()
		self.active_tool.current = tool
	else
		self.active_tool.current = tool
	end
	
	if tool then
		tool.selected = true
	end
end

function derp:create_menu(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	wid.widget_type = "menu"
	
	wid.super_draw = wid.draw
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h, 1, 1, 1, 1)
			
			-- DRAW BOTTOM BORDER
			gfx.drawtopleft(self.drawbox.x + 20,self.drawbox.y + self.drawbox.h-1,self.drawbox.w - 40,1,1,5,1,1)
		
			self:super_draw(force)
		end
	end
	
	return wid
end

function derp:create_statusbar(x,y,w,h)
	local wid = gui:create_statusbar(x,y,w,h, editor.name .. " v" .. editor.version)--create_basewidget(x,y,w,h)
	wid.widget_type = "statusbar"
	
	wid.sdraw = wid.draw
	function wid:draw(force) 
		if (self.redraw_needed or force) then
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,1,1,1)	
			
			-- DRAW BORDER
			gfx.drawtopleft(self.drawbox.x+20,self.drawbox.y,self.drawbox.w-40,1,1,5,1,1)
			
			self:sdraw(force)
		end
	end
	
	return wid
end

function derp:create_inspector(x,y,w,h)
	local wid = gui:create_verticalstack(x,y,w,h)
	wid.widget_type = "inspector"
	wid.super_draw = wid.draw
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,9,1,1,1)
			gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+1,self.drawbox.w-2,self.drawbox.h-2,1,1,1,1)
		
			-- DRAW BORDER
			-- TOP
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y,self.drawbox.w-2,1,1,5,1,1)
			-- BOTTOM
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y + self.drawbox.h-2,self.drawbox.w-2,1,1,5,1,1)
			-- LEFT
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
			-- RIGHT
			gfx.drawtopleft(self.drawbox.x + self.drawbox.w-2, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
		
			wid:super_draw(force)
		end
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
		self.drawbox.y = self.drawbox.y + h
	end
	
	return wid
end

function RGB_to_HSV(r,g,b,h,s,v)
	local out = {r,g,b}
	local VSU = v * s * math.cos(h * math.pi/180)
	local VSW = v * s * math.sin(h * math.pi/180)

	out.r = (.299*v+.701*VSU+.168*VSW)*r
        + (.587*v-.587*VSU+.330*VSW)*g
        + (.114*v-.114*VSU-.497*VSW)*b
	out.g = (.299*v-.299*VSU-.328*VSW)*r
        + (.587*v+.413*VSU+.035*VSW)*g
        + (.114*v-.114*VSU+.292*VSW)*b
	out.b = (.299*v-.3*VSU+1.25*VSW)*r
        + (.587*v-.588*VSU-1.05*VSW)*g
        + (.114*v+.886*VSU-.203*VSW)*b
	
	return out.r,out.g,out.b
end

function derp:create_slider(x,y,w,min,max)
	local wid = gui:create_basewidget(x,y,w,30)
	local slide_button = gui:create_basewidget(x + w*0.5,y + 15,10,15)
	
	wid.value = (max - min) * 0.5
	
	--print(wid.value)
	
	wid:addwidget(slide_button)
	
	function slide_button:mousedrag(dx,dy)
		if self.drawbox.x + dx > wid.drawbox.x and self.drawbox.x + dx < wid.drawbox.x + wid.drawbox.w then
			self:needsredraw()
			self:move_relative(dx,0)
			
			self.hitbox.x = self.drawbox.x - self.drawbox.w * 0.5
			self.hitbox.y = self.drawbox.y - self.drawbox.h * 0.5
			
			self:needsredraw()
		else
			--self:move_abs(wid.drawbox.x, self.drawbox.y)
		
		end
		
		local step = wid.drawbox.w / (max - min)
		local pos = (self.drawbox.x - wid.drawbox.x) / wid.drawbox.w
		
		wid.value = pos * (max - min)
	end
	
	function slide_button:draw(force)
		if (self.redraw_needed or force) then
			gfx.drawcentered(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,5,5,1,1) -- top
		end
	end
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y+self.drawbox.h * 0.5,self.drawbox.w,1,5,5,1,1) -- top	
			gui:drawfont(tostring(self.value), self.drawbox.x + self.drawbox.w + 2, self.drawbox.y)
		end
		
		slide_button:draw(force)
	end
	
	return wid
end

function derp:create_workspace_menu()
  local component_menu = {}
  for group,types in pairs(derp_components) do
    
    local type_menu = {}
    for blocktype,v in pairs(types) do
      if not (blocktype == "name") then
        table.insert(type_menu, {tostring(v.name), {tooltip = "lol sup", 
onclick = function()
                                                                                     local x,y = inp.getmousepos()
                                                                                     self.active_workspace:addcomponent(v:new_block(x,y))
                                                                                   end
                                                   }
                                }
                    )
      end
    end
    
    local sub_menu = {tostring(types.name), {menu = type_menu}}
    table.insert(component_menu, sub_menu)
  end
  local menu = {{"New component", {menu = component_menu}}}
  
	--[[local menu = {
		{"New Component", 
			{
				menu = {
					{"Aux", { onclick = function () 
								local x,y = self.active_workspace.cam:coord_transform(inp.getmousepos())
								self.active_workspace:addcomponent(x,y,"aux") 
							end, tooltip = "Creates a new Aux component"}},
					{"Render", { onclick = function () 
								local x,y = self.active_workspace.cam:coord_transform(inp.getmousepos())
								self.active_workspace:addcomponent(x,y,"render") 
							end, tooltip = "Creates a new Render component"}},
					{"Output", { onclick = function () 
								local x,y = self.active_workspace.cam:coord_transform(inp.getmousepos())
								self.active_workspace:addcomponent(x,y,"output") 
							end, tooltip = "Creates a new Output component"}}
				}
			}
	   }
	}]]
			
	local wid = gui:create_menu(0,0,menu)
	
	wid.super_menu = wid.menu
	wid.widget_type = "workspace menu"
	wid.visible = false
	
	function wid:show()
		if self.visible then
			self:hide()
		end
		
		local x,y = inp.getmousepos()
		self:move_abs(x,y)
			
		self.visible = true
		gui.widgets:addwidget(self)
		
		derp:set_activetool(nil)
	end
	
	function wid:hide()
		self.visible = false
		gui.widgets:removewidget(self)
		
		gui.focuswidget = derp.active_workspace.cam
		derp:set_activetool(derp.active_tool.last)
	end
	
	function wid:destroy_callback()
		self:hide()
	end
	
	return wid
end

function derp:create_workspace_tabs(x,y,w,h,workspace)
	local wid = gui:create_horizontalstack(x,y,w,h)
	wid.widget_type = "workspace tabs"
	wid.super_draw = wid.draw
	wid.workspace = workspace
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			-- DRAW BG
			gfx.drawtopleft(#self.childwidgets*100 + self.drawbox.x + 1,self.drawbox.y,self.drawbox.w-1 - #self.childwidgets*100,self.drawbox.h,9,5,1,1)
			
			wid:super_draw(force)
		end
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.y = self.drawbox.y + h
	end
	
	function wid:addtab(name)
		local ws = gui:create_basewidget(0,0,100,20)
		ws.widget_type = "workspace tab " .. #self.childwidgets
		ws.parent = self
		ws.super_find_mousehit = ws.find_mousehit
		
		function ws:draw(force)
			if (self.redraw_needed or force) then
				
				-- DRAW BG
				gfx.drawtopleft(self.drawbox.x,0,self.drawbox.w,self.drawbox.h,9,1,1,1)
				gfx.drawtopleft(self.drawbox.x + 1,0,self.drawbox.w-1,self.drawbox.h,508,0,1,128)

				
				-- DRAW BORDERS
				gfx.drawtopleft(self.drawbox.x + 1,0,self.drawbox.w-1,1,1,5,1,1) -- TOP
				gfx.drawtopleft(self.drawbox.x + 1,self.drawbox.h-1,self.drawbox.w-1,1,1,5,1,1) -- BOTTOM
				gfx.drawtopleft(self.drawbox.x + 1,0,1,self.drawbox.h-1,1,5,1,1) -- LEFT
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w-1,0,1,self.drawbox.h-1,1,5,1,1) -- RIGHT
			end
		end
		
		self:addwidget(ws)
		
		return ws
	end
	
	return wid
end

function derp:create_navigator(x,y,w,h)
	local wid = gui:create_basewidget(x,y,w,h)
	local cam = derp.active_workspace.cam				
	local ws_aspectratio = 581 / 476
	local da_w = w*0.9
	local da_h = 0.9 * h / ws_aspectratio
	
	local draw_area = gui:create_basewidget((wid.drawbox.w - da_w) / 2,(wid.drawbox.h - da_h) / 2,da_w,da_h)
	draw_area.x_step = cam.drawbox.w / draw_area.drawbox.w 
	draw_area.y_step = cam.drawbox.h / draw_area.drawbox.h
	
	draw_area.navbox = {
					x = - (cam.drawbox.x / draw_area.x_step),
					y = -(cam.drawbox.y / draw_area.y_step),
					w = (581 / cam.drawbox.w) * draw_area.drawbox.w,
					h = (476 / cam.drawbox.h) * draw_area.drawbox.h  }
	
	wid.draw_area = draw_area
	wid:addwidget(draw_area)
	
	function draw_area:draw(force)
		if (self.redraw_needed or force) then
			gfx.translate(wid.drawbox.x,wid.drawbox.y)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,5,1,1,1) -- solid bg
			
			gfx.translate(self.drawbox.x,self.drawbox.y)
			local a = gfx.getalpha()
			gfx.setalpha(0.6)
			gfx.drawtopleft(self.navbox.x,self.navbox.y,self.navbox.w,self.navbox.h,1,5,1,1) -- solid bg
			--gfx.drawtopleft(self.navbox.x,self.navbox.y,1,self.navbox.h,1,5,1,1) -- solid bg
			--gfx.drawtopleft(self.navbox.x + self.navbox.w,self.navbox.y,1,self.navbox.h,1,5,1,1) -- solid bg
			--gfx.drawtopleft(self.navbox.x,self.navbox.y + self.navbox.h,self.navbox.w,1,1,5,1,1) -- solid bg
			gfx.setalpha(a)
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
			gfx.translate(-wid.drawbox.x,-wid.drawbox.y)
		end
	end
	
	function draw_area:calc_navbox()
		-- calculate position from cam
		local x = - ( (cam.drawbox.x - 20) / draw_area.x_step)
		local y = - ( (cam.drawbox.y - 102) / draw_area.y_step)
		
		self.navbox.x = x
		self.navbox.y = y
		
		print(x,y)
	end
	
	function draw_area:mousepush(x,y)
		-- determine where
		local rel_x = x - self.drawbox.x - wid.drawbox.x
		local rel_y = y - self.drawbox.y - wid.drawbox.y
		
		local mx = rel_x * self.x_step
		local my = rel_y * self.y_step
		
		if mx > cam.drawbox.w - 752 then
			mx = cam.drawbox.w-752
		end
		
		if my > cam.drawbox.h - 576 then
			my = cam.drawbox.h - 576
		end
		
		cam:move_abs(-mx,-my)
		self:calc_navbox()
	end
	
	function draw_area:mousedrag(dx,dy)
		local x = -dx * self.x_step
		local y = -dy * self.y_step
	
		cam:move_relative(x,y)
		self:calc_navbox()
	end
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,1,1,1) -- solid bg
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,1,1,5,1,1) -- solid bg
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,1,self.drawbox.h,1,5,1,1) -- solid bg
		end
		
		draw_area:draw(force)
	end
	
	self.navigator = wid
	
	return wid
end

function new_socket(comp,type,socket)
	return {comp = comp, type = type, socket = socket}
end

function derp:create_workspacecamera(x,y,w,h)
	local cam = gui:create_basewidget(x,y,w*2,h*2)
	cam.widget_type = "workspace camera"
	cam.bounds = { left = -w, right = w, top = h, bottom = -h}
	cam.super_draw = cam.draw
	cam.super_mhit = cam.find_mousehit
	local checkers_texture = gfx.loadtexture(64,"data/checkers.png")
	
	function cam:coord_transform(x,y) 
		return x - self.drawbox.x - self.drawbox.w * 0.5,y - self.drawbox.y - self.drawbox.h * 0.5
	end	
	
	function cam:coord_transform_inv(x,y)
		return x + self.drawbox.x + self.drawbox.w * 0.5,y + self.drawbox.y + self.drawbox.h * 0.5
	end
	
	cam.socket_a = nil
	cam.comp_drag = false
	
	cam.shortcuts = { 	{ name = "copy", keys = {inp.LCTRL, "C"}, was_pressed = false, onpress = function () print("Lets copy dat floppy!") end},
						{ name = "deselect", keys = {inp.ESC}, was_pressed = false, onpress = function () derp.active_workspace:set_activecomp(nil) end },
						{ name = "select move", keys = {"A"}, was_pressed = false, onpress = function () derp:set_activetool(move_select) end },
						{ name = "print data", keys = {"P"},was_pressed = false, onpress = function () derp:print_activedata() end },
						{ name = "ctrl select move", keys = {inp.LCTRL}, mouse = {inp.MOUSE_LEFT}, was_pressed = false, 
							onpress = function () 
								local mx,my = cam:coord_transform(inp.getmousepos())
								derp.active_tool.current:action({tag = "ctrl select", x = mx, y = my})
							end},
						{ name = "square select", keys = {"M"}, was_pressed = false, onpress = function () derp:set_activetool(select_rect) end },
						{ name = "show ws menu", mouse = { inp.MOUSE_RIGHT }, was_pressed = false, 
							onpress = function ()
								derp.ws_menu.menu = derp.ws_menu.super_menu	
								derp.ws_menu:show() 
							end},
						{ name = "deconnect components", keys = {inp.LSHIFT}, mouse = {inp.MOUSE_RIGHT}, was_pressed = false, 
							onpress = function () 
								derp:set_activetool(nil)
								cam.socket_a = derp.active_workspace:socket_hittest(cam:coord_transform(inp.getmousepos()))
								
								if cam.socket_a then								
									-- find all connections from this socket
									local menu = { }
									
									for k,v in pairs(derp.active_workspace.component_data.edges) do
										if (v[1].comp == cam.socket_a.comp and v[1].socket == cam.socket_a.socket and v[1].type == cam.socket_a.type) or
											(v[2].comp == cam.socket_a.comp and v[2].socket == cam.socket_a.socket and v[2].type == cam.socket_a.type) then
											
											local tag = "connection " .. v[1].comp.id .. "." .. v[1].socket .. " to " .. v[2].comp.id .. "." .. v[2].socket
											table.insert(menu,{tag, { onclick = 
												function () 
													v[1].comp.outputs[v[1].socket] = v[1].comp.outputs[v[1].socket] - 1
													v[2].comp.inputs[v[2].socket] = v[2].comp.inputs[v[2].socket] - 1
													
													derp.active_workspace.component_data.edges[k] = nil
													
													derp:push_workspace(derp.active_workspace)
												end}})
										end
										
									end
									
									if #menu > 0 then
										derp.ws_menu.menu = menu
									end
									
									cam.socket_a = nil
								end
							end,
							onrelease = function ()
								derp:set_activetool(derp.active_tool.last)
							end},
						{ name = "shift drag", keys = {inp.LSHIFT}, mouse = {inp.MOUSE_LEFT}, was_pressed = false, 
							onpress = function () 
								-- override current tool
								derp:set_activetool(nil)
								cam.socket_a = derp.active_workspace:socket_hittest(cam:coord_transform(inp.getmousepos()))
								cam.comp_drag = true
							end,
							onrelease = function () 
								-- local hit
								derp:set_activetool(derp.active_tool.last)
								local socket_b = derp.active_workspace:socket_hittest(cam:coord_transform(inp.getmousepos()))
								
								derp.active_workspace:add_connection(cam.socket_a,socket_b)
								cam.socket_a = nil
								cam.comp_drag = false
							end },
						{ name = "move ws",keys = {inp.SPACE}, was_pressed = false, 
							onpress = function () 
								derp:set_activetool(move_ws) 
							end, 
							onrelease = function () 
								derp:set_activetool(derp.active_tool.last)
							end },
						{ name = "redo",keys = {inp.LCTRL, "Y"}, was_pressed = false, onpress = function () print("REDO") derp.active_workspace:redo() end},
						{ name = "undo",keys = {inp.LCTRL, "Z"}, was_pressed = false, onpress = function () print("UNDO") derp.active_workspace:undo() end},
						{ name = "delete", keys = {inp.BACKSPACE}, was_pressed = false, 
							onpress = function ()  
								if #derp.active_workspace.component_data.active_widgets > 0 then
								
									for i,wid in pairs(derp.active_workspace.component_data.active_widgets) do
										for k,v in pairs(derp.active_workspace.component_data.edges) do
											if v[1].comp.id == wid.id then
												-- wid is connected to an input
												v[2].comp.inputs[v[2].socket] = v[2].comp.inputs[v[2].socket] - 1
												derp.active_workspace.component_data.edges[k] = nil
											elseif v[2].comp.id == wid.id then
												-- wid is connected to an output
												v[1].comp.outputs[v[1].socket] = v[1].comp.outputs[v[1].socket] - 1
												derp.active_workspace.component_data.edges[k] = nil
											end
										end
									
										derp.active_workspace.component_data.nodes[wid.id] = nil
									end
									
									derp.active_workspace.component_data.active_widgets = { }
									derp:push_workspace(derp.active_workspace)
								end
							end},
					}
	
	cam.super_move_relative = cam.move_relative
	function cam:move_relative(dx,dy)
		self:needsredraw()
			
		-- static values should be dynamically evaluated to ensure resizeability..
		if ((self.drawbox.x + dx) > 20) then
			self.drawbox.x = 20
			self.hitbox.x = 20
		elseif ((self.drawbox.x + dx) < (-self.drawbox.w + 752)) then
			self.drawbox.x = -self.drawbox.w + 752
			self.hitbox.x = -self.drawbox.w + 752
		else
			self:super_move_relative(dx,0)
		end
		
		if ((self.drawbox.y + dy) > 102) then
			self.drawbox.y = 102
			self.hitbox.y = 102
		elseif ((self.drawbox.y + dy) < (-self.drawbox.h + 576)) then
			self.drawbox.y = -self.drawbox.h + 576
			self.hitbox.y = -self.drawbox.h + 576
		else
			self:super_move_relative(0,dy)
		end

		self:needsredraw()
	end
	
	function cam:mousedrag(mx,my)
		if derp.active_tool.current then 
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my})
			derp.navigator.draw_area:calc_navbox()
		end
	end
	
	function cam:mousepush(mx,my,button)
		local x,y = self:coord_transform(mx,my)
		
		self.parent:mousepush(x,y,button)
		--derp.navigator.draw_area:calc_navbox()
	end
	
	function cam:mouserelease(mx,my,button)
		local x,y = self:coord_transform(mx,my)
	
		self.parent:mouserelease(x,y,button)
	end
	
	function cam:draw(force)
		if (force or self.redraw_needed) then
			local old_alpha = gfx.getalpha()
			gfx.setalpha(0.25)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,5,1,1) -- solid bg
			gfx.setalpha(old_alpha)
			
			local old_tex = gfx.bindtexture(checkers_texture)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,0,0,500,500*0.75)	-- checkers
			gfx.bindtexture(old_tex)
		end
	end
	
	return cam
end

function derp:create_basecomponentblock(component_data)
	local wid = gui:create_basewidget(component_data.x, component_data.y,
                                    component_data.w, component_data.h)
	
	wid.super_draw = wid.draw
	function wid:draw(force)
		if self.redraw_needed or force then
			local r,g,b = gfx.getcolor()
			gfx.setcolor(1.0,1.0,1.0)
			local old_tex = gfx.bindtexture(0)
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h,1,5,1,1) -- solid bg
			gfx.bindtexture(old_tex)
			gfx.setcolor(r,g,b)
			
			-- draw chlid widgets
			self:super_draw(force)
		end
	end
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
		
		end
		--derp.active_tool(action 
	end
	
	function wid:mouserelease(x,y,button)
		if derp.active_tool.current then
		
		end
	end
	
	function wid:mousedrag(mx,my)
	  self.parent:needsredraw()
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my, widget = self})
		end
		self.parent:needsredraw()
	end
	
	return wid
end

function derp:create_workspace(x,y,w,h,from_path)
	local checkers_texture = gfx.loadtexture(1,"data/checkers.png")
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	
	wid.id_counter = 1
	wid.component_data = { active_components = {}, components = {}}
	wid.workspace_stack = { counter = 0, stack = {} }
	
	wid.shortcuts = { { name = "show ws menu", mouse = { inp.MOUSE_RIGHT }, was_pressed = false, 
							onpress = function ()
								derp.ws_menu.menu = derp.ws_menu.super_menu	
								derp.ws_menu:show() 
							end} }
	
	if from_path then
		wid.component_data = derp:load(from_path)
	end
	
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
		  
		  -- draw background
			local old_alpha = gfx.getalpha()
			gfx.setalpha(0.25)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,5,1,1) -- solid bg
			gfx.setalpha(old_alpha)
			
			local old_tex = gfx.bindtexture(checkers_texture)
			local aspect = app.height/app.width
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,0,0,500,500*aspect)	-- checkers
			gfx.bindtexture(old_tex)
			
			self:super_draw(force)
		end
	end
	
	function wid:addcomponent(comp)
		local id = "wid " .. wid.id_counter 
		self.component_data.components[id] = comp
		comp.id = id
		
		local new_comp = derp_components[comp.group][comp.type]:create_widget(comp)
		self:addwidget(new_comp)
	end
	
	function wid:undo()
		-- decrement pointer
		if self.workspace_stack.counter ~= 0 then
			self.workspace_stack.counter = self.workspace_stack.counter - 1
		
			-- retreive from workspace stack
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			if ws then
				self.component_data = loadstring("return" .. ws)()
			end
		end
	end
	
	function wid:redo()
		if self.workspace_stack.counter ~= #self.workspace_stack.stack then
			self.workspace_stack.counter = self.workspace_stack.counter + 1
			
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			self.component_data = loadstring("return" .. ws)()
		end
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
	end
	
	
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
		
		end
	end
	
	function wid:mouserelease(x,y,button)
		if derp.active_tool.current then
		
		end
	end
	
	function wid:mousedrag(mx,my)
	  self:needsredraw()
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my, widget = self})
		end
		self:needsredraw()
	end
	
	function wid:needsredraw(full)
    if (full) then
      gui:redraw()
    else
      --local x,y = self:find_abspos(self)
      --gui:redraw(x, y, self.drawbox.w, self.drawbox.h)
      -- hack so that only the visible workspace is redrawn
      gui:redraw(21, 101, 733, 478)
    end
    self.redraw_needed = true
    
    -- notify parent
    if not (self.parent == nil) then
      self.parent:childisredrawn()
    end
  end
	
	return wid
end

--[[
function derp:create_workspace(x,y,w,h,from_path)
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	--wid.active_widget = nil
	wid.cam = derp:create_workspacecamera(0,0,w,h)
	wid:addwidget(wid.cam)
	
	wid.component_data = { active_widgets = {}, nodes = {} , edges = { } }
	wid.saved = true
	wid.id_counter = 1
	wid.workspace_stack = { max_size = 10, counter = 0, stack = {} }
	
	if from_path then
		wid.component_data = derp:load(from_path)
	end
	
	function wid:set_activecomp(comp)
		if self.component_data.active_widgets then
			for k,v in pairs(self.component_data.active_widgets) do
				v.active = false
			end
		end
		
		if comp then
			for k,v in pairs(comp) do
				v.active = true
			end
		end
		
		self.component_data.active_widgets = comp
	end
	
	function wid:add_connection(socket_a,socket_b)
		if socket_a and socket_b then
			if (socket_a.type == socket_b.type) or (socket_a.comp == socket_b.comp) then 
				return nil
			end
		
			-- determine wether or not we need to switch sockets
			if socket_a.type == "input" and socket_b.type == "output" then
				-- switcharoo
				local socket_tmp = socket_a
				socket_a = socket_b
				socket_b = socket_tmp
			end
	
			-- check if input socket is connected
			if socket_b.comp.inputs[socket_b.socket] > 0 then
				return nil
			end
	
			socket_a.comp.outputs[socket_a.socket] = socket_a.comp.outputs[socket_a.socket] + 1
			socket_b.comp.inputs[socket_b.socket] = socket_b.comp.inputs[socket_b.socket] + 1
			
			local found = false
			
			for k,v in pairs(self.component_data.edges) do
				if (v[1].comp == socket_a.comp) and (v[1].socket == socket_a.socket)
					and (v[2].comp == socket_b.comp) and (v[2].socket == socket_b.socket) then
					found = true
					break
				end
			end
			
			if not found then
				table.insert(self.component_data.edges,{socket_a,socket_b})
				derp:push_workspace(self)
			end
		end
	end
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			self.cam:draw(force)
			
			if self.component_data then
			
				gfx.translate(self.cam.drawbox.w*0.5 + self.cam.drawbox.x,self.cam.drawbox.h*0.5 + self.cam.drawbox.y)

				-- add draw scaling?
				for k,v in pairs(self.component_data.nodes) do
					if v.active then
						gfx.drawcentered(v.x,v.y,v.w,v.h,13,5,1,1)
					else
						gfx.drawcentered(v.x,v.y,v.w,v.h,5,1,1,1)
					end
					
					-- draw input-boxes
					for i,inp in pairs(v.inputs) do
						if inp == 0 then
							gfx.drawcentered(v.x - v.w*0.5 + 8, v.y - v.h * 0.5 + 8 + (i-1)*16,16,16,17,1,1,1)
						else
							gfx.drawcentered(v.x - v.w*0.5 + 8, v.y - v.h * 0.5 + 8 + (i-1)*16,16,16,5,5,1,1)
						end
					end
					
					-- draw output-boxes
					for i,inp in pairs(v.outputs) do
						if inp == 0 then
							gfx.drawcentered(v.x + v.w*0.5 - 8, v.y - v.h * 0.5 + 8 + (i-1)*16,16,16,17,1,1,1)
						else
							gfx.drawcentered(v.x + v.w*0.5 - 8, v.y - v.h * 0.5 + 8 + (i-1)*16,16,16,5,5,1,1)
						end
					end
				end
				
				for k,v in pairs(self.component_data.edges) do
					local x0 = v[1].comp.x + v[1].comp.w * 0.5 - 8
					local y0 = v[1].comp.y - v[1].comp.h * 0.5 + (v[1].socket-1) * 16 + 8
					
					local x1 = v[2].comp.x - v[2].comp.w * 0.5 + 8
					local y1 = v[2].comp.y - v[2].comp.h * 0.5 + (v[2].socket-1) * 16 + 8
					
					local r,g,b = gfx.getcolor()
					gfx.setcolor(0.878431373,0.494117647 ,0.0)
					--draw_spline({{x0,y0},{x0+5,y0},{x0 + (x1 - x0)* 0.25, y0},{x1 - (x1 - x0)*0.25,y1},{x1-5,y1},{x1,y1}}, 60,2)
					local line = create_spline({{x0,y0},{x0 + (x1-x0)*0.25,y0},{x1-(x1-x0)*0.25,y1},{x1,y1}},60,2)
					line:update()
					line:draw()
					gfx.setcolor(r,g,b)
				end
			
				if self.cam.comp_drag and self.cam.socket_a then
					local x0 = self.cam.socket_a.comp.x
					local y0 = self.cam.socket_a.comp.y
					
					if self.cam.socket_a.type == "output" then
						x0 = x0 + self.cam.socket_a.comp.w * 0.5 - 8
						y0 = y0 - self.cam.socket_a.comp.h * 0.5 + (self.cam.socket_a.socket-1)*16 + 8
					else
						x0 = x0 - self.cam.socket_a.comp.w * 0.5 + 8
						y0 = y0 - self.cam.socket_a.comp.h * 0.5 + (self.cam.socket_a.socket-1)*16 + 8
					end
					
					local x1,y1 = self.cam:coord_transform(inp.getmousepos())

					local line = create_spline({{x0,y0},{x0 + (x1-x0)*0.25,y0},{x1-(x1-x0)*0.25,y1},{x1,y1}},60,2)
					line:update()
					line:draw()
				end
			
				gfx.translate(-self.cam.drawbox.w*0.5 - self.cam.drawbox.x,-self.cam.drawbox.h*0.5 - self.cam.drawbox.y)
			end
		end
	end
	
	function wid:undo()
		-- decrement pointer
		if self.workspace_stack.counter ~= 0 then
			self.workspace_stack.counter = self.workspace_stack.counter - 1
		
			-- retreive from workspace stack
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			if ws then
				self.component_data = loadstring("return" .. ws)()
				
				local edges = {}
				for k,v in pairs (self.component_data.edges) do
					local socket_a = new_socket(self.component_data.nodes[v[1].comp.id],v[1].type,v[1].socket)
					local socket_b = new_socket(self.component_data.nodes[v[2].comp.id],v[2].type,v[2].socket)
				
					table.insert(edges,{socket_a,socket_b})
				end
				self.component_data.edges = edges
				
				-- rebuild active widget data
				self.component_data.active_widgets = { }
				for k,v in pairs(self.component_data.nodes) do
					if v.active then
						table.insert(self.component_data.active_widgets,v)
					end
				end
			else
				self.component_data = { active_widgets = {}, nodes = {}, edges = {}}
			end
		end
	end
	
	function wid:redo()
		if self.workspace_stack.counter ~= #self.workspace_stack.stack then
			self.workspace_stack.counter = self.workspace_stack.counter + 1
			
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			self.component_data = loadstring("return" .. ws)()
			
			-- rebuild edge data
			local edges = {}
			for k,v in pairs (self.component_data.edges) do
				local socket_a = new_socket(self.component_data.nodes[v[1].comp.id],v[1].type,v[1].socket)
				local socket_b = new_socket(self.component_data.nodes[v[2].comp.id],v[2].type,v[2].socket)
			
				table.insert(edges,{socket_a,socket_b})
			end
			self.component_data.edges = edges
			
			-- rebuild active widget data
			self.component_data.active_widgets = { }
			for k,v in pairs(self.component_data.nodes) do
				if v.active then
					table.insert(self.component_data.active_widgets,v)
				end
			end
		end
	end
	
	function wid:addcomponent(compdata)
		local new_comp = compdata--{ x = x, y = y, w = 100, h = 100, type = ctype,id = self.id_counter, inputs = { 0,0,0 }, outputs = { 0,0,0 } }
		new_comp.id = self.id_counter
		self.component_data.nodes[self.id_counter] = new_comp
		self.id_counter = self.id_counter + 1

		self:set_activecomp( {new_comp} )
		
		derp:push_workspace(self)
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
	end
	
	function wid:socket_hittest(mx,my)
		-- first, find a component
		local hit = self:custom_hittest(mx,my)
		
		if hit then
			local y_c = hit.y - hit.h * 0.5
			local socket_id = math.ceil((my - y_c) / 16)
			
			if mx >= (hit.x - hit.w * 0.5) and mx <= (hit.x - hit.w * 0.5 + 16) then
				if socket_id <= #hit.inputs then
					return new_socket(hit,"input",socket_id)
				end
			elseif mx <= (hit.x + hit.w * 0.5) and mx >= (hit.x + hit.w * 0.5 - 16) then
				if socket_id <= #hit.outputs then
					return new_socket(hit,"output",socket_id)
				end
			end
		else
			return nil
		end
	end
	
	function wid:custom_hittest(mx,my)		
		local hit = nil
	
		for k,v in pairs(self.component_data.nodes) do
			if v then
				if mx >= (v.x - v.w * 0.5) and mx <= (v.x + v.w * 0.5) 
					and my >= (v.y - v.h * 0.5) and my <= (v.y + v.h * 0.5) then
					hit = v
				end
			end
		end
		
		return hit
	end
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mousepush",x = mx,y = my, button = button})
		end
	end
	
	local cp = 0
	
	function wid:mouserelease(x,y,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mouserelease", x = x, y = y, button = button})
		end
	end
	
	return wid
end

]]

function derp:create_workspacecontainer(x,y,w,h)
	local wid = gui:create_verticalstack(x,y,w,h)
	
	function wid:resize_callback(w,h)
		for k,v in pairs(self.childwidgets) do
			v:resize_callback(w,h)
		end
	
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
		self.drawbox.y = self.drawbox.y - h
	end
	
	return wid
end

function derp:create_maincontainer(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	wid.widget_type = "main container"
	
	function wid:resize_callback(w,h,state)
		for k,v in pairs(self.childwidgets) do
			v:resize_callback(w,h)
		end
		
		self.drawbox.y = self.drawbox.y + h
		self.drawbox.h = self.drawbox.h - h
		
		
		self.hitbox.y = self.drawbox.y
		self.hitbox.h = self.drawbox.h
		
		self:needsredraw()
	end
	
	return wid
end

function derp:create_workspaceframe(x,y,w,h)
	local wid = gui:create_basewidget(x,y,w,h)
	
	function wid:hittest(mx,my) 
	
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.y = self.drawbox.y + h
		self.drawbox.h = self.drawbox.h - h
	end
	
	function wid:draw(force)
		-- DRAW BORDERS
		
		-- OUTER BORDERS
		gfx.drawtopleft(self.drawbox.x, self.drawbox.y,self.drawbox.w,1,9,1,1,1)
		-- BOTTOM
		gfx.drawtopleft(self.drawbox.x, self.drawbox.y + self.drawbox.h,self.drawbox.w,1,9,1,1,1)
		-- LEFT
		gfx.drawtopleft(self.drawbox.x, self.drawbox.y,1,self.drawbox.h,9,1,1,1)
		-- RIGHT
		--gfx.drawtopleft(self.drawbox.x + self.drawbox.h, 1,self.drawbox.w,1,9,1,1,1)
		
		
		-- INNER BORDERS
		-- TOP
		gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,self.drawbox.w-2,1,1,5,1,1)
		-- BOTTOM
		gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y + self.drawbox.h-1,self.drawbox.w-2,1,1,5,1,1)
		-- LEFT
		gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
		-- RIGHT
		gfx.drawtopleft(self.drawbox.x + self.drawbox.w-1, self.drawbox.y+1,1,self.drawbox.h-1,1,5,1,1)
	end
	
	return wid
end

function derp:base_tool(w,h,s,t,name, onclick)
	local tool = gui:create_basewidget(0,0,40,40)	-- determine from size of toolbar mayhabps?	
	tool.highlight = false
	tool.selected = false
	tool.toggle = true
	tool.super_find_mousehit = tool.find_mousehit
	tool.icon_properties = { w = w, h = h, s = s,t = t }
	tool.onclick = onclick
	
	
	if name ~= nil then
		tool.widget_type = "tool: " .. name
	else
		tool.widget_type = "stdtool"
	end
	
	function tool:action(action)
		
	end
	
	function tool:draw(force)
		if (self.redraw_needed or force) then
			local offsetx = (self.drawbox.w - self.icon_properties.w) * 0.5
			local offsety = (self.drawbox.h - self.icon_properties.h) * 0.5
		
			if self.selected or self.pressed then
				-- BG
				gfx.drawtopleft(self.drawbox.x+4,self.drawbox.y + 6,33,29,9,22,1,1) -- upper left corner
				
				-- BORDERS
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y + 2,4,4,5,18,4,4) -- upper left corner
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w - 3,self.drawbox.y+2,3,4,9,18,3,4) -- upper right corner
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y + self.drawbox.h-5,4,3,5,22,4,3) -- lower left corner
				gfx.drawtopleft(self.drawbox.x+self.drawbox.w-3,self.drawbox.y + self.drawbox.h-5,3,3,9,22,3,3) -- lower right corner
				
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y+6,4,self.drawbox.h-11,5,22,4,1) -- left frame	
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w-3,self.drawbox.y+6,3,self.drawbox.h-11,9,22,3,1) -- right frame	
				gfx.drawtopleft(self.drawbox.x+4,self.drawbox.y+2,self.drawbox.w-7,4,9,18,1,4) -- upper frame
				gfx.drawtopleft(self.drawbox.x+4,self.drawbox.y+self.drawbox.h - 5,self.drawbox.w-7,3,9,22,1,3) -- lower frame 
				
				offsetx = offsetx + 2
				offsety = offsety + 2
			
			elseif self.highlight then
				-- BG
				gfx.drawtopleft(self.drawbox.x+2,self.drawbox.y + 4,36,32,507,0,1,128)
			
				-- draw borders
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y + 2,2,2,5,10,2,2) -- upper left corner
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w - 2,self.drawbox.y+2,2,2,10,10,2,2) -- upper right corner
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y + self.drawbox.h-4,2,2,5,15,2,2) -- lower left corner
				gfx.drawtopleft(self.drawbox.x+self.drawbox.w-2,self.drawbox.y + self.drawbox.h-4,2,2,10,15,2,2) -- lower right corner
				
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y+4,2,self.drawbox.h-8,5,12,2,1) -- left frame	
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w-2,self.drawbox.y+4,2,self.drawbox.h-8,10,12,2,1) -- right frame	
				gfx.drawtopleft(self.drawbox.x+2,self.drawbox.y+2,self.drawbox.w-4,2,7,10,1,2) -- upper frame
				gfx.drawtopleft(self.drawbox.x+2,self.drawbox.y+self.drawbox.h - 4,self.drawbox.w-4,2,7,15,1,2) -- lower frame)
			end
			
			gfx.drawtopleft(self.drawbox.x + offsetx,self.drawbox.y + offsety,self.icon_properties.w,self.icon_properties.h,self.icon_properties.s,self.icon_properties.t,self.icon_properties.w,self.icon_properties.h) -- upper left corner
		end	
	end
	
	function tool:mousedrag(mx,my)
		self.pressed = true
	end
	
	function tool:mouserelease(mx,my,button)
		self.pressed = false
	end
	
	function tool:mouseleave(mx,my)
		self:needsredraw()
		self.highlight = false
		self:needsredraw()
	end
	
	function tool:mouseover(mx,my)
		self:needsredraw()	
		self.highlight = true
		self:needsredraw()
	end
	
	function tool:mousepush(mx,my,button)
		if (button == inp.MOUSE_LEFT) then
			if self.selected then
				derp:set_activetool(nil)
				return
			end
		
			self:needsredraw()
			
			derp:set_activetool(self)
			
			if self.onclick then
				self:onclick()
			end
			
			self:needsredraw()
		end
	end
	
	return tool
end

function derp:create_toolbar(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	local draggies = gui:create_basewidget(0,0,25,40)
	local separator = gui:create_basewidget(0,0,10,40)
	
	-------- TOOLS --------
	undo = derp:base_tool(28,24,1,102,"undo", 
			function () 
				derp:set_activetool(nil)
				if derp.active_workspace then
					derp.active_workspace:undo()
				end
				derp:set_activetool(derp.active_tool.last)
			end
			)
	undo.toggle = false
	redo = derp:base_tool(28,24,2,127,"redo",
			function () 
				derp:set_activetool(nil)
				if derp.active_workspace then
					derp.active_workspace:redo()
				end
				derp:set_activetool(derp.active_tool.last)
			end)
	redo.toggle = false
	select_rect = derp:base_tool(24,24,1,36,"square select")
	move_select = derp:base_tool(24,17,1,61,"move/select")
	move_ws = derp:base_tool(21,21,1,79,"move workspace")
	
	self:set_activetool(move_ws)
	
	-----------------------
	draggies.super_draw = draggies.draw
	draggies.widget_type = "toolbar drag widget"
	
	wid.widget_type = "toolbar"
	wid.drag_removed = false
	wid.drag = false
	wid.prev_owner = nil
	
	select_rect.draw_rect = gui:create_basewidget(0,0,1,1)
	select_rect.draw_rect.widget_type = "select rect draw square"
	
	wid:addwidget(draggies)
	wid:addwidget(undo)
	wid:addwidget(redo)
	wid:addwidget(separator)
	wid:addwidget(move_ws)
	wid:addwidget(move_select)
	wid:addwidget(select_rect)
	
		
	function move_ws:action(action)
		if action.tag == "drag" then
		
			local mx = action.dx
			local my = action.dy
			
			derp.active_workspace:move_relative(mx,my)
		end
	end
	
	function select_rect.draw_rect:draw(force)
		if (self.redrawneeded or force) then
			local a = gfx.getalpha()
			gfx.setalpha(0.25)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,13,1,1,1)
			gfx.setalpha(a)
		end
	end
	
	function select_rect:action(action)
		if action.tag == "mousepush" then
			gui.widgets:addwidget(select_rect.draw_rect)
			gui.focuswidget = derp.active_workspace.cam
		
			self.new_drag = { x0 = action.x, y0 = action.y, x1 = action.x, y1 = action.y }
			
			local mx,my = inp.getmousepos()
			self.draw_rect:move_abs(mx,my)
			self.draw_rect:resize_abs(1,1)
			
		elseif action.tag == "drag" then
			self.new_drag.x1 = self.new_drag.x1 + action.dx
			self.new_drag.y1 = self.new_drag.y1 + action.dy
			
			self.draw_rect:resize_relative(action.dx,action.dy)
		elseif action.tag == "mouserelease" then
			-- sort coords
			if self.new_drag.x0 > self.new_drag.x1 then
				local tmp = self.new_drag.x0
				self.new_drag.x0 = self.new_drag.x1
				self.new_drag.x1 = tmp
			end
			
			if self.new_drag.y0 > self.new_drag.y1 then
				local tmp = self.new_drag.y0
				self.new_drag.y0 = self.new_drag.y1
				self.new_drag.y1 = tmp
			end
			
			
			derp.active_workspace.component_data.active_widgets = {}
			
			for k,v in pairs (derp.active_workspace.component_data.nodes) do
				-- bounds check
				v.active = false
				
				if self.new_drag.x0 > (v.x + v.w*0.5) then
				elseif self.new_drag.x1 < (v.x - v.w*0.5) then
				elseif self.new_drag.y0 > (v.y +v.h*0.5) then
				elseif self.new_drag.y1 < (v.y - v.h*0.5) then
				else
					if not derp.active_workspace.component_data.active_widgets then
						derp.active_workspace.component_data.active_widgets = {}
					end
					
					table.insert(derp.active_workspace.component_data.active_widgets,v)
					
					v.active = true
				end
			end
			
			if derp.active_workspace.component_data.active_widgets then
				derp:push_workspace(derp.active_workspace)
			end

			gui.widgets:removewidget(self.draw_rect)
		end
	end
	
	function move_select:action(action)
		if action.tag == "ctrl select" then
			local hit = derp.active_workspace:custom_hittest(action.x,action.y) 
			
			if hit then
				local found = false
				
				for k,v in pairs (derp.active_workspace.component_data.active_widgets) do
					if v == hit then
						found = true
						break
					end
				end
				
				if not found then
					table.insert(derp.active_workspace.component_data.active_widgets,hit)
					derp.active_workspace:set_activecomp(derp.active_workspace.component_data.active_widgets)
				end
			end	
		elseif action.tag == "mousepush" then
			local hit = derp.active_workspace:custom_hittest(action.x,action.y) 
			
			if hit then
				local found = false
				if derp.active_workspace.component_data.active_widgets then
					for k,v in pairs (derp.active_workspace.component_data.active_widgets) do
						if v == hit then
							found = true
							break
						end
					end
				end
				
				if not found then
					derp.active_workspace:set_activecomp({hit})
				end
			else
				derp.active_workspace:set_activecomp(nil)
			end
		elseif action.tag == "drag" then
			if derp.active_workspace.component_data.active_widgets then
				local rect = {x0,y0,x1,y1}
				for k,v in pairs(derp.active_workspace.component_data.active_widgets) do
					local x0 = v.x - v.w * 0.5
					local x1 = v.x + v.w * 0.5
					local y0 = v.y - v.h * 0.5
					local y1 = v.y + v.h * 0.5
				
					if not (rect.x0 and rect.x1 and rect.y0 and rect.y1) then
						rect.x0 = x0
						rect.x1 = x1
						rect.y0 = y0
						rect.y1 = y1
					end
					
					if x0 < rect.x0 then
						rect.x0 = x0
					end
					
					if x1 > rect.x1 then
						rect.x1 = x1
					end
					
					if y0 < rect.y0 then
						rect.y0 = y0
					end
					
					if y1 > rect.y1 then
						rect.y1 = y1
					end
				end
				
				local cam = derp.active_workspace.cam.drawbox
				local cx,cy = derp.active_workspace.cam:coord_transform(cam.x,cam.y)
				local dx = action.dx
				local dy = action.dy
				
				if (rect.x0 + action.dx) < cx or (rect.x1 + action.dx) > (cx + cam.w) then				
					dx = 0
				end
				
				if (rect.y0 + action.dy) < cy or (rect.y1 + action.dy) > (cy + cam.h) then
					dy = 0
				end
				
				for k,v in pairs(derp.active_workspace.component_data.active_widgets) do
					v.x = v.x + dx
					v.y = v.y + dy
				end
				
				self.moved = true
			end
		elseif action.tag == "mouserelease" then
			if self.moved then
				derp:push_workspace(derp.active_workspace)
				self.moved = false
			end
		end
	end
	
	function separator:draw(force)
		if (self.redraw_needed or force) then
			gfx.drawtopleft(self.drawbox.x + 4,self.drawbox.y + 8,2,24,5,26,2,1)
		end
	end
	
	function draggies:draw(force)
		if (self.redraw_needed or force) then
		  local old_a = gfx.getalpha()
			if self.drag then 
				gfx.setalpha(0.7)
		  end
			
			gfx.drawtopleft(4,8,3,25,1,10,3,25)
			
			gfx.setalpha(old_a)
			
			draggies:super_draw()
		end
	end
	
	function draggies:mousedrag(mx,my)
	  self.parent:needsredraw()
		self.parent:move_relative(mx,my)
		self.parent.drag = true
		
		if not self.parent.drag_removed then
		  self.parent:needsredraw(true)
		  self.parent.prev_owner = self.parent.parent
			self.parent.parent:removewidget(self.parent)
			
			self.parent.parent:resize_callback(0,-wid.drawbox.h+1)
			gui.widgets:addwidget(self.parent)
			
			x,y = inp.getmousepos()
			self.parent:move_abs(x,y)
			
			self.parent.drag_removed = true
		end
		self.parent:needsredraw()
	end
	
	function draggies:mouserelease(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
			self.parent.drag = false
			gui.widgets:removewidget(self.parent)

			table.insert(self.parent.prev_owner.childwidgets,self.parent)
			gui:set_focus(self.parent)
			
			-- determine where to put toolbar, for now just put it back..
			self.parent:move_abs(20,40)
			self.parent.parent:resize_callback(0,wid.drawbox.h-1)
			
			self.parent.parent:needsredraw()
			
			self.parent.drag_removed = false
		end
	end
	
	wid.super_draw = wid.draw
	function wid:draw(force)	
		if (self.redraw_needed or force) then
			if self.drag then 
				gfx.setalpha(0.5)
			end
			
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h, 9, 1, 1, 1)
			
			gfx.drawtopleft(self.drawbox.x+2, self.drawbox.y+2, self.drawbox.w-4, self.drawbox.h-4, 510, 0, 1, 127)
			
			-- DRAW BORDERS
			-- TOP
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,self.drawbox.w-2,1,1,5,1,1)
			-- BOTTOM
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y + self.drawbox.h-2,self.drawbox.w-2,1,1,5,1,1)
			-- LEFT
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
			-- RIGHT
			gfx.drawtopleft(self.drawbox.x + self.drawbox.w-2, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
			
			if self.drag then
				gfx.setalpha(1.0)
			end
			
			self:super_draw(force)
		end
	end
	return wid
end

function derp:create_block(x,y,w,h,type)
	local wid = gui:create_basewidget(x,y,w,h)
	wid.border = { left = false, right = false, top = false, bottom = false }
	wid.widget_type = type
	wid.super_draw = wid.draw
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h, 1, 1, 1, 1)
		
			-- DRAW BORDER 
			if (self.border.right) then
				gfx.drawtopleft(self.drawbox.x+self.drawbox.w-1,self.drawbox.y,1,self.drawbox.h,1,5,1,1)
			end
			
			if(self.border.left) then
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y,1,self.drawbox.h,1,5,1,1)
			end
			
			if(self.border.top) then
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,1,1,5,1,1)
			end
			
			if(self.border.bottom) then
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y + self.drawbox.h,self.drawbox.w,1,1,5,1,1)
			end
			
			self:super_draw(force)
		end
	end
	
	return wid
end
