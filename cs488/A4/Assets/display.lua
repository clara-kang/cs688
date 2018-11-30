-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

white = gr.material({1.0,1.0,1.0}, {1.0,1.0,1.0}, 25)
-- test_grass = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 1.3, 'rock.jpg')
test_blue = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 1, 0, 1.6, '', '')
tex = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 0, 1.6, 'rock.jpg', 'leather.jpg')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(6, -2, -15)

-- mesh
bwall = gr.mesh( 'bwall', 'mesh/back_wall.obj' )
scene:add_child(bwall)
poly:set_material(white)

-- -- mesh
-- poly = gr.mesh( 'cube', 'mesh/uv_cube.obj' )
-- scene:add_child(poly)
-- poly:scale(3,3,3)
-- poly:translate(-5, 1.618034, 0)
-- poly:set_material(tex)

-- pumpkin
poly = gr.mesh( 'cube', 'mesh/pumpkin_noUV.obj' )
scene:add_child(poly)
poly:scale(3,3,3)
poly:translate(-5, 1.618034, 0)
poly:set_material(gold)

-- -- curves
-- poly = gr.curves( 'particles', 'fur.obj' )
-- scene:add_child(poly)
-- poly:scale(2,2,2)
-- -- poly:translate(-5, 1.618034, 0)
-- poly:translate(-5, 0, -1)
-- poly:set_material(gold)

-- -- sphere
-- poly = gr.sphere( 'ball' )
-- scene:add_child(poly)
-- poly:scale(2,2,2)
-- poly:translate(-5, 2, 0)
-- poly:set_material(tex)

-- -- cube
-- poly = gr.cube( 'cube' )
-- scene:add_child(poly)
-- poly:scale(2,2,2)
-- poly:translate(-6, -1.618034, 0)
-- poly:set_material(test_blue)

-- The lights
l1 = gr.light({0, 0, 0}, {0.8, 0.8, 0.8}, {1, 0, 0}, 60000, 0.05)
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'test.png', 512, 512,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1}, 100, 0)
