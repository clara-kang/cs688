-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
-- test_grass = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 1.3, 'rock.jpg')
test_blue = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 0.5, '', 'leather.jpg')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- -- the arc
-- arc = gr.node('arc')
-- scene:add_child(arc)
-- arc:translate(0,0,-10)
-- arc:rotate('Y', 60)
-- p1 = gr.cube('p1')
-- arc:add_child(p1)
-- p1:set_material(gold)
-- p1:scale(0.8, 4, 0.8)
-- p1:translate(-2.4, 0, -0.4)
--
-- p2 = gr.cube('p2')
-- arc:add_child(p2)
-- p2:set_material(gold)
-- p2:scale(0.8, 4, 0.8)
-- p2:translate(1.6, 0, -0.4)
--
-- s = gr.sphere('s')
-- arc:add_child(s)
-- s:set_material(gold)
-- s:scale(4, 0.6, 0.6)
-- s:translate(0, 4, 0)
--
-- -- the floor
-- plane = gr.mesh( 'plane', 'plane.obj' )
-- scene:add_child(plane)
-- plane:set_material(grass)
-- plane:scale(30, 30, 30)

-- mesh
poly = gr.mesh( 'tex_monkey', 'tex_monkey.obj' )
scene:add_child(poly)
poly:scale(3,3,3)
poly:translate(-5, 1.618034, 0)
poly:set_material(test_blue)

-- curves
-- poly = gr.curves( 'particles', 'particle_test.obj' )
-- scene:add_child(poly)
-- poly:scale(5,5,5)
-- -- poly:translate(-5, 1.618034, 0)
-- poly:translate(-5, 0, 0)
-- poly:set_material(gold)

-- -- sphere
-- poly = gr.sphere( 'ball' )
-- scene:add_child(poly)
-- poly:scale(3,3,3)
-- poly:translate(-5, 1.618034, 0)
-- poly:set_material(test_blue)

-- -- cube
-- poly = gr.cube( 'cube' )
-- scene:add_child(poly)
-- poly:scale(3,3,3)
-- poly:translate(-5, 1.618034, 0)
-- poly:set_material(test_blue)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0})
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'test.png', 512, 512,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
