-- angry_bird.lua

rootnode = gr.node('root')
rootnode:rotate('y', -45.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(red)
torso:scale(0.5,0.6,0.5)

neckJoint = gr.joint('neckJoint', {0.0, 0.0, 0.0}, {-70.0, 0.0, 70.0})
torso:add_child(neckJoint)
neckJoint:translate(0.0, 0.6, 0.0)

neck = gr.mesh('sphere', 'neck')
neckJoint:add_child(neck)
neck:scale(0.9,0.9,0.9)
-- neck:translate(0.0, 0.6, 0.0)
neck:set_material(red)

head = gr.mesh('sphere', 'head')
neck:add_child(head)
head:scale(0.95, 0.95, 0.95)
head:translate(0.0, 0.5, 0.0)
head:set_material(red)

leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.4,0.4,0.4)
leftEye:translate(-0.2, 0.0, 0.6)
leftEye:set_material(white)

leftEyeBrow = gr.mesh('cube', 'leftEyeBrow')
leftEye:add_child(leftEyeBrow)
leftEyeBrow:scale(1.7,0.5,0.5)
leftEyeBrow:rotate('z', -20)
leftEyeBrow:translate(-0.2,0.6,0.8)
leftEyeBrow:set_material(black)

rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.4,0.4,0.4)
rightEye:translate(0.2, 0.0, 0.6)
rightEye:set_material(white)

rightEyeBrow = gr.mesh('cube', 'rightEyeBrow')
rightEye:add_child(rightEyeBrow)
rightEyeBrow:scale(1.7,0.5,0.5)
rightEyeBrow:rotate('z', 20)
rightEyeBrow:translate(0.2,0.6,0.8)
rightEyeBrow:set_material(black)

leftArm = gr.mesh('sphere', 'leftArm')
leftArm:scale(0.1, 0.6, 0.35)
leftArm:translate(0.0, -0.7, 0.0)
leftArm:set_material(red)

leftArmJoint = gr.joint('leftArmJoint',{-45.0,0.0,45.0},{0.0,0.0,0.0})
torso:add_child(leftArmJoint)
leftArmJoint:rotate('y', 180);
leftArmJoint:rotate('z', -30);
leftArmJoint:translate(-0.8, 0.9, 0.0)
leftArmJoint:add_child(leftArm)

lowerleftArm = gr.mesh('sphere', 'lowerleftArm')
lowerleftArm:scale(0.4, 0.6, 0.4)
lowerleftArm:translate(0.0, -0.8, 0.0)
lowerleftArm:set_material(blue)

lowerleftArmJoint = gr.joint('lowerleftArmJoint',{0.0,0.0,100.0},{0.0,0.0,0.0})
leftArm:add_child(lowerleftArmJoint)
lowerleftArmJoint:translate(0.0,-0.6,0.0)
lowerleftArmJoint:add_child(lowerleftArm)




return rootnode
