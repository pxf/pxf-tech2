derp = {}
derp.active_workspace = nil
derp.active_tool = { last = nil, current = nil }

function derp:printstack()
	print("\n-------------\n")
	for k,v in pairs(self.active_workspace.workspace_stack.stack) do
		print("k == " .. k)
		print(v)
		print("\n-------------\n")
	end
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
	
	local socket_a = nil
	
	cam.shortcuts = { 	{ name = "copy", keys = {inp.LCTRL, "C"}, was_pressed = false, onpress = function () print("Lets copy dat floppy!") end},
						{ name = "deselect", keys = {inp.ESC}, was_pressed = false, onpress = function () derp.active_workspace:set_activecomp(nil) end },
						{ name = "select move", keys = {"A"}, was_pressed = false, onpress = function () derp:set_activetool(move_select) end },
						{ name = "square select", keys = {"M"}, was_pressed = false, onpress = function () derp:set_activetool(select_rect) end },
						{ name = "shift drag", keys = {inp.LSHIFT}, mouse = {inp.MOUSE_LEFT}, was_pressed = false, 
							onpress = function () 
								-- override current tool
								derp:set_activetool(nil)
								local mx,my = inp.getmousepos()
								local x,y = cam:coord_transform(mx,my)
								socket_a = derp.active_workspace:socket_hittest(x,y)
							end,
							onrelease = function () 
								-- local hit
								derp:set_activetool(derp.active_tool.last)
								local mx,my = inp.getmousepos()
								local x,y = cam:coord_transform(mx,my)
								local socket_b = derp.active_workspace:socket_hittest(x,y)
								
								derp.active_workspace:add_connection(socket_a,socket_b)
								socket_a = nil
							end },
						{ name = "move ws",keys = {inp.SPACE}, was_pressed = false, 
							onpress = function () 
								derp:set_activetool(move_ws) 
							end, 
							onrelease = function () 
								derp:set_activetool(derp.active_tool.last)
							end },
						{ name = "redo",keys = {inp.LCTRL, inp.LSHIFT, "Z"}, was_pressed = false, onpress = function () print("REDO") derp.active_workspace:redo() end},
						{ name = "undo",keys = {inp.LCTRL, "Z"}, was_pressed = false, onpress = function () print("UNDO") derp.active_workspace:undo() end},
						{ name = "delete", keys = {inp.BACKSPACE}, was_pressed = false, 
							onpress = function ()  
								if #derp.active_workspace.component_data.active_widgets > 0 then
									local save_ws = false
								
									for i,wid in pairs(derp.active_workspace.component_data.active_widgets) do
										for k,v in pairs(derp.active_workspace.component_data.nodes) do
											if wid.id == v.id then
												table.remove(derp.active_workspace.component_data.nodes,k)
												save_ws = true
											end
										end
									end
									
									derp.active_workspace.component_data.active_widgets = { }
									
									if save_ws then
										derp:push_workspace(derp.active_workspace)
									end
								end
							end},
					}
	
	function cam:mousedrag(mx,my)
		if derp.active_tool.current then 
			derp.active_tool.current:action({tag = "drag", dx = mx, dy = my})
		end
	end
	
	function cam:mousepush(mx,my,button)
		local x,y = self:coord_transform(mx,my)
		
		self.parent:mousepush(x,y,button)
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

