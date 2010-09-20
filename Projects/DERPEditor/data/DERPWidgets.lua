derp = {}

function derp:window_container()
	wid = gui:create_verticalstack(0,0,app.width,app.height)
	wid.widget_type = "window container"
	
	--[[
	function wid:draw(force)
		if(self.redraw_needed or force) then
			gfx.bindtexture(gui.themetex)
			-- DRAW BG
			gfx.drawtopleft(self.drawbox.x, self.drawbox.y, self.drawbox.w, self.drawbox.h, 9, 1, 1, 1)
		end
	end
	]]
	
	return wid
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
			gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+1,self.drawbox.w-2,self.drawbox.h-2,1,1,1,1)
		
			-- DRAW BORDER
			-- TOP
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y,self.drawbox.w-2,1,1,5,1,1)
			-- BOTTOM
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.h-2,self.drawbox.w-2,1,1,5,1,1)
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
	end
	
	function wid:mousedrag(dx,dy,button)
		if button == inp.MOUSE_LEFT then
			self.drawbox.x = self.drawbox.x + dx
			self.drawbox.w = self.drawbox.w - dx
			self.hitbox.x = self.hitbox.x + dx
			self.hitbox.w = self.hitbox.w - dx
			
			if (not (self.parent == nil)) then
				--self.parent:child_resized(self)
			end
		end
	end
	
	return wid
end

function derp:create_workspace_tabs(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	wid.widget_type = "workspace tabs"
	wid.super_draw = wid.draw
	wid.active_workspace = nil
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			-- DRAW BG
			gfx.drawtopleft(#self.childwidgets*100 + self.drawbox.x + 1,self.drawbox.y,self.drawbox.w-1 - #self.childwidgets*100,self.drawbox.h,9,5,1,1)
			
			wid:super_draw(force)
		end
	end
	
	function wid:addworkspace(name)
		local ws = gui:create_basewidget(0,0,100,20)
		ws.widget_type = "workspace tab " .. #self.childwidgets
		ws.active = false
		ws.parent = self
		
		function ws:draw(force)
			if (self.redraw_needed or force) then
				
				-- DRAW BG
				if self.active then
					gfx.drawtopleft(self.drawbox.x + 1,0,self.drawbox.w-1,self.drawbox.h,13,1,1,1)
				else
					gfx.drawtopleft(self.drawbox.x + 1,0,self.drawbox.w-1,self.drawbox.h,13,5,1,1)
				end
				
				-- DRAW BORDERS
				gfx.drawtopleft(self.drawbox.x + 1,0,self.drawbox.w-1,1,1,5,1,1) -- TOP
				gfx.drawtopleft(self.drawbox.x + 1,self.drawbox.h-1,self.drawbox.w-1,1,1,5,1,1) -- BOTTOM
				gfx.drawtopleft(self.drawbox.x + 1,0,1,self.drawbox.h-1,1,5,1,1) -- LEFT
				gfx.drawtopleft(self.drawbox.x + self.drawbox.w-1,0,1,self.drawbox.h-1,1,5,1,1) -- RIGHT
				
				--gui:drawfont(name,self.drawbox.x + 10,self.drawbox.y + 10)
			end
		end
		
		function ws:update()

		end
		
		function ws:mouseover(mx,my)
		
		end
		
		function ws:mouserelease(mx,my,button)
			if (button == inp.MOUSE_LEFT) then
				self.parent.active_workspace = self
				for k,v in pairs(self.parent.childwidgets) do
					if not (v == self) then
						v.active = false
					end
				end
				
				self.active = true
			end
			
			self:needsredraw()
		 end
		
		self:addwidget(ws)
		
		return ws
	end
	
	return wid
end

function derp:create_workspace(x,y,w,h)
	local wid = gui:create_basewidget(x,y,w,h)
	wid.widget_type = "workspace"
	wid.super_draw = wid.draw
	
	local checkers_texture = gfx.loadtexture(64,"data/checkers.png")
	
	function wid:draw(force)
		if (self.redraw_needed or force) then
			-- DRAW BG
			
			gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+1+128,self.drawbox.w-2,self.drawbox.h-2-128,1,1,1,1) -- solid bg
			
			local old_tex = gfx.bindtexture(checkers_texture)
			gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+1,self.drawbox.w-2,self.drawbox.h-2,0,0,500,500*0.75)	-- checkers
			gfx.bindtexture(old_tex)
			
			--gfx.drawtopleft(self.drawbox.x+1,self.drawbox.y+1,self.drawbox.w-2,128,509,1,1,127)	-- gradient
		
			-- DRAW BORDER
			-- TOP
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,self.drawbox.w-2,1,1,5,1,1)
			-- BOTTOM
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y + self.drawbox.h-2,self.drawbox.w-2,1,1,5,1,1)
			-- LEFT
			gfx.drawtopleft(self.drawbox.x+1, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
			-- RIGHT
			gfx.drawtopleft(self.drawbox.x + self.drawbox.w-1, self.drawbox.y+1,1,self.drawbox.h-2,1,5,1,1)
			
		
			wid:super_draw(force)
		end
	end
	
	function wid:resize_callback(w,h)
		self.drawbox.h = self.drawbox.h - h
		self.hitbox.h = self.drawbox.h
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

function derp:create_toolbar(x,y,w,h)
	local wid = gui:create_horizontalstack(x,y,w,h)
	local draggies = gui:create_basewidget(0,0,12,40)
	local parent = nil
	
	draggies.super_draw = draggies.draw
	draggies.widget_type = "toolbar drag widget"
	
	wid:addwidget(draggies)
	wid.widget_type = "toolbar"
	wid.drag_removed = false
	wid.drag = false
	
	function draggies:draw(force)
		if (self.redraw_needed or force) then
			if self.drag then 
				gfx.setalpha(0.7)
			end
			
			gfx.drawtopleft(4,8,3,25,1,10,3,25)
			
			if self.drag then 
				gfx.setalpha(1.0)
			end
			
			draggies:super_draw()
		end
	end
	
	function draggies:mousedrag(mx,my)
		wid:move_relative(mx,my)
		
		if not wid.drag_removed then
			parent = wid.parent
			wid.parent:removewidget(wid)
			
			parent:resize_callback(0,-wid.drawbox.h+1)
			gui.widgets:addwidget(wid)
			
			x,y = inp.getmousepos()
			wid:move_abs(x,y)
			
			wid.drag_removed = true
			wid.drag = true
		end
	end
	
	function draggies:mouserelease(dx,dy,button)
		if (button == inp.MOUSE_LEFT) then
			wid.drag = false
			gui.widgets:removewidget(wid)
			
			wid.parent = parent
			table.insert(parent.childwidgets,0,wid)
			
			-- determine where to put toolbar, for now just put it back..
			wid:move_abs(0,0)
			parent:resize_callback(0,wid.drawbox.h-1)
			
			parent:needsredraw()
		end
	end
	
	wid.super_draw = wid.draw
	function wid:draw(force)	
		if (self.redraw_needed or force) then
			if self.drag then 
				gfx.setalpha(0.7)
			end
		
			-- DRAW BG
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
				gfx.drawtopleft(self.drawbox,self.drawbox.y + self.drawbox.h,self.drawbox.w,1,1,5,1,1)
			end
			
			self:super_draw(force)
		end
	end
	
	return wid
end
