require("fabric/settingshandler")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512})
settings:load()
settings:save()

----------------------------------------------
-- initial draw
gfx.redrawneeded()

-- shadertest
local testshader = gfx.createshader("testshader", [[

void main() {
  gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


]], [[
uniform sampler2D tex;
void main()
{
	// Setting Each Pixel To Red
	vec4 color = texture2D(tex, gl_TexCoord[0].st);
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * color;
}


]])


util.newgrid()
local testtex = gfx.newtexture(12, 128, 128)
local testfbo = gfx.newframebuffer()
testfbo:attach(testtex, 1)
local a = 0

function update()
  if inp.iskeydown(inp.ESC) then
    app.quit()
  end
  if inp.iskeydown(inp.F1) then
    print(app.framestats())
  end
  a = a + 0.004
end


function draw(force)
  
  gfx.bindframebuffer(testfbo)
  gfx.clear()
  gfx.setview(0,0,128,128)
  gfx.setortho(0,128,0,128)
  gfx.bindshader(testshader)
  
  gfx.bindtexture(runtimeerror_tex)
  gfx.translate(64, 64)
  gfx.rotate(a)
  gfx.drawcentered(0, 0, 128, 128)
  
  gfx.bindshader()
  gfx.bindframebuffer()
  
  gfx.loadidentity()
  gfx.clear()
  gfx.bindtexture(testtex)
  gfx.drawcentered(app.width / 2, app.height / 2 - 128,256,256)
  gfx.drawcentered(app.width / 2, app.height / 2,256,256)
  gfx.drawcentered(128 + math.cos(a)*32, 128 + math.sin(a)*32,256,256)

end
