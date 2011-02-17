function gui:create_stackdivider(x,y,w,h)
  local wid = gui:create_basewidget(x,y,w,h)
  
  wid.sdraw = wid.draw
  
  function wid:draw(force)
    if (self.redraw_needed or force) then
      gfx.translate(self.drawbox.x, self.drawbox.y)
    
      -- line
			gfx.drawtopleft(2,self.drawbox.h / 2 + 2,self.drawbox.w-4,1, 1,5,1,1)
      
      gfx.translate(-self.drawbox.x, -self.drawbox.y)
		end
  end
  
  return wid
end

function spawn_error_dialog(msg)
  local error_dialog = gui:create_window("errordialog"..msg[1], app.width / 2 - 250,app.height / 2 - 75, 500,120, true, "Error")
  local msg_panel = gui:create_centeredmultiline_label(0,10,500,100,msg)
  error_dialog.panel:addwidget(msg_panel)
  
  -- ok button
  local button = gui:create_labelbutton(210,60,90,30,"Ok", function(self) self.parent.parent:destroy() end)
  error_dialog.panel:addwidget(button)

  gui.windows:add(error_dialog)
end

function spawn_toolwindow()
  --local tool_window = gui:create_window("tool_window", settings.data.toolpos[1],settings.data.toolpos[2],300,450, false, "Lightning Ray-Tracer", true)
  local tool_window = gui:create_scrollable_panel(app.width-300-5,0,300,app.height*2)
  local tool_scroller = gui:create_horizontal_scroll(app.width-5,0,5,app.height, function (self, value) tool_window:move_abs(app.width-300-5, -value*(tool_window.drawbox.h/2)) end)
  gui.widgets:addwidget(tool_scroller)
  gui.widgets:addwidget(tool_window)
  
  tool_window.scroll = function (self, mx, my)
    local deltaval = tool_scroller.value - my / (self.drawbox.h / 2)
    tool_scroller:setvalue(deltaval)
  end
  
  -- store move pos
  tool_window.s_mousedrag = tool_window.mousedrag
  --tool_window.label.s_mousedrag = tool_window.label.mousedrag
  --tool_window.panel.s_mousedrag = tool_window.panel.mousedrag
  --[[function tool_window:mousedrag(dx,dy,button)
    self:s_mousedrag(dx,dy,button)
    settings.data.toolpos[1] = tool_window.drawbox.x
    settings.data.toolpos[2] = tool_window.drawbox.y
  end]]
  --tool_window.label.mousedrag = tool_window.mousedrag
  --tool_window.panel.mousedrag = tool_window.mousedrag
  
  -- tool panel draw:
  tool_window.s_draw = tool_window.draw
  function tool_window:draw(force)
    if (self.redraw_needed or force) and (self.visible) then
  		gfx.translate(self.drawbox.x,self.drawbox.y)
  		
      -- bg
  		local a = gfx.getalpha()
  		gfx.setalpha(0.9)
  		gfx.drawtopleft(0, 0, self.drawbox.w, self.drawbox.h, 2, 2, 1, 1)
  		gfx.setalpha(a)
  		
  		gfx.translate(-self.drawbox.x,-self.drawbox.y)
  	end
		
		self:s_draw(force)
		
  end
  
  local tool_stack = gui:create_verticalstack(10,5,300,20)
  tool_window:addwidget(tool_stack)
  
  -- file menu
  local file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)", onclick = 
  									function () 	
  										app.reboot() 
  									end}},
  									{"Save Settings", {tooltip = "Saves settings to disk.", onclick = function () settings:save() end}},
                     {"Quit", {tooltip = "Quit the application.", shortcut = "Esc", onclick = function () settings:save(); app.quit() end}},
             }
  
  -- menu bar
  local menubar = gui:create_menubar(0,0,300,{{"File",file_menu}})
  menubar.widget_type = "menubar"
  tool_stack:addwidget(menubar)
  
  -- interleaved label
  local interleaved_label = gui:create_labelpanel(0,0,280,26,"Interleaved feedback:")
  tool_stack:addwidget(interleaved_label)
  
  -- interleaved-input stack
  local interleaved_inputs = gui:create_horizontalstack(0,0,280,32)
  tool_stack:addwidget(interleaved_inputs)
  
  -- interleaved slider
  local interleaved_value = gui:create_labelpanel(0,0,20,26,"1")
  local interleaved_slider = gui:create_slider(0,0,260,20,1,5,function (self) interleaved_value.label_text = tostring(self.value*self.value) end, true)
  interleaved_slider:setvalue(1)
  interleaved_inputs:addwidget(interleaved_slider)
  
  -- interleaved label
  interleaved_inputs:addwidget(interleaved_value)
  
  
  
  
  -- task count label
  local taskcount_label = gui:create_labelpanel(0,0,280,26,"Task count:")
  tool_stack:addwidget(taskcount_label)
  
  -- task count-input stack
  local taskcount_inputs = gui:create_horizontalstack(0,0,280,32)
  tool_stack:addwidget(taskcount_inputs)
  
  -- task count slider
  local taskcount_value = gui:create_labelpanel(0,0,60,26, tostring(2^settings.data.gridcount) .. "x" .. tostring(2^settings.data.gridcount))
  local taskcount_slider = gui:create_slider(0,0,220,20,1,8,function (self) settings.data.gridcount = self.value; settings:save() taskcount_value.label_text = tostring(2^self.value) .. "x" ..  tostring(2^self.value) end, true)
  taskcount_slider:setvalue(settings.data.gridcount)
  taskcount_inputs:addwidget(taskcount_slider)
  
  -- task count label
  taskcount_inputs:addwidget(taskcount_value)
  
  -- load model button
  local load_model_button = gui:create_labelbutton(128,0,120,32,"Load Model",
		function () 
			local path = app.opendialog() 
			loadmodel(path)
		end)
  tool_stack:addwidget(load_model_button)
  
  
  
  -- spacing
  local spacer = gui:create_stackdivider(0,0,280,20)
  tool_stack:addwidget(spacer)
  
  -- label for host input fields
  local host_label = gui:create_labelpanel(0,0,280,26,"Render host:")
  tool_stack:addwidget(host_label)
  
  -- host-input stack
  local host_inputs = gui:create_horizontalstack(0,0,280,32)
  tool_stack:addwidget(host_inputs)
  
  -- ip input
  local ip_input = gui:create_textinput(0,0,200,false,settings.data.clienthost, function (self) settings.data.clienthost = tostring(self.value); settings:save(); print("changed to: " .. tostring(self.value)) end)
  host_inputs:addwidget(ip_input)
  
  -- label divider between ip and port fields
  local input_divider = gui:create_labelpanel(0,0,22,20,":")
  host_inputs:addwidget(input_divider)
  
  -- port input
  local port_input = gui:create_textinput(0,0,58,false,settings.data.clientport, function (self) settings.data.clientport = tostring(self.value); settings:save(); print("changed to: " .. tostring(self.value)) end)
  host_inputs:addwidget(port_input)
  
  
  -- label for localhost input fields
  local localhost_label = gui:create_labelpanel(0,0,280,26,"Receive address and port:")
  tool_stack:addwidget(localhost_label)
  
  -- localhost-input stack
  local localhost_inputs = gui:create_horizontalstack(0,0,280,32)
  tool_stack:addwidget(localhost_inputs)
  
  -- ip input
  local localip_input = gui:create_textinput(0,0,200,false,settings.data.localhost, function (self) settings.data.localhost = tostring(self.value); settings:save(); print("changed to: " .. tostring(self.value)) end)
  localhost_inputs:addwidget(localip_input)
  
  -- label divider between ip and port fields
  local localinput_divider = gui:create_labelpanel(0,0,22,20,":")
  localhost_inputs:addwidget(localinput_divider)
  
  -- port input
  local localport_input = gui:create_textinput(0,0,58,false,settings.data.localport, function (self) settings.data.localport = tostring(self.value); settings:save(); print("changed to: " .. tostring(self.value)) end)
  localhost_inputs:addwidget(localport_input)
  
  -- button stack
  local buttons_stack = gui:create_horizontalstack(0,0,280,46)
  tool_stack:addwidget(buttons_stack)
  
  -- stop button
  local stop_button = gui:create_labelbutton(0,0,80,32,"Stop", function (self) 
    stoprender()
    self.enabled = false
  end)
  stop_button.enabled = false
  
  -- render button
  local render_button = gui:create_labelbutton(0,0,120,32,"Render", function (self) 
    print("Trying to send job to: " .. tostring(ip_input.value) .. ":" .. tostring(port_input.value))
    local succ, msg = startrender(tostring(ip_input.value), tostring(port_input.value),           -- remote client
                                  tostring(localip_input.value), tostring(localport_input.value), -- result ip:port
                                  interleaved_slider.value*interleaved_slider.value,              -- interleaved feedback
                                  2^taskcount_slider.value)                                         -- task count
    if not succ then
      spawn_error_dialog({msg})
    else
      stop_button.enabled = true
    end
  end)
  buttons_stack:addwidget(render_button)
  
  buttons_stack:addwidget(stop_button)
  
  -- divider
  local spacer = gui:create_stackdivider(0,0,280,20)
  tool_stack:addwidget(spacer)
  
  -- label for render progress
  local progress_label = gui:create_labelpanel(0,0,280,26,"Render progress:")
  tool_stack:addwidget(progress_label)
  
  -- progressbar
  local progress = gui:create_progressbar(0,0,280,12,0.0)
  tool_stack:addwidget(progress)
  
  -- progress blocks label
  local progress_label_blocks = gui:create_centeredlabelpanel(0,0,300,16,"")
  tool_stack:addwidget(progress_label_blocks)
  
  -- progress time est
  local progress_label_time = gui:create_centeredlabelpanel(0,0,300,16,"")
  tool_stack:addwidget(progress_label_time)
  
  local spacer = gui:create_stackdivider(0,0,280,20)
  tool_stack:addwidget(spacer)
  
  -- client results list
  local client_res_list = gui:create_multiline_label(0,0,300,300,{"-"})
  tool_stack:addwidget(client_res_list)

  -- fancify time
  function fancy_time(in_ms)
    local s = in_ms / 1000 % 60
    local m = (in_ms / 1000) / 60
    --local h = math.floor(s % 60)
    m = math.floor(m)
    s = math.floor(s)
    
    return (tostring(m) .. "m " .. tostring(s) .. "s")
  end

  -- update func
  old_time = 0
  tool_window.s_update = tool_window.update
  function tool_window:update()
    self:s_update()
    
    -- get clients and result
    local clientres = clientsstatus()
    local clientres_labellist = {}
    local clientres_count = 1
    for k,v in pairs(clientres) do
      table.insert(clientres_labellist, "#" .. tostring(clientres_count) .. " " .. tostring(k) .. " (^(1,0,0){" .. tostring(v) .. "})")
      clientres_count = clientres_count + 1
    end
    client_res_list.lines = clientres_labellist
    
    local done,total,time = renderstatus()
    
    progress.progress = done / total
    
    if (done == 0) then
      
      progress_label_blocks.label_text = ""
      
    else    
    
      if not (old_done == done) then
        old_done = done
      
        est_time = 0
        if (done > 0) then
          local delta_time_block = time / done
          est_time = (total - done) * delta_time_block
        end
      
        old_time = time
      
        progress_label_blocks.label_text = "Blocks: " .. tostring(done) .. "/" .. tostring(total)
        if (done == total) then
          progress_label_blocks.label_text = "Done!"
        end
      end
    
      local delta_time = time - old_time
      if (done == total) then
        progress_label_time.label_text = "Total time: " .. fancy_time(time)
      else
        progress_label_time.label_text = fancy_time(time) .. " (est. " .. fancy_time(est_time - delta_time) .. ")"
      end
    
    end
    
    --print(done, total, done / total, time)
  end
  
  -- add settings saving for minimize button

  --[[
  if (settings.data.toolbarstate == "compact") then
    tool_window:toggle_state()
  end
  tool_window.s_toggle_state = tool_window.toggle_state
  function tool_window:toggle_state()
    tool_window:s_toggle_state()
    settings.data.toolbarstate = tool_window.state
    settings:save()
  end]]
  --gui.windows:add(tool_window)
  
  return tool_window, tool_scroller
end
