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

torsoMesh = gr.mesh('body', 'torso')
torso:add_child(torsoMesh)
torsoMesh:set_material(orange)
torsoMesh:scale(0.5,0.6,0.6)
torsoMesh:translate(0.0,0.3,0.0)

neckJoint = gr.joint('neckJoint', {0.0, 0.0, 0.0}, {-70.0, 0.0, 70.0})
torso:add_child(neckJoint)
neckJoint:translate(0.0, 0.6, 0.0)

head = gr.mesh('head', 'head')
neckJoint:add_child(head)
head:scale(0.4, 0.4, 0.4)
head:translate(0.0, 0.3, 0.0)
head:set_material(orange)

leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.4,0.5,0.4)
leftEye:translate(-0.2, 0.1, 0.23)
leftEye:set_material(white)

rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.4,0.5,0.4)
rightEye:translate(0.2, 0.1, 0.23)
rightEye:set_material(white)

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

leftArmJoint = gr.joint('leftArmJoint',{0.0,0.0,0.0},{-60.0,0.0,60.0})
torso:add_child(leftArmJoint)
leftArmJoint:rotate('y', 180);
leftArmJoint:rotate('z', -30);
leftArmJoint:translate(-0.28, 0.40, 0.0)

leftArm = gr.mesh('cylinder', 'leftArm')
leftArm:scale(0.08, 0.3, 0.08)
leftArm:translate(0.0, -0.22, 0.0)
leftArm:set_material(orange)
leftArmJoint:add_child(leftArm)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
leftShoulder:scale(0.1,0.1,0.1)
leftShoulder:set_material(orange)
leftArmJoint:add_child(leftShoulder)

leftWristJointP = gr.node('leftWristJointP')
leftWristJointP:rotate('x',90)
leftWristJointP:translate(0.0, -0.55, 0.0)
leftArmJoint:add_child(leftWristJointP)

