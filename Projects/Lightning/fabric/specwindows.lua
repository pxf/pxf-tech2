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
  local tool_window = gui:create_window("tool_window", settings.data.toolpos[1],settings.data.toolpos[2],300,400, false, "Lightning Demo", true)
  -- store move pos
  tool_window.s_mousedrag = tool_window.mousedrag
  tool_window.label.s_mousedrag = tool_window.label.mousedrag
  tool_window.panel.s_mousedrag = tool_window.panel.mousedrag
  function tool_window:mousedrag(dx,dy,button)
    self:s_mousedrag(dx,dy,button)
    settings.data.toolpos[1] = tool_window.drawbox.x
    settings.data.toolpos[2] = tool_window.drawbox.y
  end
  tool_window.label.mousedrag = tool_window.mousedrag
  tool_window.panel.mousedrag = tool_window.mousedrag
  
  local tool_stack = gui:create_verticalstack(10,5,300,20)
  tool_window.panel:addwidget(tool_stack)
  
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
  local interleaved_value = gui:create_labelpanel(0,0,20,26,"0")
  local interleaved_slider = gui:create_slider(0,0,260,20,0,8,function (self) interleaved_value.label_text = tostring(self.value) end, true)
  interleaved_slider:setvalue(0)
  interleaved_inputs:addwidget(interleaved_slider)
  
  -- interleaved label
  interleaved_inputs:addwidget(interleaved_value)
  
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
  local ip_input = gui:create_textinput(0,0,200,false,"localhost", function (self) print("changed to: " .. tostring(self.value)) end)
  host_inputs:addwidget(ip_input)
  
  -- label divider between ip and port fields
  local input_divider = gui:create_labelpanel(0,0,22,20,":")
  host_inputs:addwidget(input_divider)
  
  -- port input
  local port_input = gui:create_textinput(0,0,58,false,"50002", function (self) print("changed to: " .. tostring(self.value)) end)
  host_inputs:addwidget(port_input)
  
  -- render button
  local render_button = gui:create_labelbutton(0,0,120,32,"Render", function () 
    print("Trying to send job to: " .. tostring(ip_input.value) .. ":" .. tostring(port_input.value))
    local succ, msg = startrender(tostring(ip_input.value), tostring(port_input.value), -- remote client
                                  "localhost", 4632,                                    -- result ip:port
                                  interleaved_slider.value)                             -- interleaved feedback
    if not succ then
      spawn_error_dialog({msg})
    end
  end)
  tool_stack:addwidget(render_button)
  
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

  
  if (settings.data.toolbarstate == "compact") then
    tool_window:toggle_state()
  end
  tool_window.s_toggle_state = tool_window.toggle_state
  function tool_window:toggle_state()
    tool_window:s_toggle_state()
    settings.data.toolbarstate = tool_window.state
    settings:save()
  end
  gui.windows:add(tool_window)
end
