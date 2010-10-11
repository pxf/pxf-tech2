function spawn_aboutwindow()
  local about_window = gui:create_movablewindow(100,100,300,300, false, "About")
  about_window:addwidget(gui:create_labelpanel(20,20,200,200,"sup?"))
  
  gui:addwindow(about_window)
end
