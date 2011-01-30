require("jam/vecmath")

local liq_render_shader = gfx.createshader("df_render", [[

void main() {
  gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}


]], [[
uniform sampler2D tex;
uniform int liqtype;
void main()
{
	vec4 color = texture2D(tex, gl_TexCoord[0].st);
  gl_FragColor = vec4(1.0,1.0,1.0,color.a);//vec4(water_tex.rgb * vec3(0.5, 0.5, 1.0), color.r);
}

]])

function create_new_liq(x,y,size, mass, liqtype)
  local liq = {x = x, y = y, size = size, forces = {0,0}, mass = mass,
               vel = {0,0}, liqtype = liqtype}
  
  function liq:intersect(b)
    local tx = b.x - self.x
    local ty = b.y - self.y
    local dist = math.sqrt(tx*tx + ty*ty)
    
    if (dist - self.size - b.size <= 0) then
      local r = vec(tx,ty)
      r = r:norm()
      r = r*math.abs(dist - self.size - b.size)
      return r
    else
      return nil
    end
  end
  
  function liq:draw()
    --gfx.bindtexture(0)
    if (math.abs(self.vel[2]) > 1) then
      gfx.drawcentered(self.x-self.vel[1]*1.5, self.y-self.vel[2]*1.5, 12, 12)
      gfx.drawcentered(self.x-self.vel[1], self.y-self.vel[2], 24, 24)
    end
    gfx.drawcentered(self.x, self.y, 32, 32)--self.size * 2, self.size * 2)
  end
  
  function liq:apply_force( x,y )
    self.forces[1] = self.forces[1] + x
    self.forces[2] = self.forces[2] + y
  end
  
  function liq:undo_step()
    self.x = self.old_x
    self.y = self.old_y
  end
  
  function liq:inverse_step()
    self.x = self.old_x
    self.y = self.old_y
    
    local inv = (vec(0,0) - vec(self.vel[1], self.vel[2])) * vec(0.999, 0.4)
    self.vel[1] = inv.x
    self.vel[2] = inv.y
  end
  
  function liq:step(time)
    self.old_x = self.x
    self.old_y = self.y
    
    -- f = ma, a = f / m, v = s/t, s = v * t, v = a * t = (f / m) * t
    self.vel[1] = self.vel[1] + (self.forces[1] / self.mass) * time
    self.vel[2] = self.vel[2] + (self.forces[2] / self.mass) * time
    
    -- step pos
    self.x = self.x + self.vel[1] * time
    self.y = self.y + self.vel[2] * time
    
    -- reset forces
    self.forces = {0,0}
  end
  
  return liq
end


