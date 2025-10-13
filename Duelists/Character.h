#pragma once
#include <string>
#include <iostream>

enum Action {
	ATTACK,
	PARRY,
	DEFEND,
	HEAL,
	DODGE,
	HEAVY_ATTACK,
	NONE
};

class Character {
public:
	Character(int health, int attack, int armor, int stamina, std::string name);
	int GetHealth() const { return Health; }
	int GetAttackPower() const { return AttackPower; }
	int GetStamina() const { return Stamina;  }
	void UpdateHealth(int Amount);
	void UpdateStamina(bool Increase, bool Heavy=false);
	bool IsAlive() const { return Health > 0; }
	void ResetStats();
	virtual Action ChooseAction() = 0;
	Action GetActionFromInput(int Input);
	std::string GetName() const { return Name; }

protected:
	std::string Name;
	int MaxHealth;
	int Health;
	int AttackPower;
	int Armor;
	int MaxStamina;
	int Stamina;
};