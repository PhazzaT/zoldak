#include <algorithm>
#include <memory>

#include "Player.h"
#include "Entities/PlayerEntity.h"
#include "Config/InputConfig.h"
#include "SpawnerMesh.h"
#include "Game.h"
#include "GameSystem.h"

using namespace Zk::Game;

Player::Player(int id)
	: ui(*this)
{
	this->id = id;
	timeToRespawn = 0.0;
	numKills = 0;
	numDeaths = 0;
}

void Player::setMouseDevice(MouseDeviceHandle md)
{
	mouseDevice = md;
}

void Player::setSpawnerMesh(const SpawnerMesh & sm)
{
	spawnerMesh = sm;
}

void Player::setInputConfig(const InputConfig & ic)
{
	inputConfig = ic;
}

void Player::setWeaponDef(const WeaponDef & wd)
{
	weaponDef = wd;
}

void Player::reportKill()
{
	numKills++;
}

void Player::reportDeath()
{
	numDeaths++;
}

std::weak_ptr<PlayerEntity> Player::getPlayerEntity() const
{
	return entity;
}

int Player::getID() const
{
	return id;
}

int Player::getKillCount() const
{
	return numKills;
}

int Player::getDeathCount() const
{
	return numDeaths;
}

double Player::getSecondsToRespawn() const
{
	return timeToRespawn;
}

void Player::update(double step)
{
	auto ptr = entity.lock();
	if (ptr == nullptr)
	{
		timeToRespawn = std::max(0.0, timeToRespawn - step);
		
		if (timeToRespawn == 0.0)
		{
			ptr = std::make_shared<PlayerEntity>(
				*this,
				spawnerMesh.pickSpawnLocation(),
				inputConfig,
				mouseDevice,
				weaponDef
			);
			
			ptr->registerMe();
			
			Game::getInstance()->addEntity(ptr);
			entity = ptr;
			
			timeToRespawn = RESPAWN_TIME;
		}
	}
}

void Player::paintUI(sf::RenderTarget * rt)
{
	ui.paint(rt);
}