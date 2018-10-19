-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:rotate('y', -45.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('cube', 'torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(0.5,1.0,0.5);

leftShoulder = gr.mesh('sphere', 'leftShoulder')
leftShoulder:scale(2.0,1.0,1/0.5);
leftShoulder:scale(0.2, 0.2, 0.2)
-- leftShoulder:translate(-0.4, 0.35, 0.0)
leftShoulder:translate(0.0, 0.0, 0.0)
leftShoulder:set_material(blue)

leftShoulderJoint = gr.joint('shoulderJoint', {-90.0, 0.0, 90.0}, {-45.0, 0.0, 90.0})
-- leftShoulderJoint:rotate('x', -45.0)
leftShoulderJoint:translate(-0.05, 0.1, 0.0)
leftShoulderJoint:add_child(leftShoulder);
torso:add_child(leftShoulderJoint)

leftArm = gr.mesh('cube', 'leftArm')
leftArm:scale(3.0, 0.3, 0.3)
leftArm:translate(-0.08, -0.05, 0.0)
leftArm:rotate('z', 45);
leftShoulder:add_child(leftArm)
leftArm:set_material(red)

leftElbowJoint = gr.joint('leftElbowJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 90.0})
leftElbowJoint:scale(1.0, 1.0, 1.0)
leftElbowJoint:translate(-0.09, 0.0, 0.0)
-- leftElbowJoint:rotate('x',90)
-- leftElbowJoint:rotate('y',90)
leftArm:add_child(leftElbowJoint)

leftLowerArm = gr.mesh('cube', 'leftLowerArm')
leftLowerArm:scale(0.5, 1.0, 1.0)
leftLowerArm:translate(-0.04, 0.0, 0.0)
-- leftElbowJoint:rotate('y',90)
leftElbowJoint:add_child(leftLowerArm)
leftLowerArm:set_material(red)



return rootnode
