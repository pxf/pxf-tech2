function vec_mul(v1,v2)
  local n1,n2 = tonumber(v1),tonumber(v2)
  if n1 then
    return vec(n1 * v2[1], n1 * v2[2])
  elseif n2 then
    return vec(v1[1] * n2, v1[2] * n2)
  end
  
  -- both are vectors
  return vec(v1[1] * v2[1], v1[2] * v2[2])
end

function vec_add(v1,v2)
  local n1,n2 = tonumber(v1),tonumber(v2)
  if n1 then
    return vec(n1 + v2[1], n1 + v2[2])
  elseif n2 then
    return vec(v1[1] + n2, v1[2] + n2)
  end
  
  -- both are vectors
  return vec(v1[1] + v2[1], v1[2] + v2[2])
end

function vec_sub(v1,v2)
  local n1,n2 = tonumber(v1),tonumber(v2)
  if n1 then
    return vec(n1 - v2[1], n1 - v2[2])
  elseif n2 then
    return vec(v1[1] - n2, v1[2] - n2)
  end
  
  -- both are vectors
  return vec(v1[1] - v2[1], v1[2] - v2[2])
end

function vec_norm(v)
	local d = vec_len(v)
	
	if d == 0 then
		return vec(0,0)
	end
	
	return vec(v.x / d,v.y / d)
end

function vec_len(v)
  return math.sqrt(v[1]*v[1] + v[2]*v[2])
end

function vec_cross(v)
  return vec(v[2], -v[1])
end

function vec_dot(v1,v2)
	return v1.x*v2.x + v1.y*v2.y
end

function vec_print(a,b)
  if (type(a) == "string") then
    return (a .. "{x = " .. tostring(b[1]) .. ", y = " .. tostring(b[2]) .. "}")
  end
  
  return ("{x = " .. tostring(a[1]) .. ", y = " .. tostring(a[2]) .. "}" .. b)
end

-- create vector
function vec(x,y)
  local v = {x,y}
  v.x = v[1]
  v.y = v[2]
  
  if (y == nil) then
    v[2] = v[1]
  end
  
  local mt = {__add = vec_add,
              __sub = vec_sub,
              __mul = vec_mul,
              __concat = vec_print}
  v.len = vec_len
  v.cross = vec_cross
  v.norm = vec_norm
  v.dot = vec_dot
  
  
  setmetatable(v, mt)
  return v
end