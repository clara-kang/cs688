-- cat.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:rotate('y', -45.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

pink = gr.material({1.0, 0.37, 0.37}, {0.8, 0.8, 0.8}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.8, 0.8, 0.8}, 10)
orange = gr.material({1.0, 0.6, 0.2}, {0.8, 0.48, 0.16}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.4, 0.4, 0.4}, 10)

torso = gr.node('torso')
rootnode:add_child(torso)

torsoMesh = gr.mesh('body', 'torso')
torso:add_child(torsoMesh)
torsoMesh:set_material(orange)
torsoMesh:scale(0.5,0.6,0.6)
torsoMesh:translate(0.0,0.3,0.0)

neckJointPP = gr.node('neckJointPP')
neckJointPP:rotate('x', 90)
neckJointPP:translate(0.0, 0.6, 0.0)
torso:add_child(neckJointPP)

neckJointP = gr.joint('neckJointP', {0.0, 0.0, 0.0}, {-40.0, 0.0, 40.0})
neckJointPP:add_child(neckJointP)

neckJointII = gr.node('neckJointII')
neckJointII:rotate('x', -90)
neckJointII:translate(0.0, 0.6, 0.0)
neckJointP:add_child(neckJointII)

neckJoint = gr.joint('neckJoint', {0.0, 0.0, 0.0}, {-70.0, 0.0, 70.0})
neckJointII:add_child(neckJoint)
neckJoint:translate(0.0, 0.3, -0.5)

head = gr.mesh('head', 'head')
neckJoint:add_child(head)
head:scale(0.4, 0.4, 0.4)
-- head:rotate('x', -90)
--head:translate(0.0, 0.0, -0.3)
head:set_material(orange)

leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.4,0.5,0.4)
leftEye:translate(-0.2, 0.1, 0.23)
leftEye:set_material(white)

leftEyeBall = gr.mesh('sphere', 'leftEye')
leftEye:add_child(leftEyeBall)
leftEyeBall:scale(0.15,0.15,0.15)
leftEyeBall:translate(0.0, 0.15, 1.0)
leftEyeBall:set_material(black)

rightEye = gr.mesh('sphere', 'rightEyeBall')
head:add_child(rightEye)
rightEye:scale(0.4,0.5,0.4)
rightEye:translate(0.2, 0.1, 0.23)
rightEye:set_material(white)

rightEyeBall = gr.mesh('sphere', 'rightEyeBall')
rightEye:add_child(rightEyeBall)
rightEyeBall:scale(0.15,0.15,0.15)
rightEyeBall:translate(0.0, 0.15, 1.0)
rightEyeBall:set_material(black)

nose = gr.mesh('sphere', 'nose')
nose:scale(0.1,0.1,0.1)
nose:translate(0.0,0.0,0.7)
nose:set_material(pink)
head:add_child(nose)

tooth = gr.mesh('cube', 'tooth')
tooth:scale(0.9,0.45,0.4)
tooth:rotate('x',12)
tooth:translate(0.0,-0.45,0.25)
tooth:set_material(white)
head:add_child(tooth)

mustache = gr.mesh('mustache', 'mustache')
head:add_child(mustache)
--mustache:scale(0.4, 0.4, 0.4)
--mustache:translate(0.0, 0.9, 0.0)
mustache:set_material(white)

leftEarJoint = gr.joint('leftEarJoint',{0.0,0.0,0.0},{-60.0,0.0,0.0})
leftEarJoint:translate(-0.4,0.5,0.0)
head:add_child(leftEarJoint)

leftEar = gr.mesh('ear', 'leftEar')
leftEarJoint:add_child(leftEar)
leftEar:rotate('z', 15.0)
leftEar:translate(0.4, -0.5, 0.4)
leftEar:set_material(orange)

rightEarJoint = gr.joint('rightEarJoint',{0.0,0.0,0.0},{0.0,0.0,60.0})
rightEarJoint:translate(0.4,0.5,0.0)
head:add_child(rightEarJoint)

rightEar = gr.mesh('ear', 'rightEar')
rightEarJoint:add_child(rightEar)
rightEar:rotate('z', -15.0)
rightEar:translate(0.2, -0.7, 0.4)
rightEar:set_material(orange)

leftArmJointP = gr.node('leftArmJointP')
leftArmJointP:rotate('z',90)
leftArmJointP:translate(-0.28, 0.40, 0.0)
torso:add_child(leftArmJointP)

leftArmJoint = gr.joint('leftArmJoint',{0.0,0.0,0.0},{-60.0,0.0,60.0})
leftArmJointP:add_child(leftArmJoint)
--leftArmJoint:rotate('z',60)

