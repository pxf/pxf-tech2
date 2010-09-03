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

-- error handling
error_stop = false
function _runtimeerror(str)
  print(" -- Runtime Error -- \n" .. str)
  error_stop = true
end

-- font system
-- Font/text (using charmap) rendering
function draw_text(str, x, y)
  
  gfx.bindtexture(font)
  --pxf.graphics.setcolor(1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
  gfx.translate(x, y)
	local strlen = #str
	local char_w = 8
	
	local color_indicator = "^"
	local change_color = false
	local char_counter = 0
	
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
        
        --[[ Color indexes
        if string.char(tostring(string.byte(str, i))) == "0" then
          pxf.graphics.setcolor(0, 0, 0)
        elseif string.char(tostring(string.byte(str, i))) == "1" then
          pxf.graphics.setcolor(1, 0, 0)
        elseif string.char(tostring(string.byte(str, i))) == "2" then
          pxf.graphics.setcolor(0, 1, 0)
        elseif string.char(tostring(string.byte(str, i))) == "3" then
          pxf.graphics.setcolor(0, 0, 1)
        elseif string.char(tostring(string.byte(str, i))) == "4" then
          pxf.graphics.setcolor(1.0, 0.3, 0.3)
        else
          pxf.graphics.setcolor(1, 1, 1)
        end]]
        
        change_color = false
      else
  	    -- draw quad
  	    gfx.drawcentered((char_counter)*char_w, 0, 16, 16, s, t, 16, 16)
  	    char_counter = char_counter + 1
	    end
    end
	end
	
	gfx.translate(-x, -y)
	--pxf.graphics.setcolor(1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
end

-- basic callfunctions
function _update()
  update()
end

function _draw()
  if (not error_stop) then
    draw()
  end
end

--print("gfx.test() -> " .. tostring(gfx.test()))