leftWristJoint = gr.joint('leftWristJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
leftWristJointP:add_child(leftWristJoint)
--leftWristJoint:translate(0.0, -0.55, 0.0)

leftWrist = gr.mesh('sphere', 'leftWrist')
leftWrist:scale(0.08,0.08,0.08)
leftWrist:set_material(orange)
leftWristJoint:add_child(leftWrist)

lefthand = gr.mesh('sphere', 'hand')
lefthand:scale(0.08,0.15,0.15)
lefthand:translate(0.0,0.0,0.2)
lefthand:set_material(orange)
leftWristJoint:add_child(lefthand)

leftHipJoint = gr.joint('leftHipJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
torso:add_child(leftHipJoint)
leftHipJoint:translate(-0.2, -0.25, 0.0)

leftHip = gr.mesh('sphere', 'leftHip')
leftHip:scale(0.08,0.08,0.08)
--leftArm:translate(0.0, -0.1, 0.0)
leftHip:set_material(orange)
leftHipJoint:add_child(leftHip)

leftUpLeg = gr.mesh('cylinder', 'leftUpLeg')
leftUpLeg:scale(0.08, 0.15, 0.08)
leftUpLeg:translate(0.0, -0.2, 0.0)
leftUpLeg:set_material(orange)
leftHipJoint:add_child(leftUpLeg)

leftKneeJoint = gr.joint('leftKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
leftHipJoint:add_child(leftKneeJoint)
leftKneeJoint:translate(0.0, -0.3, 0.0)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftKnee:scale(0.08,0.08,0.08)
leftKnee:set_material(orange)
leftKneeJoint:add_child(leftKnee)

leftLowLeg = gr.mesh('cylinder', 'leftLowLeg')
leftLowLeg:scale(0.08, 0.15, 0.08)
leftLowLeg:translate(0.0, -0.2, 0.0)
leftLowLeg:set_material(orange)
leftKneeJoint:add_child(leftLowLeg)

leftAnkleJoint = gr.joint('leftKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
leftKneeJoint:add_child(leftAnkleJoint)
leftAnkleJoint:translate(0.0, -0.3, 0.0)

leftAnkle = gr.mesh('sphere', 'leftAnkle')
leftAnkle:scale(0.08,0.08,0.08)
leftAnkle:set_material(orange)
leftAnkleJoint:add_child(leftAnkle)

leftFoot = gr.mesh('sphere', 'leftFoot')
leftFoot:scale(0.15,0.08,0.15)
leftFoot:set_material(orange)
leftFoot:translate(0.0,0.0,0.08)
leftAnkleJoint:add_child(leftFoot)

-- right part
rightArmJoint = gr.joint('rightArmJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
torso:add_child(rightArmJoint)
rightArmJoint:rotate('y', 180);
rightArmJoint:rotate('z', 30);
rightArmJoint:translate(0.28, 0.40, 0.0)

rightArm = gr.mesh('cylinder', 'rightArm')
rightArm:scale(0.08, 0.3, 0.08)
rightArm:translate(0.0, -0.2, 0.0)
rightArm:set_material(orange)
rightArmJoint:add_child(rightArm)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
rightShoulder:scale(0.1,0.1,0.1)
rightArm:set_material(orange)
rightArmJoint:add_child(rightShoulder)


rightWristJoint = gr.joint('rightWristJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
rightArmJoint:add_child(rightWristJoint)
rightWristJoint:translate(0.0, -0.55, 0.0)

rightWrist = gr.mesh('sphere', 'rightWrist')
rightWrist:scale(0.08,0.08,0.08)
rightWrist:set_material(orange)
rightWristJoint:add_child(rightWrist)

righthand = gr.mesh('sphere', 'hand')
righthand:scale(0.05,0.15,0.15)
righthand:translate(0.0,-0.1,0.0)
rightWristJoint:add_child(righthand)

rightHipJoint = gr.joint('rightHipJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
torso:add_child(rightHipJoint)
rightHipJoint:translate(0.2, -0.25, 0.0)

rightHip = gr.mesh('sphere', 'rightHip')
rightHip:scale(0.08,0.08,0.08)
rightHip:set_material(orange)
rightHipJoint:add_child(rightHip)

rightUpLeg = gr.mesh('cylinder', 'rightUpLeg')
rightUpLeg:scale(0.08, 0.15, 0.08)
rightUpLeg:translate(0.0, -0.2, 0.0)
rightUpLeg:set_material(orange)
rightHipJoint:add_child(rightUpLeg)

rightKneeJoint = gr.joint('rightKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
rightHipJoint:add_child(rightKneeJoint)
rightKneeJoint:translate(0.0, -0.3, 0.0)

rightKnee = gr.mesh('sphere', 'rightKnee')
rightKnee:scale(0.08,0.08,0.08)
rightKnee:set_material(orange)
rightKneeJoint:add_child(rightKnee)

rightLowLeg = gr.mesh('cylinder', 'rightLowLeg')
rightLowLeg:scale(0.08, 0.15, 0.08)
rightLowLeg:translate(0.0, -0.2, 0.0)
rightLowLeg:set_material(orange)
rightKneeJoint:add_child(rightLowLeg)

rightAnkleJoint = gr.joint('rightKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
rightKneeJoint:add_child(rightAnkleJoint)
rightAnkleJoint:translate(0.0, -0.3, 0.0)

rightAnkle = gr.mesh('sphere', 'rightAnkle')
rightAnkle:scale(0.08,0.08,0.08)
rightAnkle:set_material(orange)
rightAnkleJoint:add_child(rightAnkle)

rightFoot = gr.mesh('sphere', 'rightFoot')
rightFoot:scale(0.15,0.08,0.15)
rightFoot:set_material(orange)
rightFoot:translate(0.0,0.0,0.08)
rightAnkleJoint:add_child(rightFoot)
return rootnode
