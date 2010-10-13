function spawn_aboutwindow()
  local short_info = { "^(1,0.4,0.4){D}^(0.4,1,0.4){E}^(0.4,0.4,1){R}^(1,0.4,0.4){P} Editor is a bla bla bla,"
                     , "yada yada yada."
                     , "You might need to take a dump in your bed."
                     }
  
  local about_window = gui:create_window(app.width / 2 - 200,app.height / 2 - 125,400,250, true, "About")
  local shortinfo_panel = gui:create_centeredmultiline_label(0,0,400,60,short_info)
  local memory_panel = gui:create_labelpanel(0,60,400,20,"Memory usage: unknown")
  
  about_window.panel:addwidget(shortinfo_panel)
  about_window.panel:addwidget(memory_panel)
  
  -- test memory stuff
  memory_panel.last_update = os.clock()
  function memory_panel:update()
    if (os.clock() - self.last_update > 0.1) then
      memory_panel.last_update = os.clock()
      memory_panel.label_text = "Memory usage: ^(0.4,0.4,1){" .. string.sub(tostring(app.getmemusage()/1024/1024), 1, 5) .. "}MB"
    end
  end
  
  gui.windows:add(about_window)
end

function spawn_settingswindow()
  local settings_window = gui:create_window(200,120,400,250, true, "Application Settings")
  --local shortinfo_panel = gui:create_centeredmultiline_label(0,0,400,60,short_info)
  --about_window.panel:addwidget(shortinfo_panel)
  
  gui.windows:add(settings_window)
end
