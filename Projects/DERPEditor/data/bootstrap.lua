-- some debug output before we start being awesome
print("app has:")
for k,v in pairs(app) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("gfx has:")
for k,v in pairs(gfx) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("snd has:")
for k,v in pairs(snd) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("inp has:")
for k,v in pairs(inp) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("Render mode: " .. tostring(app._getrenderoption()))

-- store window dimensions for later use
app.width, app.height = app.getwindimensions()

function split(str, pat)
   local t = {}  -- NOTE: use {n = 0} in Lua-5.0
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
	 table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

function point_hittest(px,py, x0,y0, x1,y1 )
  if px < x0 then
    return false
  elseif px > x1 then
    return false
  elseif py < y0 then
    return false
  elseif py > y1 then
    return false
  end
  
  return true
end

app.rendermode = app._getrenderoption()
function app.setrenderoption(render_mode)
  app.rendermode = render_mode
  app._setrenderoption(render_mode)
end
function app.getrenderoption()
  return app.rendermode
end

gfx.forceredraw = true
function gfx.redrawneeded(x,y,w,h)
  if (x == nil) then
    gfx._redrawneeded()
    gfx.forceredraw = true
  else
    gfx._redrawneeded(x,y,w,h)
  end
end

-- load standard textures
font = gfx.loadtexture(1024, "data/consolefont2.png")
runtimeerror_tex = gfx.loadtexture(1,"data/runtimeerror.png")

-- error handling
error_stop = false
error_text = ""
error_lines = {}
function _runtimeerror(str)
  error_text = str
  local fancy = str--string.gsub(str, "(%d+)", "^4%1^r")
  fancy = string.gsub(fancy, "(\t)", "    ")
  error_lines = split(fancy, '\n+')
  print(" -- Runtime Error-- \n" .. str)
  error_stop = true
end

function draw_runtimeerror()
  gfx.bindtexture(runtimeerror_tex)
  --gfx.setclearcolor(46.0/255.0,46.0/255.0,46.0/255.0)
  gfx.setcolor(1,1,1)
  gfx.drawcentered(400,300,512,256)
  panic.text_box(error_lines, 400 - 230, 300 - 42, 500, 100, 4, 1)
end

-- debug text drawing system
panic = {}
function panic.text(str, x, y)
  
  local oldtex = gfx.bindtexture(font)
  local r,g,b = gfx.getcolor()
  gfx.setcolor(1, 1, 1)
  gfx.translate(x, y)
	local strlen = #str
	local char_w = 8
	
	local color_indicator = "^"
	local change_color = false
	local char_counter = 0
	local rr,rg,rb
	local euro_next = false
	
	for i=1,strlen do
	  -- calculate tex coords
	  local index = string.byte(str, i)
	  if (index == 195) then
	    -- found special char
	    euro_next = true
    else	  
  	  -- change color?
  	  if index == string.byte(color_indicator, 1) then
  	    change_color = true
      else
        if change_color then
        
          -- Color indexes
          if string.char(tostring(string.byte(str, i))) == "r" then
            gfx.setcolor(rr,rg,rb)
          else
            rr,rg,rb = gfx.getcolor()
            if string.char(tostring(string.byte(str, i))) == "0" then
              gfx.setcolor(0, 0, 0)
            elseif string.char(tostring(string.byte(str, i))) == "1" then
              gfx.setcolor(1, 0, 0)
            elseif string.char(tostring(string.byte(str, i))) == "2" then
              gfx.setcolor(0, 1, 0)
            elseif string.char(tostring(string.byte(str, i))) == "3" then
              gfx.setcolor(0, 0, 1)
            elseif string.char(tostring(string.byte(str, i))) == "4" then
              gfx.setcolor(1.0, 0.3, 0.3)
            else
              gfx.setcolor(1, 1, 1)
            end
          end
        
          change_color = false
        else
    	    -- draw quad
    	    if (euro_next) then
    	      euro_next = false
    	      index = index + 32
  	      else
  	        index = index - 32
  	      end
      	  local s = math.fmod(index, 16) * 16
      	  local t = math.floor(index / 16) * 16
    	    gfx.drawcentered((char_counter)*char_w, 0, 16, 16, s, t, 16, 16)
    	    char_counter = char_counter + 1
  	    end
      end
    
    end
	end
	
	gfx.translate(-x, -y)
	gfx.setcolor(r,g,b)
	gfx.bindtexture(oldtex)
end

