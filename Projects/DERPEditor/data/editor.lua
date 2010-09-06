require("data/guibase")

print("This is the editor!")

-- init GUI
gui:init()

--test_texture = gfx.loadtexture("data/apperror.png")

function update()
  gfx.redrawneeded()
  gui:update()
end

function draw()
  --gfx.bindtexture(test_texture)
  --gfx.drawcentered(300,300,256,128, 0, 0, 256, 128)
  --gfx.drawcentered(300,300,256,128)
  --gfx.setcolor(1, 0, 0)
  panic.text("Welcome to ^1D^2E^1R^2P^9 Editor!", 200, 200)
  --poooooooooop()
  
  gui:draw()
end
