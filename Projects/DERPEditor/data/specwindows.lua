function spawn_aboutwindow()
  local about_window = gui:create_movablewindow(100,100,300,300, "About")
  about_window:addwidget(gui:create_labelpanel(20,20,200,200,"sup?"))
  
  gui.widgets:addwidget(about_window)
end
