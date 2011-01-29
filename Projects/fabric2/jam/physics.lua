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
	
	function b:clear()
		self.force = vec(0,0)
	end
	
	function b:position()
		return self.body.c
	end
	
	return b
end

function particle(x,y)
	local p = {}
	p.body = physics_body(x,r,2.5,1.0)
	p.type = "Some type"

	return p
end