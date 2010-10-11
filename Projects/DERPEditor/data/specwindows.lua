function spawn_aboutwindow()
  local about_window = gui:create_window(100,100,300,300, true, "About")
  local memory_panel = gui:create_labelpanel(0,0,100,20,"Memory usage: ??")
  about_window.panel:addwidget(memory_panel)
  
  -- test memory stuff
  memory_panel.last_update = os.clock()
  function memory_panel:update()
    if (os.clock() - self.last_update > 0.1) then
      memory_panel.last_update = os.clock()
      memory_panel.label_text = "Memory usage: " .. string.sub(tostring(app.getmemusage()/1024/1024), 1, 5) .. "MB"
    end
  end
  
  gui.windows:add(about_window)
end
