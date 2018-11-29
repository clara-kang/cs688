-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
white = gr.material({1, 1, 1}, {1, 1, 1}, 25)
-- kd, ks, shininess, Dielectric?, Glossy?, param, texture_map, normal_map
test_blue = gr.material({0.7, 0.6, 1}, {0.8,0.8,0.8}, 25, 1, 0, 1.6, '', '')
gloss_blue = gr.material({1.0,1.0,1.0}, {0.8,0.8,0.8}, 25, 0, 1, 0.1, '', '')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0, 0, -15)
--
-- the floor
plane = gr.mesh( 'plane', 'plane.obj' )
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)
plane:translate(0, -10, 0)

-- left wall
lwall = gr.mesh( 'lwall', 'plane.obj' )
scene:add_child(lwall)
lwall:scale(30, 30, 30)
lwall:rotate('z', 270)
lwall:translate(-20, 0, 0)
lwall:set_material(grass)

-- right wall
rwall = gr.mesh( 'rwall', 'plane.obj' )
scene:add_child(rwall)
rwall:scale(30, 30, 30)
rwall:rotate('z', 90)
rwall:translate(20, 0, 0)
rwall:set_material(gold)

-- back wall
bwall = gr.mesh( 'bwall', 'plane.obj' )
scene:add_child(bwall)
bwall:scale(30,30,30)
bwall:rotate('x', 90)
bwall:translate(0, 0.5, -20)
bwall:set_material(blue)

-- ceiling
ceiling = gr.mesh( 'bwall', 'plane.obj' )
scene:add_child(ceiling)
ceiling:scale(30,30,30)
ceiling:rotate('z',180)
ceiling:translate(0, 25, -0)
ceiling:set_material(blue)

-- -- sphere
-- ball = gr.sphere( 'ball' )
-- scene:add_child(ball)
-- ball:scale(6,6,6)
-- ball:translate(8, -4, 6)
-- ball:set_material(test_blue)

s1 = gr.nh_sphere('s1', {8, -4, 6}, 6)
scene:add_child(s1)
s1:set_material(test_blue)

-- The lights
l1 = gr.light({-1, 4, 0}, {0.8, 0.8, 0.8}, {1, 0, 0}, 6000)
-- l1 = gr.light({0, 0, 40}, {0.8, 0.8, 0.8}, {1, 0, 0}, 60000)
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'room.png', 128, 128,
	  {0, -0, 40}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
