print("This is the editor!")

test_texture = gfx.loadtexture("data/apperror.png")

function _update()
  gfx.redrawneeded()
end

function _draw()
  _draw = nil
  gfx.bindtexture(test_texture)
  gfx.drawcentered(300,300,128,64)
end