function panic.text_box(strs, x, y, w, h, sx, sy) -- sx, sy = scrollx, scrolly
  local ylines = 0
  local max_len = math.floor(w / 8)
  for k,v in pairs(strs) do
    local l = v:sub(sx)
    l = l:sub(1, max_len)
    panic.text(l, x, y+ylines)
    ylines = ylines + 12
  end
end

-- basic callfunctions
function _update()
  if (not error_stop) then
    if (app.rendermode == app.REDRAWMODE_FULL) then
      gfx.redrawneeded()
    end
    update()
  else
    gfx.redrawneeded()
    
    -- check hit test on buttons
    if (inp.isbuttondown(inp.MOUSE_LEFT)) then
      -- button table
      local runtimebuttons = {cont = {hb = {app.width/2-225, app.height/2+80, 72, 26},
                                      action = function ()
                                                 error_stop = false
                                               end
                                     },
                              reboot = {hb = {app.width/2-144, app.height/2+80, 128, 26},
                                      action = function ()
                                                  app.reboot()
                                               end
                                     },
                              quit = {hb = {app.width/2+171, app.height/2+80, 56, 26},
                                      action = function ()
                                                  app.quit()
                                               end
                                  }
                             }
    
      -- loop buttons and perform hittest
      local mx,my = inp.getmousepos()
      for k,v in pairs(runtimebuttons) do
        if (point_hittest(mx,my, v.hb[1], v.hb[2], v.hb[1] + v.hb[3], v.hb[2] + v.hb[4])) then
          v.action()
        end
      end
    end
  end
end

function _draw()
  if (not error_stop) then
    draw(gfx.forceredraw)
  else
    draw_runtimeerror()
  end
  gfx.forceredraw = false
end

function draw_line_segment(x1,y1,x2,y2,w)
  local oldtex = gfx.bindtexture(0)
  local cx,cy = 0,0
  local len = 0
  local a = 0
  
  -- calc line center
  cx = ((x2 - x1) / 2)
  cy = ((y2 - y1) / 2)
  
  -- calc line length
  len = math.sqrt(cx*4*cx + cy*4*cy) + w/2
  
  -- calc angle
  a = math.atan(cy/cx)
  
  gfx.translate(x1 + cx, y1 + cy)
  gfx.rotate(a)

  gfx.drawquad(-len/2, -w/2, len/2, -w/2, len/2, w/2, -len/2, w/2)
  
  gfx.rotate(-a)
  gfx.translate(-(x1 + cx),-(y1 + cy))
  gfx.bindtexture(oldtex)
end

function draw_line(points,w)
  local oldx,oldy = nil,nil
  for k,v in pairs(points) do
    if not (oldx == nil) then
      draw_line_segment(oldx,oldy,v[1],v[2],w)
    end
    
    oldx = v[1]
    oldy = v[2]
  end
end

function draw_spline(points,segments,w)
  local new_points = {}
  local point_count = #points
  local step = 1.0 / segments
  local t = 0.0
  
  for i=1,segments do
    local p0, p1, p2, p3 = {0,0},{0,0},{0,0},{0,0}
    local i0, i1, i2, i3 = 0,0,0,0
    local new_p = {0,0}
    local deltat = 0.0
    
    deltat = ((point_count) * t) + 1
    i1 = math.floor(deltat)
    i2 = i1 + 1
    i3 = i2 + 1
    i0 = i1 - 1
    
    if (i1 <= 1) then
      i1 = 1
    end
    
    if (i0 <= 1) then
      i0 = 1
    end
    
    if (i2 >= point_count) then
      i2 = point_count
    end
    
    if (i3 >= point_count) then
      i3 = point_count
    end
    
    deltat = deltat - i1
    
    p0 = points[i0]
    p1 = points[i1]
    p2 = points[i2]
    p3 = points[i3]
    
    new_p[1] = 0.5 * ( (2 * p1[1]) +
                       (-p0[1] + p2[1]) * deltat +
                       (2*p0[1] - 5*p1[1] + 4*p2[1] - p3[1]) * (deltat*deltat) +
                       (-p0[1] + 3*p1[1] - 3*p2[1] + p3[1]) * (deltat*deltat*deltat)
                     )
    
    new_p[2] = 0.5 * ( (2 * p1[2]) +
                       (-p0[2] + p2[2]) * deltat +
                       (2*p0[2] - 5*p1[2] + 4*p2[2] - p3[2]) * (deltat*deltat) +
                       (-p0[2] + 3*p1[2] - 3*p2[2] + p3[2]) * (deltat*deltat*deltat)
                     )
    
    table.insert(new_points, new_p)
    t = t + step
  end
  
  draw_line(new_points, w)
end

--print("gfx.test() -> " .. tostring(gfx.test()))
