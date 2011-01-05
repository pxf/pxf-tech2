require("fabric/guibase")
require("fabric/settingshandler")

----------------------------------------------
-- settings
settings = new_settings_handler("settings.ini", {rendersize = 512,
                                                 scale = 1,
                                                 toolpos = {0,0},
                                                 toolbarstate = "full"
                                                })
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

local testtex = gfx.newtexture(8, 128, 128)
local testfbo = gfx.newframebuffer()
local a = 0

function update()
  if inp.iskeydown(inp.ESC) then
    app.quit()
  end
  if inp.iskeydown(inp.F1) then
    print(app.framestats())
  end
  a = a + 0.001
end

function draw(force)
  gfx.setclearcolor(255,0,255)
  testfbo:attach(testtex, 1)
  
  gfx.bindframebuffer(testfbo)
  gfx.clear()
  gfx.setview(0,0,128,128)
  gfx.setortho(0,128,0,128)
  gfx.bindshader(testshader)
  
  gfx.bindtexture(runtimeerror_tex)
  gfx.drawtopleft(0, 0, 128, 128)
  
  gfx.bindshader()
  gfx.bindframebuffer()
  
  testfbo:detach(1)
  
  gfx.setclearcolor(46.0/255.0,46.0/255.0,46.0/255.0)
  gfx.bindframebuffer()
  gfx.clear()
  --testfbo:detach(1)
  --gfx.drawcentered(app.width / 2, app.height / 2 - 128,256,256)
  --gfx.drawcentered(app.width / 2, app.height / 2,256,256)
  gfx.bindtexture(testtex)
  gfx.drawcentered(128 + math.cos(a)*32, 128 + math.sin(a)*32,256,256)

end
