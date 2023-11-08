//
// (c) 2023 Eduardo Doria.
//

#include "PhysicsSystem.h"
#include "Scene.h"
#include "util/Angle.h"

#include "util/Box2DAux.h"
#include "util/JoltPhysicsAux.h"


using namespace Supernova;


PhysicsSystem::PhysicsSystem(Scene* scene): SubSystem(scene){
	signature.set(scene->getComponentType<Body2DComponent>());

	this->scene = scene;

    this->pointsToMeterScale = 64.0;

    b2Vec2 gravity(0.0f, 10.0f);
    world2D = new b2World(gravity);
  
    contactListener2D = new Box2DContactListener(scene, this);
    contactFilter2D = new Box2DContactFilter(scene, this);

    world2D->SetContactListener(contactListener2D);
    world2D->SetContactFilter(contactFilter2D);

    // https://github.com/jrouwe/JoltPhysics/issues/244
    JPH::RegisterDefaultAllocator();

    // Install callbacks
    //JPH::Trace = TraceImpl;
    //JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    // Create a factory
    JPH::Factory::sInstance = new JPH::Factory();

    // Register all Jolt physics types
    JPH::RegisterTypes();

    const unsigned int cMaxBodies = 1024;
    const unsigned int cNumBodyMutexes = 0;
    const unsigned int cMaxBodyPairs = 1024;
    const unsigned int cMaxContactConstraints = 1024;

    broad_phase_layer_interface = new BPLayerInterfaceImpl();
    object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
    object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

    // Now we can create the actual physics system.
    world3D = new JPH::PhysicsSystem();
    world3D->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broad_phase_layer_interface, *object_vs_broadphase_layer_filter, *object_vs_object_layer_filter);
    world3D->SetGravity(JPH::Vec3(0, -9.81f, 0));

    temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    job_system = new JPH::JobSystemThreadPool (JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);

    activationListener3D = new JoltActivationListener(scene, this);
    world3D->SetBodyActivationListener(activationListener3D);

    contactListener3D = new JoltContactListener(scene, this);
    world3D->SetContactListener(contactListener3D);
}

PhysicsSystem::~PhysicsSystem(){
    delete world2D;

    delete contactListener2D;
    delete contactFilter2D;

    delete activationListener3D;
    delete contactListener3D;

    delete world3D;
    delete temp_allocator;
    delete job_system;
    delete broad_phase_layer_interface;
    delete object_vs_broadphase_layer_filter;
    delete object_vs_object_layer_filter;

    //delete JPH::Factory::sInstance;
    //JPH::UnregisterTypes();
}

float PhysicsSystem::getPointsToMeterScale() const{
    return pointsToMeterScale;
}

void PhysicsSystem::setPointsToMeterScale(float pointsToMeterScale){
    this->pointsToMeterScale = pointsToMeterScale;
}

void PhysicsSystem::updateBody2DPosition(Signature signature, Entity entity, Body2DComponent& body, bool updateAnyway){
    if (signature.test(scene->getComponentType<Transform>())){
        Transform& transform = scene->getComponent<Transform>(entity);

        if (transform.needUpdate || updateAnyway){
            b2Vec2 bPosition(transform.position.x / pointsToMeterScale, transform.position.y / pointsToMeterScale);
            body.body->SetTransform(bPosition, Angle::defaultToRad(transform.rotation.getRoll()));
        }
    }
}

void PhysicsSystem::updateBody3DPosition(Signature signature, Entity entity, Body3DComponent& body, bool updateAnyway){
    if (signature.test(scene->getComponentType<Transform>())){
        Transform& transform = scene->getComponent<Transform>(entity);

        if (transform.needUpdate || updateAnyway){
            JPH::Vec3 jPosition(transform.position.x, transform.position.y, transform.position.z);
            JPH::Quat jQuat(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);

            body.body->SetPositionAndRotationInternal(jPosition, jQuat);
        }
    }
}

void PhysicsSystem::createGenericJoltBody(Entity entity, Body3DComponent& body, const JPH::Shape* shape){
    JPH::ObjectLayer layer = Layers::NON_MOVING;
    JPH::EMotionType joltType = JPH::EMotionType::Static;
    JPH::EActivation activation = JPH::EActivation::DontActivate;
    if (body.type == BodyType::DYNAMIC){
        layer = Layers::MOVING;
        joltType = JPH::EMotionType::Dynamic;
        activation = JPH::EActivation::Activate;
    }else if (body.type == BodyType::KINEMATIC){
        layer = Layers::MOVING;
        joltType = JPH::EMotionType::Kinematic;
        activation = JPH::EActivation::Activate;
    }

    JPH::BodyCreationSettings settings(shape, JPH::Vec3(0.0, 0.0, 0.0), JPH::Quat::sIdentity(), joltType, layer);

    JPH::BodyInterface &body_interface = world3D->GetBodyInterface();

    body.body = body_interface.CreateBody(settings);
    body.body->SetUserData(entity);
    //if (type != BodyType::STATIC){
    //    body.body->SetAllowSleeping(false);
    //}
    body.newBody = true;

    body_interface.AddBody(body.body->GetID(), activation);
}

