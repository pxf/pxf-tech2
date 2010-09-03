print("app has:")
for k,v in pairs(app) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("gfx has:")
for k,v in pairs(gfx) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

-- load standard textures
font = gfx.loadtexture("data/consolefont.png")
runtimeerror_tex = gfx.loadtexture("data/runtimeerror.png")

-- error handling
error_stop = false
function _runtimeerror(str)
  print(" -- Runtime Error -- \n" .. str)
  error_stop = true
end

function draw_runtimeerror()
  gfx.bindtexture(runtimeerror_tex)
  --gfx.setclearcolor(46.0/255.0,46.0/255.0,46.0/255.0)
  gfx.setcolor(1,1,1)
  gfx.drawcentered(400,300,512,256)
end

-- font system
-- Font/text (using charmap) rendering
function draw_text(str, x, y)
  
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
	
	for i=1,strlen do
	  -- calculate tex coords
	  local index = string.byte(str, i)
	  
	  local s = math.fmod(index, 16) * 16
	  local t = math.floor(index / 16) * 16
	  
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
  	    gfx.drawcentered((char_counter)*char_w, 0, 16, 16, s, t, 16, 16)
  	    char_counter = char_counter + 1
	    end
    end
	end
	
	gfx.translate(-x, -y)
	gfx.setcolor(r,g,b)
end

-- basic callfunctions
function _update()
  update()
end

function _draw()
  if (not error_stop) then
    draw()
  else
    draw_runtimeerror()
  end
end

--print("gfx.test() -> " .. tostring(gfx.test()))
