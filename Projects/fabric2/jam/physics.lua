require("jam/vecmath")

function sphere(x,y,r)
	local s = {}
	s.c = vec(x,y)
	s.r = r

	return s
end

function physics_body(x,y,r,m)
	local b = {}
	b.body = sphere(x,y,r)
	b.velocity = vec(0,0)
	b.force = vec(0,0)
	b.mass = m
	b.rest = false
	
	function b:clear()
		self.force = vec(0,0)
	end
	
	function b:position()
		return self.body.c
	end
	
	return b
end


function sphere_sphere_collision(a,b)
	local p0 = a.body.body
	local p1 = b.body.body
	
	local v = p1.c - p0.c
	local d = math.sqrt(v.x^2 + v.y^2)
	local sum_r = p0.r + p1.r

	if d < sum_r then
		return v
	else 
		return nil
	end
end

local max_impulse = 1

function clamp(i)
	local v = i

	if v.x > max_impulse then
		v.x = max_impulse
	end
	
	if v.x < -max_impulse then
		v.x = -max_impulse
	end
	
	if v.y > max_impulse then
		v.y = max_impulse
	end
	
	if v.y < -max_impulse then
		v.y = -max_impulse
	end
	
	return v
end

function sphere_sphere_response(a,b)
	local b0 = a.body
	local b1 = b.body
	
	local m0 = b0.mass
	local m1 = b1.mass
	
	local impact = b1.body.c - b0.body.c
	local impulse = (b1.velocity - b0.velocity) * 0.1
	
	local impact_speed = vec_dot(impact,impulse)
	impulse = impulse * math.sqrt(math.abs(impact_speed * m0 * m1))
	
	--print("IMPULSE " .. impulse.x .. "," .. impulse.y)
	local imp_len = math.abs(vec_len(impulse))
	
	--print("KUUUK", imp_len)
	local val = 0.05
	
	--print(math.abs(impulse.y))
	
	if imp_len < 2.0 then
		--impulse = vec(0.0,0.0)
		--a.rest = true
		--b.rest = true
		
		b0.velocity = vec(0.0,0.0)
		b1.velocity = vec(0.0,0.0)
		
		--print("REST")
	else	
		local inv_m0 = 1.0 / m0
		local inv_m1 = 1.0 / m1
		
		a.rest = false
		b.rest = false
		
		b0.velocity = b0.velocity + impulse * inv_m0
		b1.velocity = b1.velocity - impulse * inv_m1
	end
end

function particle(x,y)
	local p = {}
	p.body = physics_body(x,r,2.5,1.0)
	p.type = "Some type"

	return p
end