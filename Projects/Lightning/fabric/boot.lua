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
__guihit = false

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

function escape_backslashes(str)
  local res = string.gsub(str, "\\", "\\\\")
  return res
end

----------------------------
-- window resizing
function app.setwindimensions(w,h)
  app.width = w
  app.height = h
  app._setwindimensions(w,h)
end

----------------------------
-- logging functionality
backlog = {data = {}, max = 1024}
function backlog:print(...)
  table.insert(self.data, (...))
  if (#self.data > self.max) then
    table.remove(self.data, 1)
  end
end

_print = print
function print(...)
  _print(...)
  backlog:print(...)
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
font = gfx.loadtexture(4096, "fabric/consolefont.png")
runtimeerror_tex = gfx.loadtexture(1,"fabric/runtimeerror.png")

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
  panicbox_scroll_x = 1
  panicbox_scroll_y = 1
  mx_old = nil
  inp.showmouse(true)
end

function draw_runtimeerror()
  gfx.loadidentity()
  gfx.bindtexture(runtimeerror_tex)
  --gfx.setclearcolor(46.0/255.0,46.0/255.0,46.0/255.0)
  gfx.setcolor(1,1,1)
  gfx.drawcentered(app.width / 2, app.height / 2,512,256)
  panic.text_box(error_lines, app.width / 2 - 230, app.height / 2 - 60, 475, 140)
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

function panic.text_box(strs, x, y, w, h) -- sx, sy = scrollx, scrolly
  local ylines = 0
  local max_len = math.floor(w / 8)
  sx = panicbox_scroll_x
  sy = panicbox_scroll_y
  for k,v in pairs(strs) do
    local l = v:sub(sx)
    l = l:sub(1, max_len)
    panic.text(l, x, y+ylines)
    ylines = ylines + 12
  end
end

-- basic callfunctions
function _update()
  __guihit = false
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
      local runtimebuttons = {cont = {hb = {app.width/2-225, app.height/2+89, 72, 26},
                                      action = function ()
                                                 error_stop = false
                                               end
                                     },
                              reboot = {hb = {app.width/2-144, app.height/2+89, 128, 26},
                                      action = function ()
                                                  app.reboot()
                                               end
                                     },
                              save = {hb = {app.width/2-6, app.height/2+89, 92, 26},
                                      action = function ()
                                                  local errlog = io.open("error.log", "w+")
                                                  errlog:write(error_text)
                                                  errlog:close()
                                                  print("Saved error log to: error.log.")
                                               end
                                     },
                              quit = {hb = {app.width/2+171, app.height/2+89, 56, 26},
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
      
      -- hit test panic box
      if (point_hittest(mx,my, app.width / 2 - 230, app.height / 2 - 60, app.width / 2 - 230 + 475, app.height / 2 - 60 + 140)) then
        --print("LOL")
        if (mx_old == nil) then
          mx_old = mx
          my_old = my
        end
        panicbox_scroll_x = panicbox_scroll_x + (mx_old - mx) / 8.0
        panicbox_scroll_y = panicbox_scroll_y + (my_old - my) / 8.0
        if (panicbox_scroll_x < 0) then
          panicbox_scroll_x = 1
        end
        mx_old = mx
        my_old = my
      end
    else
      mx_old = nil
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

--print("gfx.test() -> " .. tostring(gfx.test()))
