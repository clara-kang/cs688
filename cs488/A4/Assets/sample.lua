-- unique scene

brown = gr.material({0.6,0.4,0.2}, {0.0, 0.0, 0.0}, 10)
green = gr.material({0.58,0.8,0.24}, {0.0, 0.0, 0.0}, 10)
green_2 = gr.material({0.84,0.83,0.26}, {0.0, 0.0, 0.0}, 10)
green_3 = gr.material({0.26,0.6,0.32}, {0.0, 0.0, 0.0}, 10)
blue = gr.material({0.7, 0.6, 1}, {0.0, 0.0, 0.0}, 10)
orange = gr.material({0.9, 0.6, 0.207}, {0.0, 0.0, 0.0}, 10)
yellow = gr.material({0.96, 0.827, 0.27}, {0.0, 0.0, 0.0}, 10)
white = gr.material({1.0,1.0,1.0}, {0.0, 0.0, 0.0}, 10)
blue_rf = gr.material({0.7, 1, 1}, {0.0, 0.0, 0.0}, 10, 1.3)


scene = gr.node('scene')
scene:rotate('X', 10)
scene:translate(7, -2, -15)

-- the floor
terrain = gr.mesh( 'terrain', 'terrain.obj' )
scene:add_child(terrain)
terrain:set_material(green_3)
terrain:scale(15,15,15)
terrain:rotate('Y', 180)
terrain:translate(-5,-5,-5)

-- first tree
tree = gr.node('tree')
scene:add_child(tree)
tree:translate(-2,-2.2,-10)
tree:rotate('Y', 60)
tree_trunk = gr.mesh( 'tree_trunk', 'tree_trunk.obj' )
tree:add_child(tree_trunk)
tree_trunk:set_material(brown)

tree_leaf = gr.mesh( 'tree_leaf', 'tree_leaf.obj' )
tree:add_child(tree_leaf)
tree_leaf:set_material(green)

-- second
tree_2 = gr.node('tree2')
scene:add_child(tree_2)
tree_2:rotate('Y',-60)
tree_2:translate(-11.5,-2,1)

tree_2_trunk = gr.mesh( 'tree_trunk', 'tree_trunk.obj' )
tree_2:add_child(tree_2_trunk)
tree_2_trunk:set_material(brown)

tree_2_leaf = gr.mesh( 'tree_leaf', 'tree_leaf.obj' )
tree_2:add_child(tree_2_leaf)
tree_2_leaf:set_material(green_2)

-- first cone tree
cone_tree = gr.node('cone_tree')
scene:add_child(cone_tree)
cone_tree:translate(-2.5,-2.2,-9.5)
cone_tree_trunk = gr.mesh( 'cone_tree_trunk', 'cone_tree_trunk.obj' )
cone_tree:add_child(cone_tree_trunk)
cone_tree_trunk:set_material(brown)

cone_tree_leaf = gr.mesh( 'cone_tree_leaf', 'cone_tree_leaf.obj' )
cone_tree:add_child(cone_tree_leaf)
cone_tree_leaf:set_material(green_3)

-- second cone tree
cone_tree_2 = gr.node('cone_tree_2')
scene:add_child(cone_tree_2)
cone_tree_2:translate(-3.5,-2.2,-6)

cone_tree_2_leaf = gr.mesh( 'cone_tree_leaf', 'cone_tree_leaf.obj' )
cone_tree_2:add_child(cone_tree_2_leaf)
-- cone_tree_2_leaf:scale(1.2,1.2,1.2)
cone_tree_2_leaf:translate(-4,-1.2,0)
cone_tree_2_leaf:set_material(green_3)

-- third tree
tree_3 = gr.node('tree2')
scene:add_child(tree_3)
tree_3:rotate('Y',-120)
tree_3:translate(-8.5,-2.2,-6)

tree_3_trunk = gr.mesh( 'tree_trunk', 'tree_trunk.obj' )
tree_3:add_child(tree_3_trunk)
tree_3_trunk:set_material(brown)

tree_3_leaf = gr.mesh( 'tree_leaf', 'tree_leaf.obj' )
tree_3:add_child(tree_3_leaf)
tree_3_leaf:set_material(green)

-- water
water = gr.mesh('plane', 'plane.obj')
scene:add_child(water)
water:scale(20,20,20)
water:translate(-6,-4.5,0)
water:set_material(blue_rf)

-- tent
tent = gr.node('cone_tree')
scene:add_child(tent)
tent:scale(0.6,0.6,0.6)
tent:rotate('Y',60)
tent:translate(-11,-4,-1)

tent_decor = gr.mesh( 'tent_decor', 'tent_decor.obj' )
tent:add_child(tent_decor)
tent_decor:set_material(orange)

tent_body = gr.mesh( 'tent', 'tent.obj' )
tent:add_child(tent_body)
tent_body:set_material(yellow)

tent_post_1 = gr.mesh( 'tent_wood', 'tent_wood.obj' )
tent:add_child(tent_post_1)
tent_post_1:set_material(brown)

tent_post_2 = gr.mesh( 'tent_wood', 'tent_wood.obj' )
tent:add_child(tent_post_2)
tent_post_2:translate(0,0,-1.5)
tent_post_2:set_material(brown)

-- tent
cloud = gr.node('cloud')
scene:add_child(cloud)
cloud:translate(-6,8,1)

cloud_body = gr.mesh( 'cloud', 'cloud.obj' )
cloud:add_child(cloud_body)
cloud_body:set_material(white)

cloud_body2 = gr.mesh( 'cloud', 'cloud.obj' )
cloud:add_child(cloud_body2)
cloud_body2:rotate('x',-45)
cloud_body2:rotate('y',-45)
cloud_body2:translate(5,0,1)
cloud_body2:set_material(white)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})

gr.render(scene, 'sample.png', 512, 512,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1})
