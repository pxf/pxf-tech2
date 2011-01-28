require("jam/vecmath")

function sphere(x,y,r)
	local s = {}
	s.c = vec(x,y)
	s.r = r

	return s
end

function particle(x,y)
	local p = {}
	p.body = sphere(x,y,5.0)
	p.velocity = vec(0,0)
	p.direction = vec(0,0)
	p.force = vec(0,0)
	p.type = "Some type"
	
	function p:clear()
		self.force = vec(0,0)
	end
	
	return p
end