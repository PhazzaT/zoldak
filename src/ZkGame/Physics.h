#pragma once

#include <Box2D/Box2D.h>

#include <memory>

namespace Zk {
namespace Game {

class PhysicsSystem;
class PhysicsComponent;
class PhysicsEvent;

class CollisionListener
{
public:
	virtual bool interestedInBeginContactEvent(b2Contact * contact) = 0;
	virtual bool interestedInEndContactEvent(b2Contact * contact) = 0;
	virtual void onBeginContactEvent(b2Contact * contact) = 0;
	virtual void onEndContactEvent(b2Contact * contact) = 0;
};

class BodyCollisionListener : public CollisionListener
{
public:
	BodyCollisionListener(b2Body * filteringBody);
	
	virtual bool interestedInBeginContactEvent(b2Contact * contact) override final;
	virtual bool interestedInEndContactEvent(b2Contact * contact) override final;
	
	inline void setFilteringBody(b2Body * fb)
	{ filteringBody = fb; }
	inline b2Body * getFilteringBody() const
	{ return filteringBody; }
	
private:
	b2Body * filteringBody;
};

//Do wewnętrznego użytku
class ContactListener : public b2ContactListener
{
public:
	ContactListener();
	
	virtual void BeginContact(b2Contact * contact) override;
	virtual void EndContact(b2Contact * contact) override;
	virtual void PreSolve(b2Contact * contact, const b2Manifold * oldManifold) override;
	virtual void PostSolve(b2Contact * contact, const b2ContactImpulse * impulse) override;
	
	void registerListener(std::weak_ptr<CollisionListener> cl);
	void discardOldListeners();
	
private:
	std::vector<std::weak_ptr<CollisionListener>> collisionListeners;
};

//Wrapper na Box2D i jego system eventów
class PhysicsSystem
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	
	void registerListener(std::weak_ptr<CollisionListener> cl);
	inline b2World & getWorld()
	{ return world; }
	
	void simulate(double step);
	
private:
	b2World world;
	ContactListener cl;
};

}}
