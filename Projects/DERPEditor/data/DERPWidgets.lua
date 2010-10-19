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

function derp:push_active_workspace()
  derp:push_workspace(derp.active_workspace)
end

function derp:push_workspace(ws)	
	ws.workspace_stack.counter = ws.workspace_stack.counter + 1
	
	for i = ws.workspace_stack.counter, #ws.workspace_stack.stack do
		self:pop_workspace(ws)
	end
	
	local str = basic_serialize(ws.component_data,0)
	
	--print(str)
	
	table.insert(ws.workspace_stack.stack,str)
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
        table.insert(type_menu, {tostring(v.name), {tooltip = "Add " .. tostring(v.name), onclick = function()
                                                                                     --local x,y = inp.getmousepos()
																					 local x = self.addcomppos.x
																					 local y = self.addcomppos.y
																					                                           x = x - self.active_workspace.drawbox.x
																					                                           y = y - self.active_workspace.drawbox.y
                                                                                     self.active_workspace:addcomponent(v:new_block(self.active_workspace,x,y))
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
								if #derp.active_workspace.component_data.active_components > 0 then
								
									for i,wid in pairs(derp.active_workspace.component_data.active_components) do
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
									
									derp.active_workspace.component_data.active_components = { }
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

function derp:create_addconnectionbutton(onclick,x,y)
	local wid = gui:create_basewidget(x,y,12,12)
	wid.active = true
	
	function wid:mousepush(x,y,button)
		if onclick then
			onclick()
		end
	end
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			gfx.translate(self.drawbox.x,self.drawbox.y)

			gfx.drawtopleft(0,1,12,12,1,205,12,12) -- bg
			
			if self.active then
				gfx.drawtopleft(3,4,6,6,4,219,6,6) -- add cross
			else 
				gfx.drawtopleft(3,4,6,6,11,219,6,6) -- add cross
			end

			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end	
	
	return wid
end

function derp:create_connectioninput(id,x,y)
  local wid = gui:create_basewidget(x, y, 12, 12)
  wid.widget_type = "connection_input"
  wid.input_id = id
  
  wid.superdraw = wid.draw
  function wid:draw(force)
    self:superdraw(force)
    
    if self.redraw_needed or force then
      gfx.translate(self.drawbox.x,self.drawbox.y)
      
	  gfx.drawtopleft(0,1,12,12,1,205,12,12)
      
      gfx.translate(-self.drawbox.x,-self.drawbox.y)
    end
    
  end
  
  function wid:mouserelease(mx,my,button)
    print("released input")
  end
  
  return wid
end

function derp:create_connectionoutput(id,x,y)
  local wid = gui:create_basewidget(x, y, 12, 12)
  wid.output_id = id
  wid.widget_type = "connection_output"
  
  wid.superdraw = wid.draw
  function wid:draw(force)
    self:superdraw(force)
    
    if self.redraw_needed or force then
      gfx.translate(self.drawbox.x,self.drawbox.y)
	  
	  gfx.drawtopleft(0,1,12,12,1,205,12,12)
      
      gfx.translate(-self.drawbox.x,-self.drawbox.y)
    end
    
  end
  
  function wid:mouserelease(mx,my,button)
    -- see if we found a input to connect to
    local htest = self.parent.parent.parent:find_mousehit(mx,my,mx,my)
	
    if htest then
      if (htest.widget_type == "connection_input") then
        --print("Trying to add connection from output '" .. self.output_id .. "' to input '" .. htest.input_id .. "'")
        htest.parent.parent:add_connection(self.parent.parent.component_id, self.output_id, self.parent.parent.data.type, htest.input_id)
      end
    end
    self.parent.temp_connection = nil
  end
  
  function wid:mousepush(mx,my,button)
	--print(mx,my)
  end
  
  function wid:mousedrag(mx,my,button)
    -- get mouse position relative to parent widget
    local x,y = inp.getmousepos()
    -- find spline start on output widget
	
	--print(self.drawbox.x,self.drawbox.y,self.parent.parent.drawbox.x)
	
	local w = self.parent.parent
	
	--x = x - w.drawbox.x
    --y = y - w.drawbox.y
	
	x = x - derp.active_workspace.drawbox.x - w.drawbox.x
    y = y - derp.active_workspace.drawbox.y - w.drawbox.y	
	
	print(w.widget_type)
	
    --local startx,starty = self.parent.parent.drawbox.x + self.drawbox.x + 7, self.parent.parent.drawbox.y + self.drawbox.y + 7
	local startx,starty = self.drawbox.x + 7,self.drawbox.y + 7 + 26
    
    -- create our spline
    self.parent.temp_connection = create_spline({{startx,starty}, {x,y}}, 30, 4)
  end
  
  return wid
end

function derp:create_basecomponentblock(component_data,max_inputs,max_outputs)
	local wid = gui:create_basewidget(component_data.x, component_data.y,
                                    component_data.w, component_data.h)

	local length = #component_data.id*8 + 20
	
	if length > (wid.drawbox.w - 12) then
		length = length - (wid.drawbox.w-12)
		wid:resize_relative(length,0)
		component_data.w = component_data.w + length
	end
	
	if not max_inputs then
		max_inputs = 0
	end
	
	if not max_outputs then
		max_outputs = 0
	end
									
	wid.data = component_data
	wid.id = component_data.id
	wid.widget_type = "component " .. wid.id
	wid.active = true
	wid.super_addwidget = wid.addwidget
	wid.temp_connection = nil
	local io_height = math.max(wid.data.inputs,#wid.data.outputs) * 14 + 7
	
	local header = gui:create_basewidget(0,0,wid.drawbox.w,26)
	local content = gui:create_basewidget(0,26,wid.drawbox.w,component_data.h + io_height + 18)
	local body = gui:create_basewidget(0,26+io_height,component_data.w,component_data.h)
	local minimize_button = gui:create_basewidget(wid.drawbox.w - 12,0,12,8)
	local activate_button = gui:create_basewidget(0,0,10,10)
	local body_minimize_button = gui:create_basewidget(0,0,12,12)
	
	content.io_height = io_height
	wid.content = content
	
	-- STATE MANAGEMENT
	body.state = "maximized"
	wid.state = "maximized"
	
	local add_input_button = nil
	add_input_button = derp:create_addconnectionbutton(
					function()
						if wid.data.inputs ~= max_inputs then
							wid.data.inputs = wid.data.inputs + 1
							
							add_input_button:move_relative(0,14)
							wid:calc_height()
							content:addwidget(derp:create_connectioninput(wid.data.inputs, -6,(wid.data.inputs-1)*14))
							
							if wid.data.inputs == max_inputs then
								add_input_button.active = false
							end
							
						end
					end
					,-6,component_data.inputs*14)
	
	local add_output_button = nil
	add_output_button = derp:create_addconnectionbutton(
					function()
						if #wid.data.outputs ~= max_outputs then
							add_output_button:move_relative(0,14)
							local new_name = derp.active_workspace:gen_new_outputname()
							table.insert(wid.data.outputs,new_name)
							wid:calc_height()
							content:addwidget(derp:create_connectionoutput(new_name, wid.drawbox.w-6, (#wid.data.outputs-1)*14))
							
							if #wid.data.outputs == max_outputs then
								add_output_button.active = false
							end
						end
					end,component_data.w-6,#component_data.outputs*14)
	
	for i=1,component_data.inputs do
		local v = derp:create_connectioninput(i, -6,(i-1)*14)
		content:addwidget(v)
		print(v.drawbox.y)
	end
  
	for i=1,#component_data.outputs do
		local v = derp:create_connectionoutput(component_data.outputs[i], wid.drawbox.w-6, (i-1)*14)
		content:addwidget(v)
		print(v.drawbox.y)
	end
	
	-- ADD WIDGETS
	
	if wid.data.inputs < max_inputs then
		content:addwidget(add_input_button)
	end
	
	if #wid.data.outputs < max_outputs then
		content:addwidget(add_output_button)
	end
	
	content:addwidget(body_minimize_button)
	content:addwidget(body)
	header:addwidget(minimize_button)
	header:addwidget(activate_button)
	wid:super_addwidget(header)
	wid:super_addwidget(content)
	
	-- FUNCTIONS
	
	-- find connection socket with output name
	function wid:get_outputsocket(socketname)
		for k,v in pairs(content.childwidgets) do
		  if v.output_id and v.output_id == socketname then
			return v
		  end
		end

		-- didnt find output socket
		return nil
	end
	
	function wid:add_connection(from_block, from_output, type_output, to_id)
		table.insert(self.data.connections_in, {block = from_block, output = from_output, type = type_output, input = to_id})
	end
	
	function wid:calc_height()
		local old_io_height = content.io_height
		content.io_height = math.max(self.data.inputs,#self.data.outputs)*14+7
		
		self:resize_abs(self.drawbox.w,header.drawbox.h+content.io_height+self.data.h + 18)
		content:resize_abs(self.drawbox.w,content.io_height+self.data.h + 18)
		body:move_abs(0,content.io_height)
		
		body_minimize_button:move_abs(content.drawbox.w - 12,content.drawbox.h - 12)
		
		--print(wid.drawbox.h,content.drawbox.h,body_minimize_button.drawbox.y)
	end
	
	wid:calc_height()
	
	function wid:addwidget(cwid)
		body:addwidget(cwid)
	end
	
	wid.super_draw = wid.draw
	
	--[[
	function wid:draw(force)
		gfx.translate(self.hitbox.x,self.hitbox.y)
			-- HEADER
		gfx.drawtopleft(0, 0, self.drawbox.w,self.drawbox.h,5,5,1,1) -- top-left corner
		gfx.translate(-self.hitbox.x,-self.hitbox.y)
		
		self:super_draw(force)
	end
	]]
	
	header.super_draw = header.draw
	function header:draw(force)
		if self.redraw_needed or force then
			gfx.translate(self.drawbox.x,self.drawbox.y)
			-- HEADER
			gfx.drawtopleft(0, 0, 2, 2,1,227,2,2) -- top-left corner
			gfx.drawtopleft(self.drawbox.w-2, 0, 2, 2,4,227,2,2) -- top-right corner
			gfx.drawtopleft(2,self.drawbox.y, self.drawbox.w-4, 1,3,227,1,1) -- top frame
			gfx.drawtopleft(2,self.drawbox.y+1, self.drawbox.w-4, 1,506, 0, 1, 1) -- top frame 2
			gfx.drawtopleft(1,self.drawbox.y+2, self.drawbox.w-2, 24,506, 1, 1, 127) -- bg
			gfx.drawtopleft(0,self.drawbox.y+25,self.drawbox.w,1,3,227, 1,1) -- bottom frame
			gfx.drawtopleft(0,self.drawbox.y+2,1,self.drawbox.h-2,3,227,1,1) -- left frame
			gfx.drawtopleft(self.drawbox.w-1,2,1,self.drawbox.h-2,3,227,1,1) -- right frame
			
			--gfx.scale(0.8)
			gui:drawfont("^(0.878431373, 0.494117647,0){" .. wid.data.id .."}",20,9)
			--gfx.scale(1 / 0.8)
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
			
			self:super_draw(force)
		end
	end
	
	function activate_button:draw(force)	
		if self.redraw_needed or force then
			if wid.active then
				gfx.drawtopleft(3, 2, 5, 5,1,232,5,5) -- top-left corner
			else
				gfx.drawtopleft(3, 2, 5, 5,7,232,5,5) -- top-left corner
			end
		end
	end
	
	function activate_button:mousepush(x,y,button)
		wid.active = not wid.active
	end
	
	wid.old_data_h = component_data.h
	
	function body_minimize_button:mousepush(mx,my,button)		
		if body.state == "minimized" then
			body.state = "maximized"
			body.visible = true
			wid.data.h = wid.old_data_h
		else
			body.state = "minimized"
			body.visible = false
			wid.data.h = 0
		end
		
		wid:calc_height()
		
		--self:move_abs(wid.drawbox.w - 12,wid.drawbox.h - 12)
	end
	
	function body_minimize_button:draw(force)
		if self.redraw_needed or force then
			gfx.translate(self.drawbox.x,self.drawbox.y)
			if body.state == "maximized" then
				gfx.drawtopleft(3,1, 5, 7,1,242,5,7)
			else 
				gfx.drawtopleft(3,1, 5, 7,7,242,5,7)
			end
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end
	
	function minimize_button:mousepush(mx,my,button)
		if wid.state == "maximized" then
			wid.state = "minimized"
			wid:resize_relative(0,-content.drawbox.h)
		else
			wid.state = "maximized"
			wid:resize_relative(0,content.drawbox.h)
		end
	end
	
	function minimize_button:draw(force)
		if self.redraw_needed or force then
			gfx.translate(self.drawbox.x,self.drawbox.y)
			if wid.state == "maximized" then
				gfx.drawtopleft(2,3, 5, 1,1,239,1,1) -- top-left corner
			else
				gfx.drawtopleft(2,2, 5, 5,18,220,5,5) -- top-left corner
			end
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end
	
	content.super_draw = content.draw
	
	function content:draw(force)
		if (self.redraw_needed or force) and (wid.state == "maximized") then
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.io_height,7,227, 1,1) -- bg
			
			local body_height = self.parent.data.h
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y+self.io_height,self.drawbox.w,self.drawbox.h - self.io_height,1,1, 1,1) -- bg
			
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y+self.drawbox.h-2,2,2,1,230,2,2) -- bottom left
			gfx.drawtopleft(self.drawbox.x+self.drawbox.w-2,self.drawbox.y+self.drawbox.h-2,2,2,4,230,2,2) -- bottom right
			
			
			gfx.drawtopleft(self.drawbox.x,wid.drawbox.h-18,self.drawbox.w,16,505,0,1,126) -- bg
			gfx.drawtopleft(2,self.drawbox.y+self.drawbox.h-2,self.drawbox.w-4,1,505,127,1,1) -- bg
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y + self.io_height - 1,self.drawbox.w,1,1,5, 1,1) -- top
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,1,self.drawbox.h-2,1,5, 1,1) -- top
			gfx.drawtopleft(self.drawbox.x + self.drawbox.w - 1,self.drawbox.y,1,self.drawbox.h-2,1,5, 1,1) -- top
			gfx.drawtopleft(self.drawbox.x + 2,self.drawbox.y + self.drawbox.h-1,self.drawbox.w-4,1,1,5, 1,1) -- top
			
			for k,v in pairs(self.parent.data.connections_in) do
				
				local startwid = self.parent.parent:get_block(v.block)
				local startsocket = startwid:get_outputsocket(v.output)
				local startx,starty
				
				if self.parent.state == "maximized" then
					startx,starty = self.drawbox.x - (self.parent.drawbox.x - startwid.drawbox.x) + self.drawbox.w,self.drawbox.y - (self.parent.drawbox.y - startwid.drawbox.y) + startsocket.drawbox.y + 7
					local endx,endy = self.drawbox.x,self.drawbox.y + (v.input - 1) * 14 + 7
				
					-- create spline from output-socket/-widget to ourself
					-- TODO: This should only be done once, when the connection is created
					local new_line = create_spline({{startx,starty},{endx,endy}}, 30, 4)
					new_line:update()
					new_line:draw()
				end
			end
			
			-----------------------
			-- Draw temporary connection (ie. a connection that is not connected to anything yet)
			if self.temp_connection then
				self.temp_connection:update()
				self.temp_connection:draw()
			end
			
			self:super_draw(force)
		end
	end
	
	-- MOUSE FUNCTIONS
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mousepush", x = mx, y = my, button = button, widget = self})
		end
	end
	
	function wid:mouserelease(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mouserelease", x = mx, y = my, button = button, widget = self})
		end
	end
	
	function wid:mousedrag(mx,my)
	  self.parent:needsredraw()
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my, widget = self})
		end
		self.parent:needsredraw()
	end
	
	function content:mousepush(mx,my,button) 
		wid:mousepush(mx,my,button)
	end
	
	function content:mousedrag(mx,my) 
		wid:mousedrag(mx,my)
	end
	
	function content:mouserelease(mx,my,button)
		wid:mouserelease(mx,my,button)
	end
	
	function body:mousepush(mx,my,button) 
		wid:mousepush(mx,my,button)
	end
	
	function body:mousedrag(mx,my) 
		wid:mousedrag(mx,my)
	end
	
	function body:mouserelease(mx,my,button)
		wid:mouserelease(mx,my,button)
	end
	
	function header:mousepush(mx,my,button) 
		wid:mousepush(mx,my,button)
	end
	
	function header:mousedrag(mx,my) 
		wid:mousedrag(mx,my)
	end
	
	function header:mouserelease(mx,my,button)
		wid:mouserelease(mx,my,button)
	end
	
	
	return wid
end

--[[
function derp:create_basecomponentblock(component_data,max_inputs,max_outputs)
	local wid = gui:create_basewidget(component_data.x, component_data.y,
                                    component_data.w, component_data.h + 26)
	-- 12
	local length = #component_data.id*8 + 20
	
	if length > (wid.drawbox.w - 12) then
		length = length - (wid.drawbox.w-12)
		wid:resize_relative(length,0)
		component_data.w = component_data.w + length
	end
	
	if not max_inputs then
		max_inputs = 0
	end
	
	if not max_outputs then
		max_outputs = 0
	end
	
	wid.active = true
	wid.data = component_data
	wid.id = component_data.id
	wid.widget_type = "component " .. wid.id
	wid.activate_button = gui:create_basewidget(0,0,10,10)
	wid.add_input_button = derp:create_addconnectionbutton(
					function()
						if wid.data.inputs ~= max_inputs then
							wid.data.inputs = wid.data.inputs + 1
							wid.add_input_button:move_relative(0,14)
							wid:calc_ioheight()
							wid.content:addwidget(derp:create_connectioninput(wid.data.inputs, -6,(wid.data.inputs-1)*14))
							
							if wid.data.inputs == max_inputs then
								wid.add_input_button.active = false
							end
							
						end
					end
					,-6,component_data.inputs*14)
	
	wid.add_output_button = nil
	wid.add_output_button = derp:create_addconnectionbutton(
					function()
						if #wid.data.outputs ~= max_outputs then
							wid.add_output_button:move_relative(0,14)
							local new_name = derp.active_workspace:gen_new_outputname()
							table.insert(wid.data.outputs,new_name)
							wid:calc_ioheight()
							wid.content:addwidget(derp:create_connectionoutput(new_name, wid.drawbox.w-6, (#wid.data.outputs-1)*14))
							
							if #wid.data.outputs == max_outputs then
								wid.add_output_button.active = false
							end
						end
					end,component_data.w-6,#component_data.outputs*14)
	
	wid.io_height = math.max(wid.data.inputs,#wid.data.outputs)*14+7
	
	-- find body y pos
	----
	-- FIXME: This forks up if we add inputs/outputs dynamically
	----
	
	-- temporary unfinished connection
	wid.temp_connection = nil
	
	wid.content = gui:create_basewidget(0,26,wid.drawbox.w,wid.io_height)
	
	-- create input/output widgets
  for i=1,component_data.inputs do
	local v = derp:create_connectioninput(i, -6,(i-1)*14)
    wid.content:addwidget(v)
	print(v.drawbox.y)
  end
  
  for i=1,#component_data.outputs do
	local v = derp:create_connectionoutput(component_data.outputs[i], wid.drawbox.w-6, (i-1)*14)
    wid.content:addwidget(v)
	print(v.drawbox.y)
  end
  
  function wid.activate_button:draw(force)	
	if self.redraw_needed or force then
		if wid.active then
			gfx.drawtopleft(3, 2, 5, 5,1,232,5,5) -- top-left corner
		else
			gfx.drawtopleft(3, 2, 5, 5,7,232,5,5) -- top-left corner
		end
	end
  end

  wid.body = gui:create_basewidget(0,26 + wid.io_height,wid.data.w,wid.data.h)
  wid.body_minimize_button = gui:create_basewidget(wid.drawbox.w - 12,wid.drawbox.h - 12,12,12)
  wid.body_minimize_button.state = "maximized"
  
  function wid:calc_ioheight()
		local old_io_height = self.io_height
		self.io_height = math.max(self.data.inputs,#self.data.outputs)*14+7
		
		self:resize_abs(self.drawbox.w,26+self.io_height+self.data.h + 18)
		self.body:move_relative(0,self.io_height - old_io_height) 
		self.body_minimize_button:move_abs(self.drawbox.w - 12,self.drawbox.h - 12)
	end

  wid:calc_ioheight()
  wid.content:addwidget(wid.body)
  
  function wid.activate_button:mousepush(x,y,button)
	wid.active = not wid.active
  end
  
  wid.minimize_button = gui:create_basewidget(wid.drawbox.w - 12,0,12,8)
  
  -- add-input component
  if max_inputs > 0 and wid.data.inputs < max_inputs then
	wid.content:addwidget(wid.add_input_button)
  end
  
  -- add-input component
  if max_outputs > 0 and #wid.data.outputs < max_outputs then
	wid.content:addwidget(wid.add_output_button)
  end
  
  wid.header = gui:create_basewidget(0,0,wid.drawbox.w,26)
  
  -- add activate button
  wid.header:addwidget(wid.activate_button)
  
  -- add minimize button
  wid.header:addwidget(wid.minimize_button)
  
  wid:addwidget(wid.header)
  wid:addwidget(wid.content)
  
  -- add body minimize button
  --wid:addwidget(wid.body_minimize_button)
  wid.content:addwidget(wid.body_minimize_button)
	
	-- how to create new connections
	function wid:add_connection(from_block, from_output, type_output, to_id)
	  table.insert(self.data.connections_in, {block = from_block, output = from_output, type = type_output, input = to_id})
  end
  
  
	
	wid.super_addwidget = wid.addwidget
	
	function wid:addwidget(cwid) 
		--cwid:move_relative(0,self.io_height + 26)
	
		self.body:addwidget(cwid)
		--self:super_addwidget(cwid)
	end
	
	wid.super_draw = wid.draw
	
	function wid:draw(force)
		if self.redraw_needed or force then
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,5,5,1,1)
		
			gfx.translate(self.drawbox.x,self.drawbox.y)
		
			for k,v in pairs(self.childwidgets) do
				v:draw(force)
			end
			
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end
	
	function wid.body_minimize_button:mousepush(mx,my,button)
		if self.state == "minimized" then
			self.state = "maximized"
			wid.body.visible = true
			wid:resize_relative(0,wid.body.drawbox.h)
		else
			self.state = "minimized"
			wid.body.visible = false
			wid:resize_relative(0,-wid.body.drawbox.h)
		end
		
		self:move_abs(wid.drawbox.w - 12,wid.drawbox.h - 12)
	end
	
	function wid.body_minimize_button:draw(force)
		if self.redraw_needed or force then
			gfx.translate(self.drawbox.x,self.drawbox.y)
			if self.state == "maximized" then
				gfx.drawtopleft(2,1, 5, 7,1,242,5,7)
			else 
				gfx.drawtopleft(2,1, 5, 7,7,242,5,7)
			end
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end
	
	function wid.minimize_button:draw(force)
		if self.redraw_needed or force then
			gfx.translate(self.drawbox.x,self.drawbox.y)
			gfx.drawtopleft(2,3, 5, 2,1,239,1,1) -- top-left corner
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
		end
	end
	
	wid.header.super_draw = wid.header.draw
	
	function wid.header:draw(force)
		if self.redraw_needed or force then
			-- HEADER
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, 2, 2,1,227,2,2) -- top-left corner
			gfx.drawtopleft(self.drawbox.x+self.drawbox.w-2, self.drawbox.y, 2, 2,4,227,2,2) -- top-right corner
			gfx.drawtopleft(self.drawbox.x+2,self.drawbox.y, self.drawbox.w-4, 1,3,227,1,1) -- top frame
			gfx.drawtopleft(self.drawbox.x+2,self.drawbox.y+1, self.drawbox.w-4, 1,506, 0, 1, 1) -- top frame 2
			gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+2, self.drawbox.w-2, 24,506, 1, 1, 127) -- bg
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y+25,self.drawbox.w,1,3,227, 1,1) -- bottom frame
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y+2,1,self.drawbox.h-2,3,227,1,1) -- left frame
			gfx.drawtopleft(self.drawbox.x + self.drawbox.w-1,self.drawbox.y+2,1,self.drawbox.h-2,3,227,1,1) -- right frame
			
			
			gfx.translate(self.drawbox.x,self.drawbox.y)
			
			--gfx.scale(0.8)
			gui:drawfont("^(0.878431373, 0.494117647,0){" .. self.parent.data.id .."}",20,9)
			--gfx.scale(1 / 0.8)
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
			
			self:super_draw(force)
		end
	end

	wid.content.super_draw = wid.draw
	
	function wid.content:draw(force)
		if self.redraw_needed or force then
			
			local io_height = wid.io_height
			
			gfx.translate(self.drawbox.x,self.drawbox.y)
			
			-- INPUT/OUTPUT BODY
			-- move calculations to add buttons			
			gfx.drawtopleft(0,0,self.drawbox.w,io_height,7,227, 1,1) -- bg
			
			-- BODY
			local body_height = self.parent.data.h
			
			gfx.drawtopleft(0,26 + io_height,	self.drawbox.w,body_height,1,1, 1,1) -- bg
			

			
			gfx.translate(-self.drawbox.x,-self.drawbox.y)
			------------------------
			-- draw chlid widgets
			self:super_draw(force)
			
			------------------------
			-- Draw connections
			for k,v in pairs(self.parent.data.connections_in) do
			  
				-- find end position (input-socket on this component, since we show incomming connections)
				local endx,endy = self.drawbox.x,self.drawbox.y
				
				-- find start position (ie. output-socket on remote component)
				local startwid = self.parent:get_block(v.block)
				local startsocket = startwid:get_outputsocket(v.output)
				local startx,starty = startwid.drawbox.x,startwid.drawbox.y
				startx = startx + startwid.drawbox.w
				
				-- find y-position on both sockets
				endy = endy + v.input * 32 + 16
				starty = starty + startsocket.drawbox.y + 16
				
				-- create spline from output-socket/-widget to ourself
				-- TODO: This should only be done once, when the connection is created
				local new_line = create_spline({{startx,starty}, {endx,endy}}, 30, 4)
				new_line:update()
				new_line:draw()
			end
			
			-----------------------
			-- Draw temporary connection (ie. a connection that is not connected to anything yet)
			if self.temp_connection then
				self.temp_connection:update()
				self.temp_connection:draw()
		  end
		end
	end
	
	function wid.content:mousepush(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mousepush", x = mx, y = my, button = button, widget = self})
		end
	end
	
	function wid.content:mouserelease(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mouserelease", x = mx, y = my, button = button, widget = self})
		end
	end
	
	function wid.content:mousedrag(mx,my)
	  self.parent:needsredraw()
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my, widget = self})
		end
		self.parent:needsredraw()
	end
	
	function wid.body:mousepush(mx,my,button) 
		wid.content:mousepush(mx,my,button)
	end
	
	function wid.body:mousedrag(mx,my) 
		wid.content:mousedrag(mx,my)
	end
	
	function wid.body:mouserelease(mx,my,button)
		wid.content:mouserelease(mx,my,button)
	end
	
	-------------------------------
	-- Connections
	
	return wid
end
]]

function derp:create_workspace(x,y,w,h,from_path)
	local checkers_texture = gfx.loadtexture(1,"data/checkers.png")
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	wid.super_move_relative = wid.move_relative
	
	wid.id_counter = 1
	wid.output_counter = 1
	wid.component_data = { active_components = {}, components = {}}
	wid.workspace_stack = { counter = 0, stack = {} }
	
	wid.shortcuts = { 	{ name = "print data", keys = {"P"},was_pressed = false, onpress = function () derp:print_activedata() end },
						{ name = "select move", keys = {"A"}, was_pressed = false, onpress = function () derp:set_activetool(move_select) end },
						{ name = "square select", keys = {"M"}, was_pressed = false, onpress = function () derp:set_activetool(select_rect) end },
						{ name = "move ws",keys = {inp.SPACE}, was_pressed = false, 
							onpress = function () 
								derp:set_activetool(move_ws) 
							end, 
							onrelease = function () 
								derp:set_activetool(derp.active_tool.last)
							end },
						{ name = "show ws menu", mouse = { inp.MOUSE_RIGHT }, was_pressed = false, 
							onpress = function ()
								local x,y = inp.getmousepos()
								derp.addcomppos = {x = x, y = y }
								derp.ws_menu.menu = derp.ws_menu.super_menu	
								derp.ws_menu:show() 
							end} }
	
	if from_path then
		wid.component_data = derp:load(from_path)
	end
	
	function wid:gen_new_blockname(blacktype)
	  local new_name = "Block_" .. tostring(blacktype) .. "_" .. tostring(self.id_counter)
	  self.id_counter = self.id_counter + 1
	  return new_name
  end
	
	function wid:gen_new_outputname()
	  local new_name = "output" .. tostring(self.output_counter)
	  self.output_counter = self.output_counter + 1
	  return new_name
  end
  
  function wid:attach_input_connection(remote_block, remote_output)
    table.insert(connections_in, {block = remote_block, output = remote_output})
  end
	
	-- function to lookup a block by id
	function wid:get_block(block_id)
	  for k,v in pairs(self.childwidgets) do
	    if (v.component_id == block_id) then
	      return v
      end
	  end
	  print("Could not find '" .. block_id .. "'")
	  return nil
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
		local id = self:gen_new_blockname(comp.group) --"wid" .. wid.id_counter 
		self.component_data.components[id] = comp
		comp.id = id
		--wid.id_counter = wid.id_counter + 1
		
		local new_comp = derp_components[comp.group][comp.type]:create_widget(comp)
		new_comp.component_id = id
		self:addwidget(new_comp,id)
		
		derp:push_workspace(self)
	end
	
	function wid:undo()
		-- decrement pointer
		
		if self.workspace_stack.counter ~= 0 then
			self.workspace_stack.counter = self.workspace_stack.counter - 1
		
			-- retreive from workspace stack
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			if ws then
				self.component_data = loadstring("return" .. ws)()
				
				self.childwidgets = { }
				
				for k,v in pairs(self.component_data.components) do
					--self.childwidgets[v.id] = derp_components[v.group][v.type]:create_widget(v)
					self:addwidget(derp_components[v.group][v.type]:create_widget(v),v.id)
				end
			else
				self.childwidgets = { }
				self.component_data = {components = { }, active_components = { } }
			end
		end
	end
	
	function wid:redo()
		if self.workspace_stack.counter ~= #self.workspace_stack.stack then
			self.workspace_stack.counter = self.workspace_stack.counter + 1
			
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			self.component_data = loadstring("return" .. ws)()
			
						
			self.childwidgets = { }
				
			for k,v in pairs(self.component_data.components) do
				--self.childwidgets[v.id] = derp_components[v.group][v.type]:create_widget(v)
				self:addwidget(derp_components[v.group][v.type]:create_widget(v),v.id)
			end
		end
	end
	
	function wid:move_relative(dx,dy)
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
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
	end
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mousepush", x = mx, y = my, button = button, widget = self})
		end
	end
	
	function wid:mouserelease(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mouserelease", x = mx, y = my, button = button, widget = self})
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
	
	wid.component_data = { active_components = {}, nodes = {} , edges = { } }
	wid.saved = true
	wid.id_counter = 1
	wid.workspace_stack = { max_size = 10, counter = 0, stack = {} }
	
	if from_path then
		wid.component_data = derp:load(from_path)
	end
	
	function wid:set_activecomp(comp)
		if self.component_data.active_components then
			for k,v in pairs(self.component_data.active_components) do
				v.active = false
			end
		end
		
		if comp then
			for k,v in pairs(comp) do
				v.active = true
			end
		end
		
		self.component_data.active_components = comp
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
				self.component_data.active_components = { }
				for k,v in pairs(self.component_data.nodes) do
					if v.active then
						table.insert(self.component_data.active_components,v)
					end
				end
			else
				self.component_data = { active_components = {}, nodes = {}, edges = {}}
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
			self.component_data.active_components = { }
			for k,v in pairs(self.component_data.nodes) do
				if v.active then
					table.insert(self.component_data.active_components,v)
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

function derp:create_toolbar_movecontainer()
	local wid = gui:create_basewidget(0,0,app.width,app.height)
	wid.super_draw = wid.draw
	wid.widget_type = "move container"
	
	function create_arrow(x,y,w,h,s,t)
		local arrow = gui:create_basewidget(x + app.width*0.5,y + app.height*0.5,w,h)
		
		
		function arrow:draw(force)
			if self.redraw_needed or force then
				gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,s,t,self.drawbox.w,self.drawbox.h)
			end
		end
		
		return arrow
	end
	
	local left = create_arrow(-24,-11,13,23,2,165)
	local right = create_arrow(12,-11,13,23,37,165)
	local top = create_arrow(-11,-24,23,13,15,152)
	local bottom = create_arrow(-11,12,23,13,15,188)
	
	left.widget_type = "left arrow"
	right.widget_type = "right arrow"
	top.widget_type = "top arrow"
	bottom.widget_type = "bottom arrow"
	
	wid:addwidget(left)
	wid:addwidget(right)
	wid:addwidget(top)
	wid:addwidget(bottom)
	
	function wid:draw(force)
		if self.redraw_needed or force then
			local a = gfx.getalpha()
			
			gfx.setalpha(0.6)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,9,1,1,1)
			gfx.setalpha(a)
			
			self:super_draw(true)
		end
	end
	
	return wid
end

function derp:create_toolbar()
	local wid = gui:create_basewidget(0,0,0,0)
	local horizontal = self:create_horizontal_toolbar(20,40,app.width-40,40)
	local vertical = nil
	
	wid.current_state = horizontal
	
	wid:addwidget(horizontal)
	
	function wid:draw(force)
		if self.needsredraw or force then
			for k,v in pairs(self.childwidgets) do
				v:draw(force)
			end
		end
	end
	
	function wid:set_state(state)
		if state == "horizontal" then
			self.current_state = self.horizontal
			self:move_abs(horizontal.drawbox.x,horizontal.drawbox.y)
			self:resize_abs(horizontal.drawbox.w,horizontal.drawbox.h)
		else
			self.current_state = self.vertical
			self:resize_abs(vertical.drawbox.w,vertical.drawbox.h)
		end
	end
	
	return wid
end

function derp:create_horizontal_toolbar(x,y,w,h)
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
	delete_wid = derp:base_tool(14,14,30,63,"delete widget",
			function () 
				derp:set_activetool(nil)
				if derp.active_workspace then

				end
				derp:set_activetool(derp.active_tool.last)
			end)
	
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
	wid:addwidget(delete_wid)
	
	local move_container = derp:create_toolbar_movecontainer()
	
		
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
			gui.focuswidget = derp.active_workspace
		
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
			
			local hits = {}
			
			for k,v in pairs(derp.active_workspace.childwidgets) do
				local hit = v:hittest(self.new_drag.x0 - derp.active_workspace.drawbox.x,
									  self.new_drag.y0 - derp.active_workspace.drawbox.y,
									  self.new_drag.x1 - derp.active_workspace.drawbox.x,
									  self.new_drag.y1 - derp.active_workspace.drawbox.y)
				
				if hit then
					table.insert(hits,v.id)
				end
			end
			
			derp.active_workspace.component_data.active_components = hits
			
			if #hits > 0 then
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
				
				for k,v in pairs (derp.active_workspace.component_data.active_components) do
					if v == hit.id then
						found = true
						break
					end
				end
				
				if not found then
					table.insert(derp.active_workspace.component_data.active_components,hit.id)
					derp:push_workspace(derp.active_workspace)
					--derp.active_workspace:set_activecomp(derp.active_workspace.component_data.active_components)
				end
			end
		elseif action.tag == "mousepush" then
			local w = action.widget
			local hit = string.find(w.widget_type,"component %a")
			
			if hit then
				local found = false
				
				for k,v in pairs(derp.active_workspace.component_data.active_components) do
					if v == w.id then
						found = true
						break
					end
				end
				
				if not found then
					derp.active_workspace.component_data.active_components = {w.id}
					derp:push_workspace(derp.active_workspace)
				end
			else
				derp.active_workspace.component_data.active_components = {}
			end	
			
			--[[
			local hit = derp.active_workspace:hittest(action.x - derp.active_workspace.drawbox.x,action.y-derp.active_workspace.drawbox.y) 
			
			if hit then
				local found = false
				local find_k = -1
				
				if derp.active_workspace.component_data.active_components then
					for k,v in pairs (derp.active_workspace.component_data.active_components) do
						if v == hit then
							find_k = k
							found = true
							break
						end
					end
				end
				
				if not found then
					print(find_k)
					derp.active_workspace:set_activecomp({hit})
				end
			else
				derp.active_workspace:set_activecomp(nil)
			end ]]
		elseif action.tag == "drag" then
			if derp.active_workspace.component_data.active_components then
				self.moved = false
				
				for k,v in pairs(derp.active_workspace.component_data.active_components) do
					local w = derp.active_workspace.childwidgets[v]
					
					if w then
						w:move_relative(action.dx,action.dy)
						self.moved = true
					end
				end
			end
		
			--[[
			if derp.active_workspace.component_data.active_components then
				local rect = {x0,y0,x1,y1}
				for k,v in pairs(derp.active_workspace.component_data.active_components) do
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
				
				for k,v in pairs(derp.active_workspace.component_data.active_components) do
					v.x = v.x + dx
					v.y = v.y + dy
				end
				
				self.moved = true
			end
			]]
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
	
	local drawlocations = { top = { x = 20,y=40, resize_w = 0, resize_h = -draggies.drawbox.h+1, location = "top"},
							bottom = { x = 20, y = 40, resize_w = 0, resize_h = -draggies.drawbox.h+1, location = "bottom"} } 
	draggies.drawlocation = drawlocations.top
	
	function draggies:mousepush(mx,my)
		gui.widgets:addwidget(move_container)
		
		self.parent:needsredraw()
		self.parent.prev_owner = self.parent.parent
		self.parent.parent:removewidget(self.parent)
		
		self.parent.parent:resize_callback(self.drawlocation.resize_w,self.drawlocation.resize_h)
		
		gui.widgets:addwidget(self.parent)
		
		self.parent:move_abs(mx,my)
	end
	
	function draggies:mousedrag(mx,my)
	  self.parent:needsredraw()
		self.parent:move_relative(mx,my)
		self.parent:needsredraw()
	end
	
	function draggies:mouserelease(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
			
			local x,y = inp.getmousepos()
			local hit = move_container:find_mousehit(x,y)
			
			if hit.widget_type == "top arrow" then
				self.drawlocation = drawlocations.top
			elseif hit.widget_type == "left arrow" then
				self.drawlocation = drawlocations.left
			end
			
			gui.widgets:removewidget(self.parent)
			gui.widgets:removewidget(move_container)
			

			table.insert(self.parent.prev_owner.childwidgets,self.parent)
			gui:set_focus(self.parent)
			
			-- determine where to put toolbar, for now just put it back..
			self.parent:move_abs(self.drawlocation.x,self.drawlocation.y)
			self.parent.parent:resize_callback(-self.drawlocation.resize_w,-self.drawlocation.resize_h,self.drawlocation.location)
			
			self.parent.parent:needsredraw()
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
			
			--move_container:draw(force)
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
