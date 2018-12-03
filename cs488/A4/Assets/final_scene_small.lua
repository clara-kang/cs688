-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

white = gr.material({1, 1, 1}, {1, 1, 1}, 25)
brown = gr.material({0.52, 0.419, 0.18}, {1, 1, 1}, 25)
orange = gr.material({1, 0.55, 0.1}, {1, 0.55, 0.1}, 25)
pale_yellow = gr.material({0.996, 0.97255, 0.80392}, {0.996, 0.97255, 0.80392}, 50)
pumpkin_yellow = gr.material({0.7968, 0.778, 0.6431}, {0.5,0.5,0.5}, 50)
yellow = gr.material({0.988, 0.843, 0.01176}, {1, 1, 1}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.8,0.8,0.8}, 25)
black_gloss = gr.material({0.1, 0.1, 0.1}, {0.8,0.8,0.8}, 25, 0, 1, 0.08,'','')
red_brown_gloss = gr.material({0.5686, 0.2706, 0}, {0.2,0.2,0.2}, 25, 0, 1, 0.08,'','')
green = gr.material({0.2588, 0.466, 0.02}, {0.2588, 0.466, 0.02}, 5)
glass = gr.material({1.0,1.0,1.0}, {1.0,1.0,1.0}, 25, 1, 0, 1.6, '', '')
linen_knit = gr.material({1, 0.55, 0.1}, {1, 0.55, 0.1}, 70, 0, 0, 0, 'textures/linen.png', 'textures/knit.jpg')
wallpaper = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 0, 0, 'textures/wallpaper.jpg', '')
wood = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 0, 0, 'textures/wood.jpg', '')
wood_gloss = gr.material({0.7, 0.6, 1.0}, {0.8,0.8,0.8}, 25, 0, 1, 0.08, 'textures/wood.jpg', '')
red_wood = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 0, 0, 'textures/red_wood.jpg', '')
yellow_leaf = gr.material({0,0,0}, {0,0,0}, 50, 0, 0, 0, 'textures/leaf1.png', '')
red_leaf = gr.material({0,0,0}, {0,0,0}, 50, 0, 0, 0, 'textures/leaf2.png', '')
pear_skin = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 0, 0, 'textures/pear.jpg', '')
pear_skin_gloss = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 1, 0.08, 'textures/pear.jpg', '')
black_cloth = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 0, 0, 'textures/black_cloth.jpg', '')
brown_leather = gr.material({0,0,0}, {0.8,0.8,0.8}, 50, 0, 0, 0, 'textures/acorn_hat.JPG', '')

scene = gr.node('scene')
scene:rotate('Y', 30)
scene:rotate('X', 23)
scene:translate(1.8, -1, -7)

-- table
table = gr.mesh( 'table', 'final_mesh/table.obj' )
scene:add_child(table)
table:set_material(wood)

-- wall
wall = gr.mesh( 'wall', 'final_mesh/wall.obj' )
scene:add_child(wall)
wall:set_material(wallpaper)

-- pumpkin small
pumpkin_small = gr.mesh( 'pumpkin_small', 'final_mesh/pumpkin_small.obj' )
scene:add_child(pumpkin_small)
pumpkin_small:set_material(pumpkin_yellow)

-- pumpkin small stem
pumpkin_small_stem = gr.mesh( 'pumpkin_small_stem', 'final_mesh/pumpkin_small_stem.obj' )
scene:add_child(pumpkin_small_stem)
pumpkin_small_stem:set_material(pumpkin_yellow)

-- pumpkin large
pumpkin_large = gr.mesh( 'pumpkin_large', 'final_mesh/pumpkin_large.obj' )
scene:add_child(pumpkin_large)
pumpkin_large:set_material(red_wood)

-- pumpkin large stem
pumpkin_large_stem = gr.mesh( 'pumpkin_large_stem', 'final_mesh/pumpkin_large_stem.obj' )
scene:add_child(pumpkin_large_stem)
pumpkin_large_stem:set_material(brown)

-- pumpkin mid
pumpkin_mid = gr.mesh( 'pumpkin_mid', 'final_mesh/pumpkin_mid.obj' )
scene:add_child(pumpkin_mid)
pumpkin_mid:set_material(linen_knit)

-- pumpkin stem
pumpkin_mid_stem = gr.mesh( 'pumpkin_mid_stem', 'final_mesh/pumpkin_mid_stem.obj' )
scene:add_child(pumpkin_mid_stem)
pumpkin_mid_stem:set_material(brown)

-- branch
branch = gr.mesh( 'branch', 'final_mesh/branch.obj' )
scene:add_child(branch)
branch:set_material(brown)

-- fruits
fruits = gr.mesh( 'fruits', 'final_mesh/fruits.obj' )
scene:add_child(fruits)
fruits:set_material(yellow)

-- box big
box_big = gr.mesh( 'box_big', 'final_mesh/box_big.obj' )
scene:add_child(box_big)
box_big:set_material(wood)

