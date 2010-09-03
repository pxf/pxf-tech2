print("This is the editor!")

test_texture = gfx.loadtexture("data/apperror.png")

function update()
  gfx.redrawneeded()
end

function draw()
  gfx.bindtexture(test_texture)
  gfx.drawcentered(300,300,128,64)
end


