#include "Common.h"
// Include the ReactPhysics3D header file 
#include "reactphysics3d.h"

// Use the ReactPhysics3D namespace 
using namespace reactphysics3d;


void TheCollisionWorld()
{
    // Create the collision world 
    rp3d::CollisionWorld world;

    // Initial position and orientation of the collision body 
    rp3d::Vector3 initPosition(0.0, 3.0, 0.0);
    rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
    rp3d::Transform transform(initPosition, initOrientation);

    // Create a collision body in the world 
    rp3d::CollisionBody* body;
    body = world.createCollisionBody(transform);

    // New position and orientation of the collision body 
    rp3d::Vector3 position(10.0, 3.0, 0.0);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform newTransform(position, orientation);

    // Move the collision body 
    body->setTransform(newTransform);

    // Here, world is an instance of the CollisionWorld class 
    // and body is a CollisionBody* pointer 

    // Destroy the collision body and remove it from the world 
    world.destroyCollisionBody(body);
}

void CollisionType()
{
    // Half extents of the box in the x, y and z directions 
    const rp3d::Vector3 halfExtents(2.0, 3.0, 5.0);

    // Create the box shape 
    const rp3d::BoxShape boxShape(halfExtents);

    // Create the box shape with a custom collision margin 
    const rp3d::BoxShape boxShape(halfExtents, 0.01);

    // Create the sphere shape with a radius of 2m 
    const rp3d::SphereShape sphereShape(2.0);

    // Create the cone shape 
    const rp3d::ConeShape coneShape(1.0, 3.0);

    // Create the cone shape with a custom collision margin 
    const rp3d::ConeShape coneShape(1.0, 3.0, 0.01);

    // Create the cylinder shape 
    const rp3d::CylinderShape cylinderShape(1.0, 3.0);

    // Create the cylinder shape with a custom collision margin 
    const rp3d::CylinderShape cylinderShape(1.0, 3.0, 0.01);

    // Create the capsule shape 
    const rp3d::CapsuleShape capsuleShape(1.0, 2.0);
}

void TheDynamicWorld()
{
    // Gravity vector 
    rp3d::Vector3 gravity(0.0, -9.81, 0.0);

    // Create the dynamics world 
    rp3d::DynamicsWorld world(gravity);

    // Change the number of iterations of the velocity solver 
    world.setNbIterationsVelocitySolver(15);

    // Change the number of iterations of the position solver 
    world.setNbIterationsPositionSolver(8);

    // Disable the sleeping technique 
    world.enableSleeping(false);

}

void UpdateDynamicWorld(rp3d::DynamicsWorld* world)
{
    const float timeStep = 1.0 / 60.0;

    long double deltaTime = 0;
    static long double accumulator;
    accumulator += deltaTime;

    while (accumulator >= timeStep) {
        world->update(timeStep);
        accumulator -= timeStep;
    }
}

void CreatingRigidBody(rp3d::DynamicsWorld* world)
{
    // Initial position and orientation of the rigid body 
    rp3d::Vector3 initPosition(0.0, 3.0, 0.0);
    rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
    rp3d::Transform transform(initPosition, initOrientation);

    // Create a rigid body in the world 
    rp3d::RigidBody* rigidBody;
    rigidBody = world->createRigidBody(transform);

    // Change the type of the body to Kinematic 
    rigidBody->setType(KINEMATIC);

    // Disable gravity for this body 
    rigidBody->enableGravity(false);

    // Get the current material of the body 
    rp3d::Material& material = rigidBody->getMaterial();

    // Change the bounciness of the body 
    material.setBounciness(rp3d::decimal(0.4));

    // Change the friction coefficient of the body 
    material.setFrictionCoefficient(rp3d::decimal(0.2));

    // This rigid body cannot sleep 
    rigidBody->setIsAllowedToSleep(false);

    // Force vector (in Newton) 
    rp3d::Vector3 force(2.0, 0.0, 0.0);

    // Apply a force to the center of the body 
    rigidBody->applyForceToCenterOfMass(force);

    // Force vector (in Newton) 
    rp3d::Vector3 force(2.0, 0.0, 0.0);

    // Point where the force is applied 
    rp3d::Vector3 point(4.0, 5.0, 6.0);

    // Apply a force to the body 
    rigidBody->applyForce(force, point);

    // Torque vector 
    rp3d::Vector3 torque(0.0, 3.0, 0.0);

    // Apply a torque to the body 
    rigidBody->applyTorque(torque);

    // Here, body is a RigidBody* pointer previously created 

    // Get the interpolated transform of the rigid body 
    rp3d::Transform transform = rigidBody->getTransform();

    // Destroy the rigid body 
    world->destroyRigidBody(rigidBody);
}