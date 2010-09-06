print("app has:")
for k,v in pairs(app) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("gfx has:")
for k,v in pairs(gfx) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("inp has:")
for k,v in pairs(inp) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

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


-- load standard textures
font = gfx.loadtexture("data/consolefont2.png")
runtimeerror_tex = gfx.loadtexture("data/runtimeerror.png")

-- error handling
error_stop = false
error_text = ""
error_lines = {}
function _runtimeerror(str)
  error_text = str
  str = "aoeae åäö ÅÄÖ ".. str
  local fancy = str--string.gsub(str, "(%d+)", "^4%1^r")
  fancy = string.gsub(fancy, "(\t)", "    ")
  error_lines = split(fancy, '\n+')
  print(" -- Runtime Error-- \n" .. str)
  error_stop = true
end

function draw_runtimeerror()
  gfx.bindtexture(runtimeerror_tex)
  gfx.setclearcolor(46.0/255.0,46.0/255.0,46.0/255.0)
  gfx.setcolor(1,1,1)
  gfx.drawcentered(400,300,512,256)
  panic.text_box(error_lines, 400 - 230, 300 - 42, 500, 100, 4, 1)
end

-- debug text drawing system
panic = {}
function panic.text(str, x, y)
  
  gfx.bindtexture(font)
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
    draw()
  else
    draw_runtimeerror()
  end
end

--print("gfx.test() -> " .. tostring(gfx.test()))
