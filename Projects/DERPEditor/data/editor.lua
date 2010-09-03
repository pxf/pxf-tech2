print("This is the editor!")

test_texture = gfx.loadtexture("data/apperror.png")

function update()
  gfx.redrawneeded()
end

function draw()
  --gfx.bindtexture(test_texture)
  --gfx.drawcentered(300,300,256,128, 0, 0, 256, 128)
  --gfx.drawcentered(300,300,256,128)
  draw_text("Welcome to DERP Editor!", 200, 200)
end


