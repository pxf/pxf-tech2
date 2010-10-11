function spawn_aboutwindow()
  local about_window = gui:create_window(100,100,300,300, true, "About")
  about_window.panel:addwidget(gui:create_labelpanel(20,20,200,200,"sup?"))
  
  gui.windows:add(about_window)
end