void PhysicsSystem::createBody2D(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (!signature.test(scene->getComponentType<Body2DComponent>())){
        scene->addComponent<Body2DComponent>(entity, {});
        loadBody2D(entity);
    }
}

void PhysicsSystem::removeBody2D(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (signature.test(scene->getComponentType<Body2DComponent>())){
        destroyBody2D(scene->getComponent<Body2DComponent>(entity));
        scene->removeComponent<Body2DComponent>(entity);
    }
}

int PhysicsSystem::createRectShape2D(Entity entity, float width, float height){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2PolygonShape shape;
            // same as shape.SetAsBox but using center on left corner
            shape.m_count = 4;
            shape.m_vertices[0].Set(0, 0);
            shape.m_vertices[1].Set( width / pointsToMeterScale, 0);
            shape.m_vertices[2].Set( width / pointsToMeterScale,  height / pointsToMeterScale);
            shape.m_vertices[3].Set(0,  height / pointsToMeterScale);
            shape.m_normals[0].Set(0.0f, -1.0f);
            shape.m_normals[1].Set(1.0f, 0.0f);
            shape.m_normals[2].Set(0.0f, 1.0f);
            shape.m_normals[3].Set(-1.0f, 0.0f);
            shape.m_centroid.SetZero();

            loadShape2D(*body, &shape, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createCenteredRectShape2D(Entity entity, float width, float height, Vector2 center, float angle){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2PolygonShape shape;
            float halfW = width / 2.0 / pointsToMeterScale;
            float halfH = height / 2.0 / pointsToMeterScale;
            shape.SetAsBox(halfW, halfH, b2Vec2(center.x / pointsToMeterScale, center.y / pointsToMeterScale), Angle::defaultToRad(angle));

            loadShape2D(*body, &shape, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createPolygonShape2D(Entity entity, std::vector<Vector2> vertices){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2PolygonShape polygon;
            std::vector<b2Vec2> b2vertices(vertices.size());
            for (int i = 0; i < vertices.size(); i++){
                b2vertices[i].Set(vertices[i].x / pointsToMeterScale, vertices[i].y / pointsToMeterScale);
            }
            polygon.Set(&b2vertices[0], (int)vertices.size());

            loadShape2D(*body, &polygon, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createCircleShape2D(Entity entity, Vector2 center, float radius){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2CircleShape circle;
            circle.m_p.Set(center.x / pointsToMeterScale, center.y / pointsToMeterScale);
            circle.m_radius = radius / pointsToMeterScale;

            loadShape2D(*body, &circle, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createTwoSidedEdgeShape2D(Entity entity, Vector2 vertice1, Vector2 vertice2){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2EdgeShape edge;
            b2Vec2 v1(vertice1.x / pointsToMeterScale, vertice1.y / pointsToMeterScale);
            b2Vec2 v2(vertice2.x / pointsToMeterScale, vertice2.y / pointsToMeterScale);
            edge.SetTwoSided(v1, v2);

            loadShape2D(*body, &edge, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createOneSidedEdgeShape2D(Entity entity, Vector2 vertice0, Vector2 vertice1, Vector2 vertice2, Vector2 vertice3){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2EdgeShape edge;
            b2Vec2 v0(vertice0.x / pointsToMeterScale, vertice0.y / pointsToMeterScale);
            b2Vec2 v1(vertice1.x / pointsToMeterScale, vertice1.y / pointsToMeterScale);
            b2Vec2 v2(vertice2.x / pointsToMeterScale, vertice2.y / pointsToMeterScale);
            b2Vec2 v3(vertice3.x / pointsToMeterScale, vertice3.y / pointsToMeterScale);
            edge.SetOneSided(v0, v1, v2, v3);

            loadShape2D(*body, &edge, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createLoopChainShape2D(Entity entity, std::vector<Vector2> vertices){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2ChainShape chain;
            std::vector<b2Vec2> b2vertices(vertices.size());
            for (int i = 0; i < vertices.size(); i++){
                b2vertices[i].Set(vertices[i].x / pointsToMeterScale, vertices[i].y / pointsToMeterScale);
            }
            chain.CreateLoop(&b2vertices[0], (int)vertices.size());

            loadShape2D(*body, &chain, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

int PhysicsSystem::createChainShape2D(Entity entity, std::vector<Vector2> vertices, Vector2 prevVertex, Vector2 nextVertex){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        if (body->numShapes < MAX_SHAPES){

            body->shapes[body->numShapes].type = CollisionShape2DType::POLYGON;

            b2ChainShape chain;
            std::vector<b2Vec2> b2vertices(vertices.size());
            for (int i = 0; i < vertices.size(); i++){
                b2vertices[i].Set(vertices[i].x / pointsToMeterScale, vertices[i].y / pointsToMeterScale);
            }
            b2Vec2 pv(prevVertex.x / pointsToMeterScale, prevVertex.y / pointsToMeterScale);
            b2Vec2 nv(nextVertex.x / pointsToMeterScale, nextVertex.y / pointsToMeterScale);
            chain.CreateChain(&b2vertices[0], (int)vertices.size(), pv, nv);

            loadShape2D(*body, &chain, body->numShapes);

            body->numShapes++;

            return (body->numShapes - 1);
        }else{
            Log::error("Cannot add more shapes in this body, please increase value MAX_SHAPES");
        }
    }

    return -1;
}

void PhysicsSystem::removeAllShapes2D(Entity entity){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        for (int i = 0; i < body->numShapes; i++){
            destroyShape2D(*body, i);
        }
        body->numShapes = 0;
    }
}

void PhysicsSystem::createBody3D(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (!signature.test(scene->getComponentType<Body3DComponent>())){
        scene->addComponent<Body3DComponent>(entity, {});
        //loadBody2D(entity);
    }
}

void PhysicsSystem::removeBody3D(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (signature.test(scene->getComponentType<Body3DComponent>())){
        //destroyBody2D(scene->getComponent<Body3DComponent>(entity));
        scene->removeComponent<Body3DComponent>(entity);
    }
}

void PhysicsSystem::createBoxShape3D(Entity entity, float width, float height, float depth){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){
        JPH::BoxShapeSettings shape_settings(JPH::Vec3(width/2.0, height/2.0, depth/2.0));

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createSphereShape3D(Entity entity, float radius){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){
        JPH::SphereShapeSettings shape_settings(radius);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createCapsuleShape3D(Entity entity, float halfHeight, float radius){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){
        JPH::CapsuleShapeSettings shape_settings(halfHeight, radius);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createTaperedCapsuleShape3D(Entity entity, float halfHeight, float topRadius, float bottomRadius){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){
        JPH::TaperedCapsuleShapeSettings shape_settings(halfHeight, topRadius, bottomRadius);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createCylinderShape3D(Entity entity, float halfHeight, float radius){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){
        JPH::CylinderShapeSettings shape_settings(halfHeight, radius);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createConvexHullShape3D(Entity entity, std::vector<Vector3> vertices){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){

        JPH::Array<JPH::Vec3> jvertices;
		jvertices.resize(vertices.size());
        for (int i = 0; i < vertices.size(); i++){
            jvertices[i] = JPH::Vec3(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        JPH::ConvexHullShapeSettings shape_settings(jvertices);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createMeshShape3D(Entity entity, std::vector<Vector3> vertices, std::vector<uint16_t> indices){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){

        JPH::VertexList jvertices;
		jvertices.resize(vertices.size());
        for (int i = 0; i < vertices.size(); i++){
            jvertices[i] = JPH::Float3(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        JPH::IndexedTriangleList jindices;
        int indicesize = int(indices.size() / 3);
		jindices.resize(indicesize);
        for (int i = 0; i < indicesize; i++){
            for (int j = 0; j < 3; j++){
                jindices[i].mIdx[j] = indices[(3*i)+j];
            }
        }

        JPH::MeshShapeSettings shape_settings(jvertices, jindices);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }
    }
}

void PhysicsSystem::createMeshShape3D(Entity entity, MeshComponent& mesh){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    if (body){

        std::map<std::string, Buffer*> buffers;

        if (mesh.buffer.getSize() > 0){
            buffers["vertices"] = &mesh.buffer;
        }
        if (mesh.indices.getSize() > 0){
            buffers["indices"] = &mesh.indices;
        }
        for (int i = 0; i < mesh.numExternalBuffers; i++){
            buffers[mesh.eBuffers[i].getName()] = &mesh.eBuffers[i];
        }

        JPH::IndexedTriangleList jindices;
        JPH::VertexList jvertices;

        for (auto const& buf : buffers){
            if (buf.second->isRenderAttributes()) {
                if (buf.second->getType() == BufferType::INDEX_BUFFER){
                    int indicesize = int(buf.second->getCount() / 3);
                    Attribute* attIndex = buf.second->getAttribute(AttributeType::INDEX);
                    if (attIndex){
                        jindices.resize(indicesize);
                        for (int i = 0; i < indicesize; i++){
                            for (int j = 0; j < 3; j++){
                                uint16_t indice = buf.second->getUInt16(attIndex, (3*i)+j);
                                jindices[i].mIdx[j] = indice;
                            }
                        }
                    }
                }else{
                    int verticesize = int(buf.second->getCount());
                    Attribute* attVertex = buf.second->getAttribute(AttributeType::POSITION);
                    if (attVertex){
                        jvertices.resize(verticesize);
                        for (int i = 0; i < verticesize; i++){
                            Vector3 vertice = buf.second->getVector3(attVertex, i);
                            jvertices[i] = JPH::Float3(vertice.x, vertice.y, vertice.z);
                        }
                    }
                }
            }
        }

        if (jindices.size() == 0){
            Log::error("Cannot create mesh shape without indices for 3D Body: %u", entity);
        }else if (jvertices.size() == 0){
            Log::error("Cannot create mesh shape without vertices for 3D Body: %u", entity);
        }else{
            JPH::MeshShapeSettings shape_settings(jvertices, jindices);

            JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
            if (shape_result.IsValid()){
                JPH::ShapeRefC shape = shape_result.Get();

                createGenericJoltBody(entity, *body, shape.GetPtr());
            }else{
                Log::error("Cannot create shape for 3D Body: %u", entity);
            }
        }
    }
}

void PhysicsSystem::createHeightFieldShape3D(Entity entity, TerrainComponent& terrain, unsigned int samplesSize){
    Body3DComponent* body = scene->findComponent<Body3DComponent>(entity);

    float logsamples = log2(samplesSize);
    if (ceil(logsamples) != floor(logsamples)) {
        Log::error("Cannot create terrain shape. Must insert a power of two samplesSize, not: %u", samplesSize);
        return;
    }

    if (body){
        TextureData& textureData = terrain.heightMap.getData();

        if (samplesSize == 0){
            samplesSize = textureData.getNearestPowerOfTwo();
            if (samplesSize > std::min(textureData.getOriginalWidth(), textureData.getOriginalHeight())){
                samplesSize = samplesSize / 2;
            }
        }

        JPH::Vec3 terrainOffset = JPH::Vec3(-terrain.terrainSize/2.0, 0.0f ,-terrain.terrainSize/2.0);
        JPH::Vec3 terrainScale = JPH::Vec3(terrain.terrainSize/samplesSize, terrain.maxHeight, terrain.terrainSize/samplesSize);

		float *samples = new float [samplesSize * samplesSize];

        for (int x = 0; x < samplesSize; x++){
            for (int y = 0; y < samplesSize; y++){
                TextureData& textureData = terrain.heightMap.getData();

                int posX = floor(textureData.getWidth() * x / samplesSize);
                int posY = floor(textureData.getHeight() * y / samplesSize);

                float val = textureData.getColorComponent(posX, posY, 0) / 255.0f;

                samples[x + (y * samplesSize)] = val;
            }
        }

        JPH::HeightFieldShapeSettings shape_settings(samples, terrainOffset, terrainScale, samplesSize);

        JPH::ShapeSettings::ShapeResult shape_result = shape_settings.Create();
        if (shape_result.IsValid()){
            JPH::ShapeRefC shape = shape_result.Get();

            createGenericJoltBody(entity, *body, shape.GetPtr());
        }else{
            Log::error("Cannot create shape for 3D Body: %u", entity);
        }

        delete samples;
    }
}

b2World* PhysicsSystem::getWorld2D() const{
     return world2D;
}

JPH::PhysicsSystem* PhysicsSystem::getWorld3D() const{
    return world3D;
}

b2Body* PhysicsSystem::getBody(Entity entity){
    Body2DComponent* body = scene->findComponent<Body2DComponent>(entity);

    if (body){
        return body->body;
    }

    return NULL;
}

bool PhysicsSystem::loadBody2D(Entity entity){
    Body2DComponent& body = scene->getComponent<Body2DComponent>(entity);

    if (world2D && !body.body){
        b2BodyDef bodyDef;
        bodyDef.userData.pointer = entity;

        body.body = world2D->CreateBody(&bodyDef);
        body.newBody = true;

        return true;
    }

    return false;
}

void PhysicsSystem::destroyBody2D(Body2DComponent& body){
    // When a world leaves scope or is deleted by calling delete on a pointer
    // all the memory reserved for bodies, fixtures, and joints is freed
    if (world2D && body.body){
        for (int i = 0; i < body.numShapes; i++){
            destroyShape2D(body, i);
        }
        body.numShapes = 0;

        world2D->DestroyBody(body.body);

        body.body = NULL;
    }
}

bool PhysicsSystem::loadShape2D(Body2DComponent& body, b2Shape* shape, size_t index){
    if (world2D && !body.shapes[index].fixture){
        b2FixtureDef fixtureDef;
        fixtureDef.shape = shape;
        fixtureDef.userData.pointer = index;

        body.shapes[index].fixture = body.body->CreateFixture(&fixtureDef);

        return true;
    }

    return false;
}

void PhysicsSystem::destroyShape2D(Body2DComponent& body, size_t index){
    if (world2D && body.shapes[index].fixture){
        body.body->DestroyFixture(body.shapes[index].fixture);

        body.shapes[index].fixture = NULL;
    }
}

bool PhysicsSystem::loadDistanceJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchorA, Vector2 anchorB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchorA(anchorA.x / pointsToMeterScale, anchorA.y / pointsToMeterScale);
        b2Vec2 myAnchorB(anchorB.x / pointsToMeterScale, anchorB.y / pointsToMeterScale);

        b2DistanceJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchorA, myAnchorB);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::DISTANCE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadRevoluteJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchor(anchor.x / pointsToMeterScale, anchor.y / pointsToMeterScale);

        b2RevoluteJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchor);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::REVOLUTE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPrismaticJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor, Vector2 axis){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchor(anchor.x / pointsToMeterScale, anchor.y / pointsToMeterScale);
        b2Vec2 myAxis(axis.x, axis.y);

        b2PrismaticJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchor, myAxis);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::PRISMATIC;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPulleyJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 groundAnchorA, Vector2 groundAnchorB, Vector2 anchorA, Vector2 anchorB, Vector2 worldAxis, float ratio){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchorA(anchorA.x / pointsToMeterScale, anchorA.y / pointsToMeterScale);
        b2Vec2 myAnchorB(anchorB.x / pointsToMeterScale, anchorB.y / pointsToMeterScale);
        b2Vec2 myGroundA(groundAnchorA.x / pointsToMeterScale, groundAnchorA.y / pointsToMeterScale);
        b2Vec2 myGroundB(groundAnchorB.x / pointsToMeterScale, groundAnchorB.y / pointsToMeterScale);

        b2PulleyJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myGroundA, myGroundB, myAnchorA, myAnchorB, ratio);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::PULLEY;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadGearJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Entity revoluteJoint, Entity prismaticJoint, float ratio){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        Signature signatureJ1 = scene->getSignature(revoluteJoint);
        Signature signatureJ2 = scene->getSignature(prismaticJoint);

        if (signatureJ1.test(scene->getComponentType<Joint2DComponent>()) && signatureJ2.test(scene->getComponentType<Joint2DComponent>())){

            Joint2DComponent myRevoluteJoint = scene->getComponent<Joint2DComponent>(revoluteJoint);
            Joint2DComponent myPrismaticJoint = scene->getComponent<Joint2DComponent>(prismaticJoint);

            updateBody2DPosition(signatureA, bodyA, myBodyA, true);
            updateBody2DPosition(signatureB, bodyB, myBodyB, true);

            b2GearJointDef jointDef;
            jointDef.joint1 = myRevoluteJoint.joint;
            jointDef.joint2 = myPrismaticJoint.joint;
            jointDef.bodyA = myBodyA.body;
            jointDef.bodyB = myBodyB.body;
            jointDef.ratio = ratio;

            joint.joint = world2D->CreateJoint(&jointDef);
            joint.type = Joint2DType::GEAR;

        }else{
            Log::error("Cannot create joint, revoluteJoint or prismaticJoint not created");
            return false;
        }

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadMouseJoint2D(Joint2DComponent& joint, Entity body, Vector2 target){
    Signature signature = scene->getSignature(body);

    if (signature.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBody = scene->getComponent<Body2DComponent>(body);

        updateBody2DPosition(signature, body, myBody, true);

        b2Vec2 myTarget(target.x / pointsToMeterScale, target.y / pointsToMeterScale);

        b2MouseJointDef jointDef;
        jointDef.bodyA = myBody.body;
        jointDef.bodyB = myBody.body;
        jointDef.target = myTarget;

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::MOUSE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadWheelJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor, Vector2 axis){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchor(anchor.x / pointsToMeterScale, anchor.y / pointsToMeterScale);
        b2Vec2 myAxis(axis.x, axis.y);

        b2WheelJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchor, myAxis);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::WHEEL;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadWeldJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchor(anchor.x / pointsToMeterScale, anchor.y / pointsToMeterScale);

        b2WeldJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchor);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::WELD;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadFrictionJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchor){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchor(anchor.x / pointsToMeterScale, anchor.y / pointsToMeterScale);

        b2FrictionJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchor);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::FRICTION;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadMotorJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2MotorJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body);

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::MOTOR;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadRopeJoint2D(Joint2DComponent& joint, Entity bodyA, Entity bodyB, Vector2 anchorA, Vector2 anchorB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body2DComponent>()) && signatureB.test(scene->getComponentType<Body2DComponent>())){

        Body2DComponent myBodyA = scene->getComponent<Body2DComponent>(bodyA);
        Body2DComponent myBodyB = scene->getComponent<Body2DComponent>(bodyB);

        updateBody2DPosition(signatureA, bodyA, myBodyA, true);
        updateBody2DPosition(signatureB, bodyB, myBodyB, true);

        b2Vec2 myAnchorA(anchorA.x / pointsToMeterScale, anchorA.y / pointsToMeterScale);
        b2Vec2 myAnchorB(anchorB.x / pointsToMeterScale, anchorB.y / pointsToMeterScale);

        b2DistanceJointDef jointDef;
        jointDef.Initialize(myBodyA.body, myBodyB.body, myAnchorA, myAnchorB);
        jointDef.minLength = 0;

        joint.joint = world2D->CreateJoint(&jointDef);
        joint.type = Joint2DType::ROPE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

void PhysicsSystem::destroyJoint2D(Joint2DComponent& joint){
    if (world2D && joint.joint){
        world2D->DestroyJoint(joint.joint);

        joint.joint = NULL;
    }
}

bool PhysicsSystem::loadFixedJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);
        JPH::FixedConstraintSettings settings;
        settings.mAutoDetectPoint = true;

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::FIXED;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadDistanceJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::DistanceConstraintSettings settings;
        settings.mPoint1 = JPH::Vec3(anchorA.x, anchorA.y, anchorA.z);
        settings.mPoint2 = JPH::Vec3(anchorB.x, anchorB.y, anchorB.z);
        //settings.mMinDistance = 4.0f;
        //settings.mMaxDistance = 8.0f;

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::DISTANCE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPointJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::PointConstraintSettings settings;
        settings.mPoint1 = settings.mPoint2 = JPH::Vec3(anchor.x, anchor.y, anchor.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::POINT;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadHingeJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 axis, Vector3 normal){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::HingeConstraintSettings settings;
        settings.mPoint1 = settings.mPoint2 = JPH::Vec3(anchor.x, anchor.y, anchor.z);
        settings.mHingeAxis1 = settings.mHingeAxis2 = JPH::Vec3(axis.x, axis.y, axis.z);
        settings.mNormalAxis1 = settings.mNormalAxis2 = JPH::Vec3(normal.x, normal.y, normal.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::HINGE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadConeJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 twistAxis){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::ConeConstraintSettings settings;
        settings.mPoint1 = settings.mPoint2 = JPH::Vec3(anchor.x, anchor.y, anchor.z);
        settings.mTwistAxis1 = settings.mTwistAxis2 = JPH::Vec3(twistAxis.x, twistAxis.y, twistAxis.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::CONE;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPrismaticJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 sliderAxis, float limitsMin, float limitsMax){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::SliderConstraintSettings settings;
        settings.mAutoDetectPoint = true;
        settings.SetSliderAxis(JPH::Vec3(sliderAxis.x, sliderAxis.y, sliderAxis.z));
		settings.mLimitsMin = limitsMin;
		settings.mLimitsMax = limitsMax;

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::PRISMATIC;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadSwingTwistJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchor, Vector3 twistAxis, Vector3 planeAxis, float normalHalfConeAngle, float planeHalfConeAngle, float twistMinAngle, float twistMaxAngle){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::SwingTwistConstraintSettings settings;
        settings.mPosition1 = settings.mPosition2 = JPH::Vec3(anchor.x, anchor.y, anchor.z);
        settings.mTwistAxis1 = settings.mTwistAxis2 = JPH::Vec3(twistAxis.x, twistAxis.y, twistAxis.z);
        settings.mPlaneAxis1 = settings.mPlaneAxis2 = JPH::Vec3(planeAxis.x, planeAxis.y, planeAxis.z);
        settings.mNormalHalfConeAngle = Angle::defaultToRad(normalHalfConeAngle);
        settings.mPlaneHalfConeAngle = Angle::defaultToRad(planeHalfConeAngle);
        settings.mTwistMinAngle = Angle::defaultToRad(twistMinAngle);
        settings.mTwistMaxAngle = Angle::defaultToRad(twistMaxAngle);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::SWINGTWIST;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadSixDOFJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB, Vector3 axisX, Vector3 axisY){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::SixDOFConstraintSettings settings;
        settings.mPosition1 = JPH::Vec3(anchorA.x, anchorA.y, anchorA.z);
        settings.mPosition2 = JPH::Vec3(anchorB.x, anchorB.y, anchorB.z);
        settings.mAxisX1 = settings.mAxisX2 = JPH::Vec3(axisX.x, axisX.y, axisX.z);
        settings.mAxisY1 = settings.mAxisY2 = JPH::Vec3(axisY.x, axisY.y, axisY.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::SIXDOF;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPathJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, std::vector<Vector3> positions, std::vector<Vector3> tangents, std::vector<Vector3> normals, Vector3 pathPosition, bool isLooping){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::Ref<JPH::PathConstraintPathHermite> path = new JPH::PathConstraintPathHermite;

        if (positions.size() != normals.size() != tangents.size()){
            Log::error("Cannot create joint, positions size is different from normals and tangents");
            return false;
        }

        for (int i = 0; i < positions.size(); i++){
            path->AddPoint(JPH::Vec3(positions[i].x, positions[i].y, positions[i].z), JPH::Vec3(tangents[i].x, tangents[i].y, tangents[i].z), JPH::Vec3(normals[i].x, normals[i].y, normals[i].z));
        }
        path->SetIsLooping(isLooping);

        JPH::PathConstraintSettings settings;
        settings.mPath = path;
        settings.mPathPosition = JPH::Vec3(pathPosition.x, pathPosition.y, pathPosition.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::PATH;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadGearJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Entity hingeA, Entity hingeB, int numTeethGearA, int numTeethGearB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);
    Signature signatureHingeA = scene->getSignature(hingeA);
    Signature signatureHingeB = scene->getSignature(hingeB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        if (signatureHingeA.test(scene->getComponentType<Joint3DComponent>()) && signatureHingeB.test(scene->getComponentType<Joint3DComponent>())){

            Joint3DComponent myHingeA = scene->getComponent<Joint3DComponent>(hingeA);
            Joint3DComponent myHingeB = scene->getComponent<Joint3DComponent>(hingeB);

            if ((myHingeA.type != Joint3DType::HINGE) || (myHingeB.type != Joint3DType::HINGE)){
                Log::error("Cannot create joint, hingeA or hingeB is not hinge type");
                return false;
            }

            // Disable collision between gears
            JPH::Ref<JPH::GroupFilterTable> group_filter = new JPH::GroupFilterTable(2);
            group_filter->DisableCollision(0, 1);
            myBodyA.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, 0, 0));
            myBodyB.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, 0, 1));

            JPH::HingeConstraintSettings* hingeSetA = (JPH::HingeConstraintSettings*)((JPH::HingeConstraint*)myHingeA.joint)->GetConstraintSettings().GetPtr();
            JPH::HingeConstraintSettings* hingeSetB = (JPH::HingeConstraintSettings*)((JPH::HingeConstraint*)myHingeB.joint)->GetConstraintSettings().GetPtr();

            JPH::GearConstraintSettings settings;
            settings.mHingeAxis1 = hingeSetA->mHingeAxis1;
            settings.mHingeAxis2 = hingeSetB->mHingeAxis1;
            settings.SetRatio(numTeethGearA, numTeethGearB);

            joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

            ((JPH::GearConstraint *)joint.joint)->SetConstraints(myHingeA.joint, myHingeB.joint);
            world3D->AddConstraint(joint.joint);
            joint.type = Joint3DType::GEAR;
        }else{
            Log::error("Cannot create joint, error in hingeA or hingeB");
            return false;
        }

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadRackAndPinionJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Entity hinge, Entity slider, int numTeethRack, int numTeethGear, int rackLength){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);
    Signature signatureHinge = scene->getSignature(hinge);
    Signature signatureSlider = scene->getSignature(slider);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        if (signatureHinge.test(scene->getComponentType<Joint3DComponent>()) && signatureSlider.test(scene->getComponentType<Joint3DComponent>())){

            Joint3DComponent myHinge = scene->getComponent<Joint3DComponent>(hinge);
            Joint3DComponent mySlider = scene->getComponent<Joint3DComponent>(slider);

            if ((myHinge.type != Joint3DType::HINGE) || (mySlider.type != Joint3DType::PRISMATIC)){
                Log::error("Cannot create joint, hinge or slider is not hinge and prismatic (slider) types");
                return false;
            }

            // Disable collision between gears
            JPH::Ref<JPH::GroupFilterTable> group_filter = new JPH::GroupFilterTable(2);
            group_filter->DisableCollision(0, 1);
            myBodyA.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, 0, 0));
            myBodyB.body->SetCollisionGroup(JPH::CollisionGroup(group_filter, 0, 1));

            JPH::HingeConstraintSettings* hingeSetA = (JPH::HingeConstraintSettings*)((JPH::HingeConstraint*)myHinge.joint)->GetConstraintSettings().GetPtr();
            JPH::SliderConstraintSettings* sliderSetB = (JPH::SliderConstraintSettings*)((JPH::SliderConstraint*)mySlider.joint)->GetConstraintSettings().GetPtr();

            JPH::RackAndPinionConstraintSettings settings;
            settings.mHingeAxis = hingeSetA->mHingeAxis1;
            settings.mSliderAxis = sliderSetB->mSliderAxis2;
            settings.SetRatio(numTeethRack, rackLength, numTeethGear);

            joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

            ((JPH::GearConstraint *)joint.joint)->SetConstraints(myHinge.joint, mySlider.joint);
            world3D->AddConstraint(joint.joint);
            joint.type = Joint3DType::RACKANDPINON;
        }else{
            Log::error("Cannot create joint, error in hinge or slider");
            return false;
        }

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

bool PhysicsSystem::loadPulleyJoint3D(Joint3DComponent& joint, Entity bodyA, Entity bodyB, Vector3 anchorA, Vector3 anchorB, Vector3 fixedPointA, Vector3 fixedPointB){
    Signature signatureA = scene->getSignature(bodyA);
    Signature signatureB = scene->getSignature(bodyB);

    if (signatureA.test(scene->getComponentType<Body3DComponent>()) && signatureB.test(scene->getComponentType<Body3DComponent>())){

        Body3DComponent myBodyA = scene->getComponent<Body3DComponent>(bodyA);
        Body3DComponent myBodyB = scene->getComponent<Body3DComponent>(bodyB);

        updateBody3DPosition(signatureA, bodyA, myBodyA, true);
        updateBody3DPosition(signatureB, bodyB, myBodyB, true);

        JPH::PulleyConstraintSettings settings;
        settings.mBodyPoint1 = JPH::Vec3(anchorA.x, anchorA.y, anchorA.z);
        settings.mBodyPoint2 = JPH::Vec3(anchorB.x, anchorB.y, anchorB.z);
        settings.mFixedPoint1 = JPH::Vec3(fixedPointA.x, fixedPointA.y, fixedPointA.z);
        settings.mFixedPoint2 = JPH::Vec3(fixedPointB.x, fixedPointB.y, fixedPointB.z);

        joint.joint = settings.Create(*myBodyA.body, *myBodyB.body);

        world3D->AddConstraint(joint.joint);
        joint.type = Joint3DType::PULLEY;

    }else{
        Log::error("Cannot create joint, error in bodyA or bodyB");
        return false;
    }

    return true;
}

void PhysicsSystem::load(){
}

void PhysicsSystem::destroy(){

}

void PhysicsSystem::update(double dt){
	auto bodies2d = scene->getComponentArray<Body2DComponent>();
    auto joints2d = scene->getComponentArray<Joint2DComponent>();

	for (int i = 0; i < bodies2d->size(); i++){
		Body2DComponent& body = bodies2d->getComponentFromIndex(i);
		Entity entity = bodies2d->getEntity(i);
		Signature signature = scene->getSignature(entity);

        if (body.body){
            updateBody2DPosition(signature, entity, body, body.newBody);

            body.newBody = false;
        }
    }

    if (bodies2d->size() > 0){
        int32 velocityIterations = 6;
        int32 positionIterations = 2;

        world2D->Step(dt, velocityIterations, positionIterations);
    }

	for (int i = 0; i < bodies2d->size(); i++){
		Body2DComponent& body = bodies2d->getComponentFromIndex(i);
		Entity entity = bodies2d->getEntity(i);
		Signature signature = scene->getSignature(entity);

        if (body.body){
            b2Vec2 position = body.body->GetPosition();
            float angle = body.body->GetAngle();
            if (signature.test(scene->getComponentType<Transform>())){
                Transform& transform = scene->getComponent<Transform>(entity);

                Vector3 nPosition = Vector3(position.x * pointsToMeterScale, position.y * pointsToMeterScale, transform.worldPosition.z);
                if (transform.position != nPosition){
                    transform.position = nPosition;
                    transform.needUpdate = true;
                }

                if (transform.worldRotation.getRoll() != angle){
                    Quaternion rotation;
                    rotation.fromAngle(Angle::radToDefault(angle));

                    transform.rotation = rotation;
                    transform.needUpdate = true;
                }

            }
        }
    }

    auto bodies3d = scene->getComponentArray<Body3DComponent>();

	for (int i = 0; i < bodies3d->size(); i++){
		Body3DComponent& body = bodies3d->getComponentFromIndex(i);
		Entity entity = bodies3d->getEntity(i);
		Signature signature = scene->getSignature(entity);

        if (body.body){
            updateBody3DPosition(signature, entity, body, body.newBody);

            body.newBody = false;
        }
    }

    if (bodies3d->size() > 0){
		const int cCollisionSteps = 1;
        const int cIntegrationSubSteps = 1;

		world3D->Update(dt, cCollisionSteps, cIntegrationSubSteps, temp_allocator, job_system);
	}

	for (int i = 0; i < bodies3d->size(); i++){
		Body3DComponent& body = bodies3d->getComponentFromIndex(i);
		Entity entity = bodies3d->getEntity(i);
		Signature signature = scene->getSignature(entity);

        if (body.body){
            JPH::RVec3 position = body.body->GetPosition();
            JPH::Quat rotation = body.body->GetRotation();
            if (signature.test(scene->getComponentType<Transform>())){
                Transform& transform = scene->getComponent<Transform>(entity);

                if (!std::isnan(position.GetX()) && !std::isnan(position.GetY()) && !std::isnan(position.GetZ())){
                    Vector3 nPosition = Vector3(position.GetX(), position.GetY(), position.GetZ());
                    if (transform.position != nPosition){
                        transform.position = nPosition;
                        transform.needUpdate = true;
                    }
                }

                Quaternion nRotation = Quaternion(rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ());
                if (transform.rotation != nRotation){
                    transform.rotation = nRotation;
                    transform.needUpdate = true;
                }

            }
        }
    }
    
}

void PhysicsSystem::draw(){

}

void PhysicsSystem::entityDestroyed(Entity entity){
    Signature signature = scene->getSignature(entity);

    if (signature.test(scene->getComponentType<Body2DComponent>())){
        destroyBody2D(scene->getComponent<Body2DComponent>(entity));
    }
    if (signature.test(scene->getComponentType<Joint2DComponent>())){
        destroyJoint2D(scene->getComponent<Joint2DComponent>(entity));
    }
}
