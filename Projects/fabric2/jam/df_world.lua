require("jam/liq")

function create_dfworld( filepath )
  local dfworld = {tex = {gfx.loadtexture(1, filepath, true),
                          gfx.loadtexture(1, filepath, true)},
                   fbo = gfx.newframebuffer(),
                   --mudtex = gfx.loadtexture(1, "jam/", false)
                   active_tex = 1,
                   sec_tex = 2,
                   bindata = {},
                   size = {512, 512},
                   feathersize = 32}
                   
  -- constants
  dfworld.solidvalue = 128
  
  -- load binary data
  --dfworld.bindata = gfx.getrawimage(filepath)
  local bindata = gfx.getrawimage(filepath)
  for y=0,511 do
    for x=0,511 do
      if (tonumber(bindata[y*512*4+x*4+3]) >= dfworld.solidvalue) then
        dfworld.bindata[y*512+x] = true
      else
        dfworld.bindata[y*512+x] = false
      end
    end
  end
  
  -- function to test intersection
  function dfworld:hittest(x,y)
    if (self.bindata[y*self.size[1]+x]) then
      return true
    end
    
    return false
  end
  
  -- set binaray data at pos
  function dfworld:setbindata(x,y,data)
    self.bindata[y*self.size[1]+x] = data
  end
  
  -- xx --> err?
  -- xx
  
  -- xo --> recheck 1
  -- oo
  
  -- xx --> |
  -- oo
  
  -- xo --> -
  -- xo
  
  -- xo --> /
  -- ox
  
  -- xo --> /
  -- xx
  
  -- ox --> recheck 2
  -- oo
  
  ------
  -- ox --> \
  -- xo
  
  -- ox --> \
  -- xx
  
  -- ox --> -
  -- ox
  ------
  
  -- oo --> recheck 3
  -- xo
  
  -- oo --> |
  -- xx
  
  ------
  
  -- oo --> recheck 4
  -- ox
  
  function dfworld:get_response_vec(x,y)
    local p1,p2,p3,p4
    p1 = self:hittest(x,y)
    p2 = self:hittest(x+1,y)
    p3 = self:hittest(x,y+1)
    p4 = self:hittest(x+1,y+1)
    
    --local res = vec(0,0)
    if (p1) then
      if (p2 and (not p3) and (not p4)) then
        --res.y = 1
        return "|"--res
      elseif ((not p2) and p3 and (not p4)) then
        --res.x = 1
        return "-"--res
      elseif ((not p2) and p3 or p4) then
        --res.x = 0.5
        --res.y = 0.5
        return "/"--res
      end
      
      return self:get_response_vec(x-1,y-1)
      
    elseif (p2) then
      if (p3) then
        --res.x = -0.5
        --res.y = 0.5
        return "\\"--res
      elseif (p4) then
        --res.x = 1
        return "-"--res
      end
      
      return self:get_response_vec(x+2,y-1)
    
    elseif (p3) then
      if (p4) then
        --res.y = 1
        return "-"--res
      end
      
      return self:get_response_vec(x-1,y+2)
    
    else
      return self:get_response_vec(x+2,y+2)
    end
  end
  
  -- df circle shader
  dfworld.df_circle_shader = gfx.createshader("df_circle", [[
  varying vec4 pos;
  void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    pos = gl_Vertex;
  	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  }


  ]], [[
  uniform sampler2D tex;
  uniform float masksize;
  uniform float feathersize;
  uniform float rel_x;
  uniform float rel_y;
  varying vec4 pos;
  void main()
  {
    float quadsize = masksize + feathersize / 2.0;
    float realsize = masksize - feathersize / 2.0;
    float dx = pos.x - rel_x;
    float dy = pos.y - rel_y;
    
    float distance_to_center = sqrt(dx*dx + dy*dy);
    float distance_to_realsize = distance_to_center - realsize / 2.0;
    float weighted_distance = abs(distance_to_realsize) / (feathersize / 2.0);
    float final_dist = clamp(weighted_distance, 0.0, 1.0);
    
    vec4 old_tex = texture2D(tex, pos.st / 512.0);
    
    if (final_dist < old_tex.a)
    {
      gl_FragColor = vec4(0.0, 0.0, 0.0, final_dist);
    } else {
      gl_FragColor = vec4(0.0, 0.0, 0.0, old_tex.a);
    }
  }

  ]])
  
  -- modification functions
  function dfworld:cutcircle(x,y,size)
    print("cut at " .. x .. ", " .. y .. ", size: " .. size)
    
    
    -- draw to fbo
    self.fbo:attach(self.tex[self.sec_tex], 1)
    gfx.bindframebuffer(self.fbo)
    gfx.clear()
    gfx.setview(0,0,512,512)
    gfx.setortho(0,512,0,512)
    
    -- copy from active to sec
    local oldtex = gfx.bindtexture(self.tex[self.active_tex])
    gfx.blending()
    gfx.alphatest()
    gfx.drawtopleft(0,0,512,512)
    
    -- TODO: render new hole
    --gfx.bindtexture(0)
    gfx.bindshader(self.df_circle_shader)
    local newquadsize = size + self.feathersize
    self.df_circle_shader:setuniformf("masksize", size)
    self.df_circle_shader:setuniformf("feathersize", self.feathersize)
    self.df_circle_shader:setuniformf("rel_x", x)
    self.df_circle_shader:setuniformf("rel_y", y)
    gfx.drawcentered(x,y,newquadsize,newquadsize)
    gfx.bindshader()
    
    gfx.bindtexture(oldtex)
    
    -- detach fbo and tex
    gfx.bindframebuffer()
    self.fbo:detach(1)
    
    -- switch ping-pong-fbo/tex
    if (self.active_tex == 1) then
      self.active_tex = 2
      self.sec_tex = 1
    else
      self.active_tex = 1
      self.sec_tex = 2
    end
    
    -- mask out from bindata
    for ty=-size/2,0 do
      for tx=-size/2,0 do
        -- find value at delta pos
        local dx,dy
        dx = x + tx
        dy = y + ty
        if (self:hittest(dx,dy)) then
          local d = math.abs(math.sqrt(tx*tx + ty*ty))
          if (d <= size/2) then
            
            for i=1,2 do
              for ofx=dx,dx+(size-tx) do
                if (i == 1) then
                  self:setbindata(ofx,dy,false)
                else
                  self:setbindata(ofx,y-ty,false)
                end
              end
            end
            
            break
          end
        end
      end
    end
  end
  
  -- df render shader
  dfworld.df_render_shader = gfx.createshader("df_render", [[

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
  	if (color.a < 0.7)
	  {
	    gl_FragColor = vec4(0.6, 0.0, 0.0, color.a);
	  } else {
	    gl_FragColor = vec4(1.0, 0.0, 0.0, color.a);
	  }
  }

  ]])
  
  function dfworld:draw()
    local oldtex = gfx.bindtexture(self.tex[self.active_tex])
    gfx.bindshader(self.df_render_shader)
    gfx.alphatest(gfx.GEQUAL, 0.5)
    gfx.drawtopleft(0,0,512,512)
    
    gfx.alphatest()
    gfx.blending()
    gfx.bindshader()
    gfx.bindtexture(oldtex)
  end
  
  return dfworld
