#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Box2D/Box2D.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <QtCore>
#include <QDebug>

#include "../../ZkCommon/Constants.h"
#include "../../ZkCommon/LibraryCast.h"

#include "PlayerEntity.h"
#include "../Renderables/BoxRenderable.h"
#include "../GameSystem.h"

using namespace Zk::Common;
using namespace Zk::Game;

PlayerEntity::ContactInfo::ContactInfo(b2Body * myBody, b2Contact * original)
{
	this->original = original;
	
	b2WorldManifold worldManifold;
	original->GetWorldManifold(&worldManifold);
	
	if (original->GetFixtureA()->GetBody() == myBody)
	{
		normal = worldManifold.normal;
		toucher = original->GetFixtureB()->GetBody();
	}
	else
	{
		normal = -worldManifold.normal;
		toucher = original->GetFixtureA()->GetBody();
	}
}

PlayerEntity::PlayerEntity(sf::Vector2f pos) :
	Entity(nullptr, nullptr),
	BodyCollisionListener(nullptr),
	std::enable_shared_from_this<PlayerEntity>()
{
	b2World & world = GameSystem::getInstance()->getPhysicsSystem().getWorld();
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = lib_cast<b2Vec2>(pos);
	bodyDef.userData = (void*)this;
	b2Body * body = world.CreateBody(&bodyDef);
	
	float radius = 0.5f;
	b2PolygonShape polyShape;
	polyShape.SetAsBox(radius, radius);
	
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polyShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.6f;
	body->CreateFixture(&fixtureDef);
	body->SetFixedRotation(true);
	
	setBody(body);
	setFilteringBody(body);
	
	BoxRenderable * br = new BoxRenderable(
		body, "../data/grenade-pack.png"
	);
	setRenderable(br);
	
	jumpCooldown = 0.0;
}

PlayerEntity::~PlayerEntity()
{
	
}

void PlayerEntity::registerMe()
{
	GameSystem::getInstance()->getPhysicsSystem().registerListener(
		shared_from_this()
	);
}

void PlayerEntity::onBeginContactEvent(b2Contact * contact)
{
	ContactInfo ci(getBody(), contact);
	contacts.push_back(ci);
	Entity * entToucher = (Entity*)ci.toucher->GetUserData();
	qDebug() << "Toucher" << entToucher;
	qDebug() << "Toucher type:" << (int)entToucher->getType();
	qDebug() << "Player is being touched by" << contacts.size() << "touchers";
}

void PlayerEntity::onEndContactEvent(b2Contact * contact)
{
	//using namespace std::rel_ops;
	ContactInfo ci(getBody(), contact);
	contacts.erase(std::find(contacts.begin(), contacts.end(), ci));
	qDebug() << "Player is being touched by" << contacts.size() << "touchers";
}

void PlayerEntity::onPreSolveEvent(b2Contact * contact, const b2Manifold * oldManifold)
{
	
}

void PlayerEntity::onPostSolveEvent(b2Contact * contact, const b2ContactImpulse * impulse)
{
	
}

void PlayerEntity::update(double step)
{
	jumpCooldown = std::max(0.0, jumpCooldown - step);
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		getBody()->ApplyForceToCenter(b2Vec2(-10.f, 0.f), true);
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		getBody()->ApplyForceToCenter(b2Vec2(10.f, 0.f), true);
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && jumpCooldown == 0.f)
	{
		//Sprawdźmy, czy stoimy na twardym gruncie
		b2Vec2 normal(0.f, 0.f);
		for (const ContactInfo & ci : contacts)
		{
			if (ci.normal.y > 0.05f)
				normal += ci.normal;
			else
				qDebug() << lib_cast<QPointF>(ci.normal) << "is bad";
		}
		
		qDebug() << lib_cast<QPointF>(normal);
		
		if (normal.LengthSquared() > 0.f)
		{
			float scale = -7.5f / normal.Length();
			normal.x *= scale;
			normal.y *= scale;
			
			qDebug() << "Jump!";
			getBody()->ApplyLinearImpulse(
				normal, b2Vec2(0.f, 0.f), true
			);
			
			jumpCooldown = 0.25;
		}
	}
}

EntityType PlayerEntity::getType() const
{
	return EntityType::PlayerEntity;
}
