function testScript.onStart(self)
    self.Transform = self:Get(TRANSFORM)
    self.LastMouseX = nil
    self.LastMouseY = nil
    print("Script Started.")
end

function testScript.onUpdate(self, dt)
    -- Mouse look
    local pos = self:GetMousePos()
    if self.LastMouseX == nil then
        self.LastMouseX = pos.x
        self.LastMouseY = pos.y
    end
    local dx = pos.x - self.LastMouseX
    local dy = pos.y - self.LastMouseY
    self.LastMouseX = pos.x
    self.LastMouseY = pos.y

    local ctrl = self:GetController()
    ctrl.Yaw = ctrl.Yaw - dx * 0.002
    ctrl.Pitch = ctrl.Pitch - dy * 0.002
    ctrl.Pitch = math.max(-1.5, math.min(1.5, ctrl.Pitch))
    print("AFTER_MOUSE_LOOK")

    -- Movement
    local mx, mz = 0, 0
    if inputs.isKey(inputs.KEY_W) then mz = mz + 1 end
    if inputs.isKey(inputs.KEY_S) then mz = mz - 1 end
    if inputs.isKey(inputs.KEY_D) then mx = mx + 1 end
    if inputs.isKey(inputs.KEY_A) then mx = mx - 1 end
    -- self:SetMove(mx, mz)
    print(type(APISetControllerMove))
    print(type(self.SetMove))
    print(type(self.Entity))
    APISetControllerMove(self.Entity, mx, mz)
end

function testScript.onCollision(self, other)
    print("Collision:", self.Entity, "->", other)
    self:Destroy(other);
end

function testScript.onResize(self, width, height)
    print("Resize: ", width, height)
end

-- call when mouse is down
function testScript.onMouseDown(self, button)
   print("Mousedown: ", button)
end

-- call when key down
function testScript.onKeyDown(self, key)
   print("Keydown: ", key)
end

-- call when entity is detroyed
function testScript.onDestroy(self)
   print("Destroyed:", self.Entity)
end
