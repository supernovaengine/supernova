v1 = Engine.new()
--v1:teste(Scaling.CROP)

function eduardo()
    print('Update')
end

--Engine.setScalingMode(Scaling.CROP)
--v1.onUpdate = eduardo

--Engine.onUpdate2(eduardo)
Engine.onViewLoaded:add('teste',eduardo)

Engine.onViewLoaded()