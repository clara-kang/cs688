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
orange = gr.material({1.0, 0.6, 0.2}, {0.1, 0.1, 0.1}, 10)

torso = gr.node('torso')
rootnode:add_child(torso)

torsoMesh = gr.mesh('sphere', 'torso')
torso:add_child(torsoMesh)
torsoMesh:set_material(orange)
torsoMesh:scale(0.5,0.6,0.5);

neckJoint = gr.joint('neckJoint', {0.0, 0.0, 0.0}, {-70.0, 0.0, 70.0})
torso:add_child(neckJoint)
neckJoint:translate(0.0, 0.6, 0.0)

head = gr.mesh('head', 'head')
neckJoint:add_child(head)
head:scale(0.4, 0.4, 0.4)
head:translate(0.0, 0.3, 0.0)
head:set_material(orange)

mustache = gr.mesh('mustache', 'mustache')
head:add_child(mustache)
--mustache:scale(0.4, 0.4, 0.4)
--mustache:translate(0.0, 0.9, 0.0)
mustache:set_material(white)

leftEar = gr.mesh('ear', 'leftEar')
head:add_child(leftEar)
leftEar:rotate('z', 15.0)
leftEar:translate(0.0, 0.0, 0.0)
leftEar:set_material(orange)

rightEar = gr.mesh('ear', 'rightEar')
head:add_child(rightEar)
rightEar:rotate('z', -15.0)
rightEar:translate(0.7, -0.2, 0.0)
rightEar:set_material(orange)

return rootnode
