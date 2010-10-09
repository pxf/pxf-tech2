function clamp(x, min, max)
  if (x < min) then
    return min
  elseif (x > max) then
    return max
  else
    return x
  end
end

function draw_line_segment(x1,y1,x2,y2,w)
  local oldtex = gfx.bindtexture(0)
  local cx,cy = 0,0
  local len = 0
  local a = 0
  
  -- calc line center
  cx = ((x2 - x1) / 2)
  cy = ((y2 - y1) / 2)
  
  -- calc line length
  len = math.sqrt(cx*4*cx + cy*4*cy) + w/2
  
  if (len > 0.5) then  
    -- calc angle
    a = math.atan(cy/cx)
  
    gfx.translate(x1 + cx, y1 + cy)
    gfx.rotate(a)

    gfx.drawquad(-len/2, -w/2, len/2, -w/2, len/2, w/2, -len/2, w/2)
  
    gfx.rotate(-a)
    gfx.translate(-(x1 + cx),-(y1 + cy))
  
  end
  gfx.bindtexture(oldtex)
end

function draw_line(points,w)
  for i=2,#points do
    draw_line_segment(points[i-1][1],points[i-1][2], points[i][1],points[i][2], w)
  end
end


function create_spline(control_points, num_segments, w)
  local spline = {control_points = control_points,
                  num_segments = num_segments,
                  segments = {},
                  width = w}
                  
  -- hit test
  function spline:hit(x,y, r)
    if (#self.segments < 3) then
      return false
    end
    
    local p1,p2 = nil,nil
    local p1len,p2len = nil,nil
    
    -- find closest points
    for k,v in pairs(self.segments) do
      local p = {v[1] - x, v[2] - y}
      local len = math.sqrt(p[1]*p[1] + p[2]*p[2])
      
      if (p1 == nil) then
        p1 = v
        p1len = len
      elseif (p2 == nil) then
        p2 = v
        p2len = len
      else
        
        if (len < p1len) then
          p2 = p1
          p2len = p2len
          
          p1 = v
          p1len = len
        elseif (len < p2len) then
          p2 = v
          p2len = len
        end
        
      end
      
    end
    
    -- get distance (from (x,y)) to line segment (betweed p1 and p2)
    local d = math.abs( (p2[1] - p1[1]) * (p1[2] - y) - (p1[1] - x) * (p2[2] - p1[2]) ) / math.sqrt(math.pow(p2[1] - p1[1],2) + math.pow(p2[2] - p1[2],2))
    if (d <= r) then
      print("d = " .. tostring(d))
      return true
    end
    
    return false
  end
  
  function spline:get_line_segment(val)
    local val = clamp(val, 0.0, 1.0)
    local num_points = #self.control_points
    
    -- get current point
    local delta = val * (num_points - 1) + 1
    local current_point = clamp(math.floor(delta), 1, num_points)
    
    local p0 = self.control_points[clamp(current_point - 1, 1, num_points)]
    local p1 = self.control_points[clamp(current_point + 0, 1, num_points)]
    local p2 = self.control_points[clamp(current_point + 1, 1, num_points)]
    local p3 = self.control_points[clamp(current_point + 2, 1, num_points)]
    
    local t = clamp(math.min(delta, num_points) - current_point, 0.0, 1.0)
    local t2 = t*t
    local t3 = t2*t
    
    local ret = {0,0}
    
    ret[1] = 0.5 * ( (2 * p1[1]) +
                     (-p0[1] + p2[1]) * t +
                     (2*p0[1] - 5*p1[1] + 4*p2[1] - p3[1]) * t2 +
                     (-p0[1] + 3*p1[1] - 3*p2[1] + p3[1]) * t3
                   )
    ret[2] = 0.5 * ( (2 * p1[2]) +
                     (-p0[2] + p2[2]) * t +
                     (2*p0[2] - 5*p1[2] + 4*p2[2] - p3[2]) * t2 +
                     (-p0[2] + 3*p1[2] - 3*p2[2] + p3[2]) * t3
                   )
    
    return ret
  end
  
  -- update spline segments
  function spline:update()
    self.segments = {}--{self.control_points[1]}
    local step = 1.0 / self.num_segments
    local t = 0.0
    
    for i=1,self.num_segments do
      local new_p = self:get_line_segment(t)
      table.insert(self.segments, new_p)
      t = t + step
    end
    print("first: " .. self.segments[1][1] .. "," .. self.segments[1][2])
    table.insert(self.segments, self.control_points[#self.control_points])
    
  end
  
  -- draw function
  function spline:draw()
    draw_line(self.segments, self.width)

    -- debug draw control points
    for k,v in pairs(self.control_points) do
      draw_line_segment(v[1]-3, v[2], v[1], v[2], 6)
    end
  end
  
  return spline
end




