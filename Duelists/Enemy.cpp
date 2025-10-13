#include "Enemy.h"

Enemy::Enemy(int MaxHealth, int AttackPower, int Armor, int MaxStamina, std::string Name) : Character(MaxHealth, AttackPower, Armor, MaxStamina, Name), Generator(std::random_device{}()), Distribution(0,5) {}


Action Enemy::ChooseAction()
{
	int Input = Distribution(Generator);
	if (Stamina == 0) 
	{
		std::uniform_int_distribution<> LimitedDistribution(0,5);
		int num = LimitedDistribution(Generator);
		Input = (num == 1 || num == 4 || num == 5) ? 2 : num;
	}
	return GetActionFromInput(Input);
}

void Enemy::IncreaseDifficulty(int RoundNumber)
{
	switch (RoundNumber) {
	case 1:
		return;
	case 2:
		Name = "Orc";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		break;
	case 3:
		Name = "Troll";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		break;
	case 4:
		Name = "Ogre";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		break;
	case 5:
		Name = "Dragon";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		Armor = RoundNumber / 2;
		break;
	}
}
