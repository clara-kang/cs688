-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
-- test_grass = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 1.3, 'rock.jpg')
test_blue = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 1, 0, 1.6, '', '')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- -- mesh
-- poly = gr.mesh( 'tex_monkey', 'tex_monkey.obj' )
-- scene:add_child(poly)
-- poly:scale(3,3,3)
-- poly:translate(-5, 1.618034, 0)
-- poly:set_material(test_blue)

-- mesh
-- poly = gr.mesh( 'cube', 'uv_cube.obj' )
-- scene:add_child(poly)
-- poly:scale(3,3,3)
-- poly:translate(-5, 1.618034, 0)
-- poly:set_material(test_blue)

-- curves
-- poly = gr.curves( 'particles', 'particle_test.obj' )
-- scene:add_child(poly)
-- poly:scale(5,5,5)
-- -- poly:translate(-5, 1.618034, 0)
-- poly:translate(-5, 0, 0)
-- poly:set_material(gold)

-- sphere
poly = gr.sphere( 'ball' )
scene:add_child(poly)
poly:scale(2,2,2)
poly:translate(-5, 2, 0)
poly:set_material(test_blue)

-- cube
poly = gr.cube( 'cube' )
scene:add_child(poly)
poly:scale(2,2,2)
poly:translate(-6, -1.618034, 0)
poly:set_material(test_blue)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0}, 60000)
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'test.png', 512, 512,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
