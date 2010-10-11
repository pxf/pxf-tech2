--[[
  Special window handling for the GUI.
]]

gui.windows = {windowlist = {}}

function gui.windows:add(wnd)
  -- add window to top of list
  table.insert(self.windowlist, 1, wnd)
  
  -- add to widget root
  self.widgets:addwidget(wnd)
end

function gui.windows:remove(wnd)
  -- remove from window list
  table.remove(self.windowlist, wnd)
end

function gui.window:setactive(wnd)
  -- remove from list
  table.remove(self.windowlist, wnd)
  -- ... and re-insert it at the top
  table.insert(self.windowlist, 1, wnd)
end