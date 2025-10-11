#pragma once
#include "Character.h"
#include <random>

class Enemy : public Character {
public:
	Enemy(int MaxHealth, int AttackPower, int Armor, int MaxStamina, std::string Name);
	Action ChooseAction() override;
	void IncreaseDifficulty(int RoundNumber);
private:
	std::mt19937 Generator;
	std::uniform_int_distribution<> Distribution;
};