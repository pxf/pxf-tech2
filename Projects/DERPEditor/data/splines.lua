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
  
  -- calc angle
  a = math.atan(cy/cx)
  
  gfx.translate(x1 + cx, y1 + cy)
  gfx.rotate(a)

  gfx.drawquad(-len/2, -w/2, len/2, -w/2, len/2, w/2, -len/2, w/2)
  
  gfx.rotate(-a)
  gfx.translate(-(x1 + cx),-(y1 + cy))
  gfx.bindtexture(oldtex)
end

function draw_line(points,w)
  local oldx,oldy = nil,nil
  for k,v in pairs(points) do
    if not (oldx == nil) then
      draw_line_segment(oldx,oldy,v[1],v[2],w)
    end
    
    oldx = v[1]
    oldy = v[2]
  end
end

function draw_spline(points,segments,w)
  local new_points = {}
  local point_count = #points
  local step = 1.0 / segments
  local t = 0.0
  
  for i=1,segments do
    local p0, p1, p2, p3 = {0,0},{0,0},{0,0},{0,0}
    local i0, i1, i2, i3 = 0,0,0,0
    local new_p = {0,0}
    local deltat = 0.0
    
    deltat = ((point_count - 1) * t) + 1
    i1 = math.floor(deltat)
    i2 = i1 + 1
    i3 = i2 + 1
    i0 = i1 - 1
    
    if (i1 <= 1) then
      i1 = 1
    end
    
    if (i0 <= 1) then
      i0 = 1
    end
    
    if (i2 >= point_count) then
      i2 = point_count
    end
    
    if (i3 >= point_count) then
      i3 = point_count
    end
    
    deltat = deltat - i1
    
    p0 = points[i0]
    p1 = points[i1]
    p2 = points[i2]
    p3 = points[i3]
    
    new_p[1] = 0.5 * ( (2 * p1[1]) +
                       (-p0[1] + p2[1]) * deltat +
                       (2*p0[1] - 5*p1[1] + 4*p2[1] - p3[1]) * (deltat*deltat) +
                       (-p0[1] + 3*p1[1] - 3*p2[1] + p3[1]) * (deltat*deltat*deltat)
                     )
    
    new_p[2] = 0.5 * ( (2 * p1[2]) +
                       (-p0[2] + p2[2]) * deltat +
                       (2*p0[2] - 5*p1[2] + 4*p2[2] - p3[2]) * (deltat*deltat) +
                       (-p0[2] + 3*p1[2] - 3*p2[2] + p3[2]) * (deltat*deltat*deltat)
                     )
    
    table.insert(new_points, new_p)
    t = t + step
  end
  
  draw_line(new_points, w)
  
  -- debug draw control points
  for k,v in pairs(points) do
    draw_line_segment(v[1]-2, v[2]-2, v[1]+2, v[2]+2, 2)
  end
end