function create_liq_world()
  local liqworld = {liqs = {}, liq_tex = gfx.loadtexture(1024, "jam/liq.png", true),
                    water_tex = gfx.loadtexture(1, "jam/waterrepeat.png", false),
                    fbo = gfx.newframebuffer(),
                    bg_tex = gfx.newtexture(1, 512, 512, true)}
  
  function liqworld:add_liq(a)
    table.insert(self.liqs, a)
  end
  
  liqworld.df_render_shader = gfx.createshader("df_render", [[

  void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
  	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  }


  ]], [[
  uniform sampler2D tex;
  uniform sampler2D tex2;
  uniform float offset_x;
  uniform float offset_y;
  void main()
  {
  	vec4 color = texture2D(tex, gl_TexCoord[0].st);
  	vec2 water_coords = gl_TexCoord[0].st*32.0;
  	
  	water_coords.x += cos(water_coords.y*0.5+offset_x);
  	water_coords.y += offset_x*0.5;
  	
  	vec4 water_tex = texture2D(tex2, water_coords);
  	if (color.r < 0.8)
	  {
	    gl_FragColor = vec4(water_tex.rgb * vec3(0.4, 0.4, 1.0), color.r);
	  } else {
	    gl_FragColor = vec4(water_tex.rgb * vec3(0.3, 0.3, 1.0), color.r);
	  }
  }

  ]])
  
  liqworld.wateranim = 0
  function liqworld:draw()
    
    -- bind fbo
    self.fbo:attach(self.bg_tex, 1)
    gfx.bindframebuffer(self.fbo)
    gfx.clear()
    gfx.setview(0,0,512,512)
    gfx.setortho(0,512,0,512)
    
    local oldtex = gfx.bindtexture(self.liq_tex)
    gfx.blending(gfx.SRC_ALPHA, gfx.ONE_MINUS_SRC_ALPHA)
    gfx.alphatest()
    --gfx.bindshader(liq_render_shader)
    for i=1,#self.liqs do
      self.liqs[i]:draw()
    end
    gfx.blending()
    
    gfx.bindframebuffer()
    self.fbo:detach(1)
    
    gfx.bindtexture(self.bg_tex)
    gfx.bindshader(self.df_render_shader)
    
    self.df_render_shader:bindtexunit(self.water_tex, 1)
    self.df_render_shader:setuniformi("tex2", 1)
    self.df_render_shader:setuniformf("offset_x", self.wateranim)
    
    gfx.alphatest(gfx.GEQUAL, 0.5)
    gfx.drawtopleft(0,0,512,512)
    gfx.bindshader()
    gfx.bindtexture(oldtex)
  end
  
  function liqworld:step(time, df_world)
    liqworld.wateranim = liqworld.wateranim + 0.05
    
    -- apply gravity
    for i=1,#self.liqs do
      self.liqs[i]:apply_force(0, 9.82)
    end
    
    -- step each liq
    for i=1,#self.liqs do
      self.liqs[i]:step(time)
    end
    
    for i=1,#self.liqs do
      -- check if we collide with world
      if (df_world:hittest(math.floor(self.liqs[i].x), math.floor(self.liqs[i].y))) then
        --self.liqs[i]:undo_step()
        -- get response direction
        local old_x = math.floor(self.liqs[i].x)
        local old_y = math.floor(self.liqs[i].y)
        local res = df_world:get_response_vec(math.floor(self.liqs[i].x), math.floor(self.liqs[i].y))
        self.liqs[i]:inverse_step()
        
        local rel = vec(0,0)
        local new_x = math.floor(self.liqs[i].x)
        local new_y = math.floor(self.liqs[i].y)
        if (new_x < old_x) then
          rel.x = -1
        elseif (new_x > old_x) then
          rel.x = 1
        end
        if (new_y < old_y) then
          rel.y = -1
        elseif (new_y > old_y) then
          rel.y = 1
        end
        rel = rel:norm()
        
        -- create reflection force
        local refl_force = vec(0,0)
        if (res == "-") then
          if (rel.y < 0) then
            refl_force[2] = -1
          else
            refl_force[2] = 1
          end
          
        elseif (res == "|") then
          if (rel.x < 0) then
            refl_force[1] = -1
          else
            refl_force[1] = 1
          end
          
        elseif (res == "/") then
          if (rel.x < 0 and rel.y < 0) then
            refl_force[1] = -1
            refl_force[2] = -1
          else
            refl_force[1] = 1
            refl_force[2] = 1
          end
        elseif (res == "\\") then
          if (rel.x > 0 and rel.y > 0) then
            refl_force[1] = 1
            refl_force[2] = -1
          else
            refl_force[1] = -1
            refl_force[2] = 1
          end
        end
        
        -- normalize refl vect and multiply by amount intersection itwas
        --refl_force = vec(0,0) - refl_force
        refl_force = refl_force:norm() * vec(60, 60)
        self.liqs[i]:apply_force(refl_force[1], refl_force[2])
        
      end
    end
    
    -- perform intersection on each liq
    local amp = 0.5
    for i=1,#self.liqs do
      for j=i,#self.liqs do
        
      	local res = self.liqs[i]:intersect(self.liqs[j])
      	if (res ~= nil) then
      	  local nx = res.x
      	  local ny = res.y
      	  self.liqs[j]:apply_force(nx*amp,ny*amp)
      	  local res2 = vec(0,0)-res
      	  local nx = res2.x
      	  local ny = res2.y
      	  self.liqs[i]:apply_force(nx*amp,ny*amp)
        end
        
      end
    end
  end
  
  return liqworld
end