-- box small
box_small = gr.mesh( 'box_small', 'final_mesh/box_small.obj' )
scene:add_child(box_small)
box_small:set_material(wood)

-- leaf red 1
leaf_red1 = gr.mesh( 'leaf_red1', 'final_mesh/leaf_red1.obj' )
scene:add_child(leaf_red1)
leaf_red1:set_material(red_leaf)

-- leaf red 2
leaf_red2 = gr.mesh( 'leaf_red2', 'final_mesh/leaf_red2.obj' )
scene:add_child(leaf_red2)
leaf_red2:set_material(red_leaf)

-- leaf red 3
leaf_red3 = gr.mesh( 'leaf_red3', 'final_mesh/leaf_red3.obj' )
scene:add_child(leaf_red3)
leaf_red3:set_material(red_leaf)

-- leaf red 4
leaf_red4 = gr.mesh( 'leaf_red4', 'final_mesh/leaf_red4.obj' )
scene:add_child(leaf_red4)
leaf_red4:set_material(red_leaf)

-- leaf yellow 1
leaf_yellow1 = gr.mesh( 'leaf_yellow', 'final_mesh/leaf_yellow1.obj' )
scene:add_child(leaf_yellow1)
leaf_yellow1:set_material(yellow_leaf)

-- leaf yellow 2
leaf_yellow2 = gr.mesh( 'leaf_yellow', 'final_mesh/leaf_yellow2.obj' )
scene:add_child(leaf_yellow2)
leaf_yellow2:set_material(yellow_leaf)

-- leaf yellow 3
leaf_yellow3 = gr.mesh( 'leaf_yellow', 'final_mesh/leaf_yellow3.obj' )
scene:add_child(leaf_yellow3)
leaf_yellow3:set_material(yellow_leaf)

-- pear
pear = gr.mesh( 'pear', 'final_mesh/pear.obj' )
scene:add_child(pear)
pear:set_material(pear_skin)

-- candle cup small
candle_cup_small = gr.mesh( 'candle_cup_small', 'final_mesh/candle_cup_small.obj' )
scene:add_child(candle_cup_small)
candle_cup_small:set_material(glass)

-- candle cup
candle_cup = gr.mesh( 'candle_cup', 'final_mesh/candle_cup.obj' )
scene:add_child(candle_cup)
candle_cup:set_material(glass)

-- candle 1
candle1 = gr.mesh( 'candle1', 'final_mesh/candle1.obj' )
scene:add_child(candle1)
candle1:set_material(pale_yellow)

-- candle 2
candle2 = gr.mesh( 'candle2', 'final_mesh/candle2.obj' )
scene:add_child(candle2)
candle2:set_material(pale_yellow)

-- bat wing
bat_wing = gr.mesh( 'bat_wing', 'final_mesh/bat_wing.obj' )
scene:add_child(bat_wing)
bat_wing:set_material(black_cloth)

-- bat core
bat_core = gr.mesh( 'bat_core', 'final_mesh/bat_core.obj' )
scene:add_child(bat_core)
bat_core:set_material(orange)

-- bat fur
bat_fur = gr.curves( 'bat_fur', 'final_mesh/bat_fur.obj' )
scene:add_child(bat_fur)
bat_fur:set_material(orange)

-- lantern
lantern = gr.mesh( 'lantern', 'final_mesh/lantern.obj' )
scene:add_child(lantern)
lantern:set_material(black)

-- wine glass short
wine_glass_short = gr.mesh( 'wine_glass_short', 'final_mesh/wine_glass_short.obj' )
scene:add_child(wine_glass_short)
wine_glass_short:set_material(glass)

-- wine glass tall
wine_glass_tall = gr.mesh( 'wine_glass_tall', 'final_mesh/wine_glass_tall.obj' )
scene:add_child(wine_glass_tall)
wine_glass_tall:set_material(glass)

-- acorn hat
acorn_hat = gr.mesh( 'acorn_hat', 'final_mesh/acorn_hat.obj' )
scene:add_child(acorn_hat)
acorn_hat:set_material(brown_leather)

-- acorn
acorn = gr.mesh( 'acorn', 'final_mesh/acorn.obj' )
scene:add_child(acorn)
acorn:set_material(red_brown_gloss)

-- acorn hat 2
acorn_hat2 = gr.mesh( 'acorn_hat2', 'final_mesh/acorn_hat2.obj' )
scene:add_child(acorn_hat2)
acorn_hat2:set_material(brown_leather)

-- acorn
acorn2 = gr.mesh( 'acorn2', 'final_mesh/acorn2.obj' )
scene:add_child(acorn2)
acorn2:set_material(red_brown_gloss)

-- The lights
l1 = gr.light({-10, 20, 20}, {1, 1, 0.902}, {1, 0, 0}, 20000, 0.1)
-- l2 = gr.light({0, 5, -20}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'test.png', 100, 100,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1}, 0.08, -4)
