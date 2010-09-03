--print("bootstrap says hi! " .. type(app))
print("app has:")
for k,v in pairs(app) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

print("gfx has:")
for k,v in pairs(gfx) do
  print("        " .. tostring(k) .. " : " .. tostring(v))
end

--print("gfx.test() -> " .. tostring(gfx.test()))
