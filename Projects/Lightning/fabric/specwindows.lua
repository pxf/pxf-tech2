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
  local tool_window = gui:create_window("tool_window", settings.data.toolpos[1],settings.data.toolpos[2],300,200, false, "Lightning Demo", true)
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
  
  local tool_stack = gui:create_verticalstack(10,5,400,20)
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
  
  -- spacing
  local render_button = gui:create_labelbutton(0,0,120,32,"Render",function () print(tostring(startrender("localhost", 50002))) end)
  tool_stack:addwidget(render_button)
  
  -- spacing
  local spacer = gui:create_basewidget(0,0,1,8)
  tool_stack:addwidget(spacer)
  
  -- progressbar
  local progress = gui:create_progressbar(0,0,280,12,0.0)
  tool_stack:addwidget(progress)
  
  -- progress blocks label
  local progress_label_blocks = gui:create_centeredlabelpanel(0,0,300,16,"Blocks: -/-")
  tool_stack:addwidget(progress_label_blocks)
  
  -- progress time est
  local progress_label_time = gui:create_centeredlabelpanel(0,0,300,16,"- ms")
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
