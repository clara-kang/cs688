-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
-- kd, ks, shininess, Dielectric?, Glossy?, param, texture_map, normal_map
test_blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25, 1, 0, 1.3, '', '')
gloss_blue = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0, 1, 0.1, '', '')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:translate(0,0,-10)
arc:rotate('Y', 60)
p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:scale(0.8, 4, 0.8)
p1:translate(-2.4, 0, -0.4)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:scale(0.8, 4, 0.8)
p2:translate(1.6, 0, -0.4)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:scale(4, 0.6, 0.6)
s:translate(0, 4, 0)

-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(gloss_blue)
plane:scale(30, 30, 30)

-- -- the floor
-- plane1 = gr.mesh( 'plane1', 'plane.obj' )
-- scene:add_child(plane1)
-- plane1:rotate('x',180)
-- plane1:translate(0,-0.01,0)
-- plane1:set_material(test_blue)
-- plane1:scale(30, 30, 30)

-- the floor floor
plane2 = gr.mesh( 'plane2', 'plane.obj' )
scene:add_child(plane2)
plane2:translate(0,-0.0625,0)
plane2:set_material(grass)
plane2:scale(100,100,100)

-- -- sphere
-- poly = gr.mesh( 'poly', 'dodeca.obj' )
-- scene:add_child(poly)
-- poly:translate(-2, 1.618034, 0)
-- poly:set_material(blue)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0.0001, 0},4000)
-- l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0},400)

gr.render(scene, 'hier.png', 256, 256,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
