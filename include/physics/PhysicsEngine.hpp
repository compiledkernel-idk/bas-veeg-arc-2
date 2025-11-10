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


#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>
#include <functional>

namespace BVA {

class PhysicsBody;
class CharacterController;

struct RaycastResult {
    bool hit = false;
    btVector3 hitPoint;
    btVector3 hitNormal;
    PhysicsBody* body = nullptr;
};

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    bool initialize();
    void shutdown();
    void update(float dt);

    // Physics body creation
    PhysicsBody* createRigidBody(float mass, const btTransform& startTransform,
                                  btCollisionShape* shape);
    CharacterController* createCharacterController(float height, float radius);

    // Collision shapes
    btBoxShape* createBoxShape(const btVector3& halfExtents);
    btSphereShape* createSphereShape(float radius);
    btCapsuleShape* createCapsuleShape(float radius, float height);
    btStaticPlaneShape* createPlaneShape(const btVector3& normal, float planeConstant);

    // Raycasting
    RaycastResult raycast(const btVector3& from, const btVector3& to);
    std::vector<PhysicsBody*> sphereOverlap(const btVector3& center, float radius);

    // Physics world settings
    void setGravity(const btVector3& gravity);
    btVector3 getGravity() const;

    btDynamicsWorld* getWorld() { return dynamicsWorld.get(); }

private:
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btBroadphaseInterface> broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

    std::vector<std::unique_ptr<btCollisionShape>> collisionShapes;
    std::vector<std::unique_ptr<PhysicsBody>> bodies;
};

class PhysicsBody {
public:
    PhysicsBody(btRigidBody* body) : rigidBody(body) {}
    ~PhysicsBody();

    btRigidBody* getRigidBody() { return rigidBody.get(); }
    const btRigidBody* getRigidBody() const { return rigidBody.get(); }

    void setPosition(const btVector3& pos);
    btVector3 getPosition() const;
    void setRotation(const btQuaternion& rot);
    btQuaternion getRotation() const;

    void applyForce(const btVector3& force);
    void applyImpulse(const btVector3& impulse);
    void setVelocity(const btVector3& velocity);
    btVector3 getVelocity() const;

    void setUserData(void* data) { userData = data; }
    void* getUserData() const { return userData; }

private:
    std::unique_ptr<btRigidBody> rigidBody;
    void* userData = nullptr;
};

} // namespace BVA
