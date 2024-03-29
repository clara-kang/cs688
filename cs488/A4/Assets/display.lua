-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

white = gr.material({1, 1, 1}, {1, 1, 1}, 25)
brown = gr.material({0.52, 0.419, 0.18}, {1, 1, 1}, 25)
orange = gr.material({1, 0.55, 0.1}, {1, 0.55, 0.1}, 25)
green = gr.material({0.2588, 0.466, 0.02}, {0.2588, 0.466, 0.02}, 5)
pale_yellow = gr.material({0.996, 0.97255, 0.80392}, {0.996, 0.97255, 0.80392}, 50)
-- test_grass = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 25, 1.3, 'rock.jpg')
glass = gr.material({1.0,1.0,1.0}, {1.0,1.0,1.0}, 25, 1, 0, 1.6, '', '')
red_glass = gr.material({1.0,0.0,0.0}, {1.0,1.0,1.0}, 25, 1, 0, 1.6, '', '')
glossy_brown = gr.material({0.52, 0.419, 0.18}, {0.5,0.5,0.5}, 25, 0, 1, 0.1, '', '')
glossy_grey = gr.material({0.6,0.6,0.6}, {0.8,0.8,0.8}, 25, 0, 1, 0.1, '', '')
linen_knit = gr.material({1, 0.55, 0.1}, {1, 0.55, 0.1}, 70, 0, 0, 0, 'textures/linen.png', 'textures/knit.jpg')
wallpaper = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 0, 0, 'textures/wallpaper.jpg', '')
wood = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 0, 0, 'textures/wood.jpg', '')
wood_gloss = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 1, 0.1, 'textures/wood.jpg', '')
linen = gr.material({1, 1, 1}, {0.8,0.8,0.8}, 70, 0, 0, 0, 'textures/linen.png', '')
black_cloth = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 0, 0, 'textures/black_cloth.jpg', '')

scene = gr.node('scene')
scene:rotate('X', 23)
scene:translate(0, 0, -5)

-- mesh
bwall = gr.mesh( 'bwall', 'mesh/back_wall.obj' )
scene:add_child(bwall)
bwall:set_material(wallpaper)

-- mesh
table = gr.mesh( 'table', 'mesh/table.obj' )
scene:add_child(table)
table:set_material(wood)

-- -- pumpkin
-- pumpkin = gr.mesh( 'pumpkin', 'mesh/pumpkin.obj' )
-- scene:add_child(pumpkin)
-- pumpkin:set_material(linen_knit)
--
-- -- pumpkin stem
-- stem = gr.mesh( 'stem', 'mesh/pumpkin_stem.obj' )
-- scene:add_child(stem)
-- stem:set_material(brown)
--
-- -- wine glass
-- wine_glass = gr.mesh( 'wine_glass', 'mesh/wine_glass.obj' )
-- scene:add_child(wine_glass)
-- wine_glass:set_material(glass)

-- -- candle cup
-- candle_cup = gr.mesh( 'candle_cup', 'mesh/candle_cup_display.obj' )
-- scene:add_child(candle_cup)
-- candle_cup:set_material(glass)

-- -- candle
-- candle = gr.mesh( 'candle', 'mesh/candle_display.obj' )
-- scene:add_child(candle)
-- candle:set_material(pale_yellow)

-- -- colored glass ball
-- r_glass_ball = gr.nh_sphere('s1', {0, 0, 0}, 0.5)
-- scene:add_child(r_glass_ball)
-- r_glass_ball:set_material(red_glass)
--
-- normal glass ball
n_glass_ball = gr.nh_sphere('s1', {0, 0, 0}, 0.5)
scene:add_child(n_glass_ball)
n_glass_ball:set_material(glass)

-- -- ball
-- s2 = gr.nh_sphere('s2', {1, 0, -1}, 0.5)
-- scene:add_child(s2)
-- s2:set_material(linen)

--
-- node = gr.node('node')
-- scene:add_child(node)
-- node:scale(2,2,2)
-- node:translate(7, -2, 4)
--
-- -- bat wing
-- bat_wing = gr.mesh( 'bat_wing', 'final_mesh/bat_wing.obj' )
-- node:add_child(bat_wing)
-- bat_wing:set_material(black_cloth)
--
-- -- bat core
-- bat_core = gr.mesh( 'bat_core', 'final_mesh/bat_core.obj' )
-- node:add_child(bat_core)
-- bat_core:set_material(orange)
--
-- -- bat fur
-- bat_fur = gr.curves( 'bat_fur', 'final_mesh/bat_fur.obj' )
-- node:add_child(bat_fur)
-- bat_fur:set_material(orange)
--


-- The lights
l1 = gr.light({-20, 20, 20}, {1, 1, 1}, {1, 0, 0}, 0, 0.1)
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'display.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1}, 0.08, -4)
