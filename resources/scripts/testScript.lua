function testScript.onStart(self)
    self.Transform = self:Get(TRANSFORM)
    print("Script Started.")
end

function testScript.onUpdate(self, dt)
    if inputs.isKey(inputs.KEY_A) then
        self:ApplyForce(Vec3.new(-100.0, 0.0, 0.0));
    end
    if inputs.isKey(inputs.KEY_D) then
        self:ApplyForce(Vec3.new(100.0, 0.0, 0.0));
    end
    -- self.Transform.Scale.y = self.Transform.Scale.y+dt
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