function derp:create_workspace(x,y,w,h,from_path)
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	--wid.active_widget = nil
	wid.cam = derp:create_workspacecamera(0,0,w,h)
	wid:addwidget(wid.cam)
	
	wid.component_data = { active_widgets = {}, nodes = {} , edges = { } }
	wid.saved = true
	wid.id_counter = 0
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
			if socket_a.type == "output" and socket_b.type == "input" and socket_a.comp ~= socket_b.comp then
				socket_a.comp.outputs[socket_a.socket] = 1
				socket_b.comp.inputs[socket_b.socket] = 1
				
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
					draw_spline({{x0,y0},{x0 + (x1 - x0)* 0.25, y0},{x1 - (x1 - x0)*0.25,y1},{x1,y1}}, 60,2)
					gfx.setcolor(r,g,b)
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
			else
				self.component_data = { active_widgets = {}, nodes = {}, edges = {}}
			end
			
			--derp:printstack()
		end
	end
	
	function wid:redo()
		if self.workspace_stack.counter ~= #self.workspace_stack.stack then
			self.workspace_stack.counter = self.workspace_stack.counter + 1
			
			local ws = self.workspace_stack.stack[self.workspace_stack.counter]
			
			self.component_data = loadstring("return" .. ws)()
			
			--derp:printstack()
		end
	end
	
	function wid:addcomponent(x,y,ctype)
		local new_comp = { x = x, y = y, w = 100, h = 100, type = ctype,id = self.id_counter, inputs = { 0,0,0 }, outputs = { 0,0,0 } }
		table.insert(self.component_data.nodes,new_comp)
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
					return {comp = hit,type = "input",socket = socket_id} --hit.inputs[socket_id] { cid = hit.id, 
				end
			elseif mx <= (hit.x + hit.w * 0.5) and mx >= (hit.x + hit.w * 0.5 - 16) then
				if socket_id <= #hit.outputs then
					return {comp = hit,type = "output",socket = socket_id} --hit.outputs[socket_id]
				end
			end
		else
			return nil
		end
	end
	
	function wid:custom_hittest(mx,my)		
		for i = #self.component_data.nodes, 1, -1 do
			local v = self.component_data.nodes[i]
			
			if v then
				if mx >= (v.x - v.w * 0.5) and mx <= (v.x + v.w * 0.5) 
					and my >= (v.y - v.h * 0.5) and my <= (v.y + v.h * 0.5) then
					return v
				end
			end
		end
		
		return nil
	end
	
	function wid:mousepush(mx,my,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mousepush",x = mx,y = my})
		end
	end
	
	local cp = 0
	
	function wid:mouserelease(x,y,button)
		if derp.active_tool.current then
			derp.active_tool.current:action({tag = "mouserelease", x = x, y = y})
		end
	
		if (button == inp.MOUSE_RIGHT) then
			self:addcomponent(x,y,"render" .. cp)
			cp = cp + 1
		end
	end
	
	return wid
end

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
			end
			)
	redo = derp:base_tool(28,24,2,127,"redo",
			function () 
				derp:set_activetool(nil)
				if derp.active_workspace then
					derp.active_workspace:redo()
				end
			end)
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
			
			local cam = derp.active_workspace.cam
		
			cam:needsredraw()
			
			if ((cam.drawbox.x + mx) > 20) then
				cam.drawbox.x = 20
				cam.hitbox.x = 20
			elseif ((cam.drawbox.x + mx) < (-cam.drawbox.w + 752)) then
				cam.drawbox.x = -cam.drawbox.w + 752
				cam.hitbox.x = -cam.drawbox.w + 752
			else
				cam:move_relative(mx,0)
			end
			
			if ((cam.drawbox.y + my) > 102) then
				cam.drawbox.y = 102
				cam.hitbox.y = 102
			elseif ((cam.drawbox.y + my) < (-cam.drawbox.h + 576)) then
				cam.drawbox.y = -cam.drawbox.h + 576
				cam.hitbox.y = -cam.drawbox.h + 576
			else
				cam:move_relative(0,my)
			end

			cam:needsredraw()
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
			
			
			derp.active_workspace.component_data.active_widgets = nil
			
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
		if action.tag == "mousepush" then
			if not derp.active_workspace.component_data.active_widgets then
				local hit = derp.active_workspace:custom_hittest(action.x,action.y) 
				
				if hit then
					derp.active_workspace:set_activecomp({hit})
				end	
			end
		elseif action.tag == "drag" then
			if derp.active_workspace.component_data.active_widgets then
				for k,v in pairs(derp.active_workspace.component_data.active_widgets) do
					v.x = v.x + action.dx
					v.y = v.y + action.dy
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
		self.parent:move_relative(mx,my)
		self.parent.drag = true
		
		if not self.parent.drag_removed then
		  self.parent.prev_owner = self.parent.parent
			self.parent.parent:removewidget(self.parent)
			
			self.parent.parent:resize_callback(0,-wid.drawbox.h+1)
			gui.widgets:addwidget(self.parent)
			
			x,y = inp.getmousepos()
			self.parent:move_abs(x,y)
			
			self.parent.drag_removed = true
		end
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