leftArm = gr.mesh('cylinder', 'leftArm')
leftArm:scale(0.08, 0.15, 0.08)
leftArm:rotate('z',60)
leftArm:translate(-0.1, 0.05, 0.0)
leftArm:set_material(orange)
leftArmJoint:add_child(leftArm)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
leftShoulder:scale(0.1,0.1,0.1)
leftShoulder:set_material(orange)
leftArmJoint:add_child(leftShoulder)

leftElbowJoint = gr.joint('leftElbowJoint',{0.0,0.0,0.0},{0.0,0.0,60.0})
leftArmJoint:add_child(leftElbowJoint)
leftElbowJoint:translate(-0.28,0.15, 0.0)

leftElbow = gr.mesh('sphere', 'leftElbow')
leftElbow:scale(0.08,0.08,0.08)
leftElbow:set_material(orange)
leftElbowJoint:add_child(leftElbow)

leftLowArm = gr.mesh('cylinder', 'leftLowArm')
leftLowArm:scale(1.0,1.8,1.0)
leftLowArm:rotate('z',60)
leftLowArm:translate(-1.5, 0.8, 0.0)
leftLowArm:set_material(orange)
leftElbow:add_child(leftLowArm)

leftWristJointP = gr.node('leftWristJointP')
leftWristJointP:rotate('x',90)
leftWristJointP:translate(-0.2, 0.1, 0.0)
leftElbowJoint:add_child(leftWristJointP)

leftWristJoint = gr.joint('leftWristJoint',{0.0,0.0,0.0},{-30.0,0.0,45.0})
leftWristJointP:add_child(leftWristJoint)

leftWrist = gr.mesh('sphere', 'leftWrist')
leftWrist:scale(0.08,0.08,0.08)
leftWrist:set_material(orange)
leftWristJoint:add_child(leftWrist)

lefthand = gr.mesh('sphere', 'hand')
lefthand:scale(0.15,0.15,0.08)
lefthand:rotate('y', -30)
lefthand:translate(-0.15,0.0,-0.07)
lefthand:set_material(orange)
leftWristJoint:add_child(lefthand)

leftHipJointP = gr.node('leftHipJointP')
leftHipJointP:rotate('z', 90)
leftHipJointP:translate(-0.2, -0.25, 0.0)
torso:add_child(leftHipJointP)

