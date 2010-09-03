--print("bootstrap says hi! " .. type(app))
print("app has:")
for k,v in pairs(app) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("gfx has:")
for k,v in pairs(gfx) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

error_stop = false

function _runtimeerror(str)
  print(" -- Runtime Error -- \n" .. str)
  error_stop = true
end

function update()
  _update()
end

function draw()
  if (not error_stop) then
    _draw()
  end
end

--print("gfx.test() -> " .. tostring(gfx.test()))
