derp = {}
derp.active_workspace = nil
derp.active_tool = nil

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
	local file = io.output(filename .. ".derp","w")
	local data = basic_serialize(workspace.component_data,0)
	
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


function derp:editor()
	
end

function derp:set_activetool(tool)
	if not tool then
		return
	end
	
	if self.active_tool then
		self.active_tool.selected = false
		self.active_tool:needsredraw()
		self.active_tool = tool
		tool.selected = true
	else
		self.active_tool = tool
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
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "statusbar"
	
	function wid:draw(force) 
		if (self.redraw_needed or force) then
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,1,1,1)	
			
			-- DRAW BORDER
			gfx.drawtopleft(self.drawbox.x+20,self.drawbox.y,self.drawbox.w-40,1,1,5,1,1)
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
		
		function ws:mouserelease(mx,my,button)
			if (button == inp.MOUSE_LEFT) then
				if (derp.active_workspace and not (derp.active_workspace == self) ) then
					derp.active_workspace.active = false
					derp.active_workspace = self
					self.active = true
				end
			end
			
			self:needsredraw()
		 end
		
		self:addwidget(ws)
		derp.active_workspace = self
		
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

	function cam:mousedrag(mx,my)
		self:needsredraw()
		self:move_relative(mx,my)
		self:needsredraw()
	end
	
	
	function cam:mouserelease(mx,my,button)
		self.parent:mouserelease(mx,my,button)
	end
	
	function cam:draw(force)
		if (force or self.redraw_needed) then
			gfx.setalpha(0.25)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,1,5,1,1) -- solid bg
			gfx.setalpha(1.0)
			
			local old_tex = gfx.bindtexture(checkers_texture)
			gfx.drawtopleft(self.drawbox.x,self.drawbox.y,self.drawbox.w,self.drawbox.h,0,0,500,500*0.75)	-- checkers
			gfx.bindtexture(old_tex)

			--gfx.drawtopleft(self.drawbox.w * 0.5,self.drawbox.h * 0.5,5,5,5,5,1,1) -- solid bg

			--self:super_draw(force)
		end
	end
	
	return cam
end

function derp:create_workspace(x,y,w,h,from_path)
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	wid.active_widget = nil
	wid.cam = derp:create_workspacecamera(0,0,w,h)
	wid:addwidget(wid.cam)
	
	wid.component_data = {}
	wid.saved = true
	wid.id_counter = 0
	
	if from_path then
		wid.component_data = derp:load(from_path)
	end
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			self.cam:draw(force)
			
			gfx.translate(self.cam.drawbox.w*0.5 + self.cam.drawbox.x,self.cam.drawbox.h*0.5 + self.cam.drawbox.y)
			
			for k,v in pairs(self.component_data) do
				gfx.drawcentered(v.x,v.y,100,100,17,5,1,1)
			end
			
			gfx.translate(-self.cam.drawbox.w*0.5 - self.cam.drawbox.x,-self.cam.drawbox.h*0.5 - self.cam.drawbox.y)
		end
	end
	
	function wid:addcomponent(x,y,ctype)
		table.insert(self.component_data,{ x = x, y = y, type = ctype,id = self.id_counter })
		print("new " .. ctype .. " component created at " .. x .. "," .. y .. " with id " .. self.id_counter)
		self.id_counter = self.id_counter + 1
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
	end
	
	function wid:custom_hittest(mx,my)
		local chit = nil
		local x,y
		
		for k,v in pairs(self.component_data) do
			x = self.cam.drawbox.w*0.5 + self.cam.drawbox.x + v.x
			y = self.cam.drawbox.h*0.5 + self.cam.drawbox.y + v.y
		end
		
		return chit
	end
	
	function wid:mouserelease(x,y,button)
		-- custom hit test on components
	
		if (button == inp.MOUSE_LEFT) then
			
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

function derp:base_tool(x,y,w,h,name)
	local tool = gui:create_basewidget(x,y,w,h)	
	tool.highlight = false
	tool.selected = false
	tool.super_find_mousehit = tool.find_mousehit
	tool.icon_properties = { w = w, h = h, s = nil,t = nil }
	
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
		
			if self.selected then
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
				return
			end
		
			self:needsredraw()
			
			derp:set_activetool(self)
			
			self:needsredraw()
		end
	end
	
	return tool
end

--local shortcuts = { { "save", function () end, {inp.MOUSE_LEFT, inp.MOUSE_RIGHT}}} 

function derp:create_toolbar(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	local draggies = gui:create_basewidget(0,0,25,40)
	local separator = gui:create_basewidget(0,0,10,40)
	
	-------- TOOLS --------
	local undo = derp:base_tool(0,0,40,40,"undo")
	local redo = derp:base_tool(0,0,40,40,"redo")
	local select_rect = derp:base_tool(0,0,40,40,"square select")
	local move_select = derp:base_tool(0,0,40,40,"move/select")
	
	undo.icon_properties.w = 28
	undo.icon_properties.h = 24
	undo.icon_properties.s = 1
	undo.icon_properties.t = 102
	
	redo.icon_properties.w = 28
	redo.icon_properties.h = 24
	redo.icon_properties.s = 2
	redo.icon_properties.t = 127
	
	move_select.icon_properties.w = 24
	move_select.icon_properties.h = 17
	move_select.icon_properties.s = 1
	move_select.icon_properties.t = 61
	
	select_rect.icon_properties.w = 24
	select_rect.icon_properties.h = 24
	select_rect.icon_properties.s = 1
	select_rect.icon_properties.t = 36
	
	self:set_activetool(move_select)
	
	-----------------------
	
	draggies.super_draw = draggies.draw
	draggies.widget_type = "toolbar drag widget"
	
	wid.widget_type = "toolbar"
	wid.drag_removed = false
	wid.drag = false
	wid.prev_owner = nil
	
	wid:addwidget(draggies)
	wid:addwidget(undo)
	wid:addwidget(redo)
	wid:addwidget(separator)
	wid:addwidget(move_select)
	wid:addwidget(select_rect)
	
	function select_rect:action(action)
		
	end
	
	function move_select:action(action)
		if action.drag then
			print("drag")
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
