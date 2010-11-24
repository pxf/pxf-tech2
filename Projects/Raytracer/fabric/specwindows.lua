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
  local tool_window = gui:create_window("tool_window", app.width / 2 - 200,app.height / 2 - 200,400,400, false, "Lightning Demo", true)
  local tool_stack = gui:create_verticalstack(10,5,400,20)
  tool_window.panel:addwidget(tool_stack)
  
  -- file menu
  local file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)", onclick = 
  									function () 	
  										app.reboot() 
  									end}},
          {"Quit", {tooltip = "Quit the application.", shortcut = "Esc", onclick = function () app.quit() end}},
             }
  
  -- menu bar
  local menubar = gui:create_menubar(0,0,300,{{"File",file_menu}})
  menubar.widget_type = "menubar"
  tool_stack:addwidget(menubar)
  
  -- reboot button
  local reboot_button = gui:create_labelbutton(0,0, 200, 32, "Reboot application", function() app.reboot() end)
  tool_stack:addwidget(reboot_button)
  
  gui.windows:add(tool_window)
end