leftHipJoint = gr.joint('leftHipJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
leftHipJointP:add_child(leftHipJoint)

leftHip = gr.mesh('sphere', 'leftHip')
leftHip:scale(0.08,0.08,0.08)
leftHip:set_material(orange)
leftHipJoint:add_child(leftHip)

leftUpLeg = gr.mesh('cylinder', 'leftUpLeg')
leftUpLeg:scale(1.0, 1.8, 1.0)
leftUpLeg:rotate('z',90)
leftUpLeg:translate(-1.0, 0.0, 0.0)
leftUpLeg:set_material(orange)
leftHip:add_child(leftUpLeg)

leftKneeJoint = gr.joint('leftKneeJoint',{0.0,0.0,0.0},{-60.0,0.0,0.0})
leftHipJoint:add_child(leftKneeJoint)
leftKneeJoint:translate(-0.3, 0.0, 0.0)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftKnee:scale(0.08,0.08,0.08)
leftKnee:set_material(orange)
leftKneeJoint:add_child(leftKnee)

leftLowLeg = gr.mesh('cylinder', 'leftLowLeg')
leftLowLeg:scale(1.0,2.0,1.0)
leftLowLeg:rotate('z',90)
leftLowLeg:translate(-1.5, 0.0, 0.0)
leftLowLeg:set_material(orange)
leftKnee:add_child(leftLowLeg)

leftAnkleJoint = gr.joint('leftKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
leftKneeJoint:add_child(leftAnkleJoint)
leftAnkleJoint:translate(-0.3, 0.0, 0.0)

leftAnkle = gr.mesh('sphere', 'leftAnkle')
leftAnkle:scale(0.08,0.08,0.08)
leftAnkle:set_material(orange)
leftAnkleJoint:add_child(leftAnkle)

leftFoot = gr.mesh('sphere', 'leftFoot')
leftFoot:scale(1.0,1.8,1.8)
leftFoot:set_material(orange)
leftFoot:translate(0.0,0.0,1.0)
leftAnkle:add_child(leftFoot)

-- right part

rightArmJointP = gr.node('rightArmJointP')
rightArmJointP:rotate('z',90)
rightArmJointP:translate(0.28, 0.40, 0.0)
torso:add_child(rightArmJointP)

rightArmJoint = gr.joint('rightArmJoint',{0.0,0.0,0.0},{-60.0,0.0,60.0})
rightArmJointP:add_child(rightArmJoint)

rightArm = gr.mesh('cylinder', 'rightArm')
rightArm:scale(0.08, 0.15, 0.08)
rightArm:rotate('z',-60)
rightArm:translate(-0.1, -0.05, 0.0)
rightArm:set_material(orange)
rightArmJoint:add_child(rightArm)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
rightShoulder:scale(0.1,0.1,0.1)
rightShoulder:set_material(orange)
rightArmJoint:add_child(rightShoulder)

rightElbowJoint = gr.joint('rightElbowJoint',{0.0,0.0,0.0},{0.0,0.0,60.0})
rightArmJoint:add_child(rightElbowJoint)
rightElbowJoint:translate(-0.28,-0.15, 0.0)

rightElbow = gr.mesh('sphere', 'rightElbow')
rightElbow:scale(0.08,0.08,0.08)
rightElbow:set_material(orange)
rightElbowJoint:add_child(rightElbow)

rightLowArm = gr.mesh('cylinder', 'rightLowArm')
rightLowArm:scale(1.0,1.8,1.0)
rightLowArm:rotate('z',-60)
rightLowArm:translate(-1.5, -0.8, 0.0)
rightLowArm:set_material(orange)
rightElbow:add_child(rightLowArm)

rightWristJointP = gr.node('rightWristJointP')
rightWristJointP:rotate('x',-90)
rightWristJointP:translate(-0.2, -0.1, 0.0)
rightElbowJoint:add_child(rightWristJointP)

rightWristJoint = gr.joint('rightWristJoint',{0.0,0.0,0.0},{-30.0,0.0,45.0})
rightWristJointP:add_child(rightWristJoint)

rightWrist = gr.mesh('sphere', 'rightWrist')
rightWrist:scale(0.08,0.08,0.08)
rightWrist:set_material(orange)
rightWristJoint:add_child(rightWrist)

righthand = gr.mesh('sphere', 'hand')
righthand:scale(0.15,0.15,0.08)
righthand:rotate('y', -30)
righthand:translate(-0.15,0.0,-0.07)
righthand:set_material(orange)
rightWristJoint:add_child(righthand)

rightHipJointP = gr.node('rightHipJointP')
rightHipJointP:rotate('z', 90)
rightHipJointP:translate(0.2, -0.25, 0.0)
torso:add_child(rightHipJointP)

rightHipJoint = gr.joint('rightHipJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
rightHipJointP:add_child(rightHipJoint)

rightHip = gr.mesh('sphere', 'rightHip')
rightHip:scale(0.08,0.08,0.08)
rightHip:set_material(orange)
rightHipJoint:add_child(rightHip)

rightUpLeg = gr.mesh('cylinder', 'rightUpLeg')
rightUpLeg:scale(1.0, 1.8, 1.0)
rightUpLeg:rotate('z',90)
rightUpLeg:translate(-1.0, 0.0, 0.0)
rightUpLeg:set_material(orange)
rightHip:add_child(rightUpLeg)

rightKneeJoint = gr.joint('rightKneeJoint',{0.0,0.0,0.0},{-60.0,0.0,0.0})
rightHipJoint:add_child(rightKneeJoint)
rightKneeJoint:translate(-0.3, 0.0, 0.0)

rightKnee = gr.mesh('sphere', 'rightKnee')
rightKnee:scale(0.08,0.08,0.08)
rightKnee:set_material(orange)
rightKneeJoint:add_child(rightKnee)

rightLowLeg = gr.mesh('cylinder', 'rightLowLeg')
rightLowLeg:scale(1.0,2.0,1.0)
rightLowLeg:rotate('z',90)
rightLowLeg:translate(-1.5, 0.0, 0.0)
rightLowLeg:set_material(orange)
rightKnee:add_child(rightLowLeg)

rightAnkleJoint = gr.joint('rightKneeJoint',{0.0,0.0,0.0},{-45.0,0.0,45.0})
rightKneeJoint:add_child(rightAnkleJoint)
rightAnkleJoint:translate(-0.3, 0.0, 0.0)

rightAnkle = gr.mesh('sphere', 'rightAnkle')
rightAnkle:scale(0.08,0.08,0.08)
rightAnkle:set_material(orange)
rightAnkleJoint:add_child(rightAnkle)

rightFoot = gr.mesh('sphere', 'rightFoot')
rightFoot:scale(1.0,1.8,1.8)
rightFoot:set_material(orange)
rightFoot:translate(0.0,0.0,1.0)
rightAnkle:add_child(rightFoot)
return rootnode
