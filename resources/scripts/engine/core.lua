local flowScript = {}
local flowScript_mt = {__index = flowScript}

function flowScript.Constructor(entity)
    local self = setmetatable({}, flowScript_mt)
    self.Entity = entity
    return self
end

function flowScript.Destroy(entity)
    APIDestroy(entity)
end

function Initializer()
    local ScriptKlass = {}
    local ScriptKlass_mt = {__index = ScriptKlass}
    setmetatable(ScriptKlass, {__index = flowScript})

    function ScriptKlass.Constructor(entity)
        local obj = flowScript.Constructor(entity)
        self = setmetatable(obj, ScriptKlass_mt)
        return self
    end

    function ScriptKlass:ApplyForce(force)
        APIApplyForce(self.Entity, force)
    end

    function ScriptKlass:Destroy()
        APIDestroy(self.Entity)
    end

    function ScriptKlass:Get(type)
        if type == TRANSFORM then
            return APIGetTransform(self.Entity)
        end
        print("Invalid Type: ", type)
        return {}
    end

    return ScriptKlass
end

return flowScript