end




--local aliq = create_new_liq(40,20,16, 20)
--local bliq = create_new_liq(20,20,16, 20)
local liq_world = create_liq_world()
--liq_world:add_liq(aliq)
--liq_world:add_liq(bliq)
for i=1,60 do
  liq_world:add_liq(create_new_liq(40+math.random(0,30),20+math.random(0,20),8, 20))
end

-- local ett_nytt_spel = create_new_game()
-- ett_nytt_spel:update()

function create_new_game()
local newgame = {}


local world = create_dfworld("jam/leveltest.png")--leveltest.png"))
local zoom = 1
local scroll_x = 0
local scroll_y = 0
local scrolling = false
local zooming = false
local mouse_x = 0
local mouse_y = 0

function newgame:update()
  if inp.iskeydown(inp.ESC) then
		app.quit()
	end
	if inp.iskeydown(inp.TAB) then
		app.reboot()
	end
	
	liq_world:step(0.5, world)
	
	if (inp.isbuttondown(inp.MOUSE_LEFT)) then
	  new_mouse_x, new_mouse_y = inp.getmousepos()
	  if (world:hittest(new_mouse_x, new_mouse_y)) then
	    print("hit")
    else
      print("miss")
    end
	  
	  if (scrolling and inp.iskeydown(inp.SPACE)) then
	    --new_mouse_x, new_mouse_y = inp.getmousepos()
	    scroll_x = scroll_x + (new_mouse_x - mouse_x)
	    scroll_y = scroll_y + (new_mouse_y - mouse_y)
	    mouse_x = new_mouse_x
	    mouse_y = new_mouse_y
	    --print(scroll_x, scroll_y)
    else
      scrolling = true
      mouse_x, mouse_y = inp.getmousepos()
    end
  else
    scrolling = false
  end
  
  if (inp.isbuttondown(inp.MOUSE_MIDDLE)) then
    local mx,my = inp.getmousepos()
    world:cutcircle(mx,my,32)
  end
	
	if (inp.isbuttondown(inp.MOUSE_RIGHT)) then
	  
	  if (zooming) then
	    new_mouse_x, new_mouse_y = inp.getmousepos()
	    zoom = zoom + (new_mouse_y - mouse_y) * 0.01
	    mouse_y = new_mouse_y
    else
      zooming = true
      mouse_x, mouse_y = inp.getmousepos()
    end
  else
    zooming = false
  end
end

function newgame:draw()
  gfx.clear()
  gfx.loadidentity()
  local w,h = app.getwindimensions()
  --gfx.translate(w / 2, h / 2)
  gfx.scale(zoom)
  gfx.translate(scroll_x, scroll_y)
  
  liq_world:draw()
  
  world:draw()
  
  gfx.loadidentity()
  
  gfx.alphatest(gfx.GEQUAL, 0.5)
  panic.text("esc = quit", 12, 12)
  panic.text("tab = reboot", 12, 12+16)
end


return newgame
end