// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include "physics/PhysicsEngine.hpp"
#include <iostream>

namespace BVA {

PhysicsEngine::PhysicsEngine() {}

PhysicsEngine::~PhysicsEngine() {
    shutdown();
}

bool PhysicsEngine::initialize() {
    // Create collision configuration
    collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();

    // Create dispatcher
    dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());

    // Create broadphase
    broadphase = std::make_unique<btDbvtBroadphase>();

    // Create solver
    solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    // Create dynamics world
    dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
        dispatcher.get(),
        broadphase.get(),
        solver.get(),
        collisionConfiguration.get()
    );

    // Set gravity
    dynamicsWorld->setGravity(btVector3(0, -20.0f, 0));

    std::cout << "Physics engine initialized with Bullet Physics" << std::endl;
    return true;
}

void PhysicsEngine::shutdown() {
    // Clean up physics bodies
    bodies.clear();
    collisionShapes.clear();

    dynamicsWorld.reset();
    solver.reset();
    broadphase.reset();
    dispatcher.reset();
    collisionConfiguration.reset();
}

void PhysicsEngine::update(float dt) {
    if (dynamicsWorld) {
        // Step simulation with fixed timestep
        dynamicsWorld->stepSimulation(dt, 10, 1.0f / 60.0f);
    }
}

PhysicsBody* PhysicsEngine::createRigidBody(float mass, const btTransform& startTransform,
                                             btCollisionShape* shape) {
    btVector3 localInertia(0, 0, 0);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);

    btRigidBody* rigidBody = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(rigidBody);

    auto body = std::make_unique<PhysicsBody>(rigidBody);
    PhysicsBody* bodyPtr = body.get();
    bodies.push_back(std::move(body));

    return bodyPtr;
}

CharacterController* PhysicsEngine::createCharacterController(float height, float radius) {
    // TODO: Implement character controller using btKinematicCharacterController
    return nullptr;
}

btBoxShape* PhysicsEngine::createBoxShape(const btVector3& halfExtents) {
    auto shape = std::make_unique<btBoxShape>(halfExtents);
    btBoxShape* shapePtr = shape.get();
    collisionShapes.push_back(std::move(shape));
    return shapePtr;
}

btSphereShape* PhysicsEngine::createSphereShape(float radius) {
    auto shape = std::make_unique<btSphereShape>(radius);
    btSphereShape* shapePtr = shape.get();
    collisionShapes.push_back(std::move(shape));
    return shapePtr;
}

btCapsuleShape* PhysicsEngine::createCapsuleShape(float radius, float height) {
    auto shape = std::make_unique<btCapsuleShape>(radius, height);
    btCapsuleShape* shapePtr = shape.get();
    collisionShapes.push_back(std::move(shape));
    return shapePtr;
}

btStaticPlaneShape* PhysicsEngine::createPlaneShape(const btVector3& normal, float planeConstant) {
    auto shape = std::make_unique<btStaticPlaneShape>(normal, planeConstant);
    btStaticPlaneShape* shapePtr = shape.get();
    collisionShapes.push_back(std::move(shape));
    return shapePtr;
}

RaycastResult PhysicsEngine::raycast(const btVector3& from, const btVector3& to) {
    btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
    dynamicsWorld->rayTest(from, to, rayCallback);

    RaycastResult result;
    result.hit = rayCallback.hasHit();
    if (result.hit) {
        result.hitPoint = rayCallback.m_hitPointWorld;
        result.hitNormal = rayCallback.m_hitNormalWorld;
        // Get physics body from collision object
        if (rayCallback.m_collisionObject) {
            result.body = static_cast<PhysicsBody*>(rayCallback.m_collisionObject->getUserPointer());
        }
    }
    return result;
}

std::vector<PhysicsBody*> PhysicsEngine::sphereOverlap(const btVector3& center, float radius) {
    std::vector<PhysicsBody*> results;

    btSphereShape sphereShape(radius);
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(center);

    // TODO: Implement sphere overlap query
    // This would use btCollisionWorld::contactTest or similar

    return results;
}

void PhysicsEngine::setGravity(const btVector3& gravity) {
    if (dynamicsWorld) {
        dynamicsWorld->setGravity(gravity);
    }
}

btVector3 PhysicsEngine::getGravity() const {
    if (dynamicsWorld) {
        return dynamicsWorld->getGravity();
    }
    return btVector3(0, 0, 0);
}

// PhysicsBody implementation
PhysicsBody::~PhysicsBody() {
    if (rigidBody) {
        if (rigidBody->getMotionState()) {
            delete rigidBody->getMotionState();
        }
        // Note: The dynamics world handles removal
    }
}

void PhysicsBody::setPosition(const btVector3& pos) {
    btTransform transform = rigidBody->getWorldTransform();
    transform.setOrigin(pos);
    rigidBody->setWorldTransform(transform);
    rigidBody->activate();
}

btVector3 PhysicsBody::getPosition() const {
    return rigidBody->getWorldTransform().getOrigin();
}

void PhysicsBody::setRotation(const btQuaternion& rot) {
    btTransform transform = rigidBody->getWorldTransform();
    transform.setRotation(rot);
    rigidBody->setWorldTransform(transform);
    rigidBody->activate();
}

btQuaternion PhysicsBody::getRotation() const {
    return rigidBody->getWorldTransform().getRotation();
}

void PhysicsBody::applyForce(const btVector3& force) {
    rigidBody->applyCentralForce(force);
    rigidBody->activate();
}

void PhysicsBody::applyImpulse(const btVector3& impulse) {
    rigidBody->applyCentralImpulse(impulse);
    rigidBody->activate();
}

void PhysicsBody::setVelocity(const btVector3& velocity) {
    rigidBody->setLinearVelocity(velocity);
    rigidBody->activate();
}

btVector3 PhysicsBody::getVelocity() const {
    return rigidBody->getLinearVelocity();
}

} // namespace BVA
