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

function spawn_gamemenu(game)
  local tool_window = gui:create_window("tool_window", (512-200)/2, 20,200,150, false, "pxf-finalsolution", true)
  
  -- store move pos
  tool_window.s_mousedrag = tool_window.mousedrag
  tool_window.label.s_mousedrag = tool_window.label.mousedrag
  tool_window.panel.s_mousedrag = tool_window.panel.mousedrag
  function tool_window:mousedrag(dx,dy,button)
    self:s_mousedrag(dx,dy,button)
  end
  tool_window.label.mousedrag = tool_window.mousedrag
  tool_window.panel.mousedrag = tool_window.mousedrag
  local tool_stack = gui:create_verticalstack(10,5,200,20)
  tool_window.panel:addwidget(tool_stack)
  
  -- file menu
  local file_menu = {{"Reboot", {tooltip = "Reboots the application. (Reloads all scripts and textures.)"
                                ,onclick = function () app.reboot() end}},
  									 {"Save Settings",{tooltip = "Saves settings to disk."
                                      ,onclick = function () settings:save() end}},
                     {"Quit", {tooltip = "Quit the application."
                              ,shortcut = "Esc"
                              ,onclick = function () settings:save(); app.quit() end}}}
  -- menu bar
  local menubar = gui:create_menubar(0,0,300,{{"File",file_menu}})
  menubar.widget_type = "menubar"
  tool_stack:addwidget(menubar)
  
  btn_start = gui:create_labelbutton(25,0, 180, 25, "Start game",
                function () 
                    print("Start game")
                    snd.playsound(music, true)
                    game.game = create_new_game()
                end)
  btn_credz = gui:create_labelbutton(25,0, 180, 25, "Credits",
                function () 
                  print("Credits")
                end)
  btn_quit = gui:create_labelbutton(25,0, 180, 25, "Quit",
                function () 
                  app.quit()
                end)
  
  local spacer = gui:create_stackdivider(0,0,150,4)
  tool_stack:addwidget(btn_start)
  tool_stack:addwidget(spacer)
  tool_stack:addwidget(btn_credz)
  tool_stack:addwidget(spacer)
  tool_stack:addwidget(btn_quit)
    
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
