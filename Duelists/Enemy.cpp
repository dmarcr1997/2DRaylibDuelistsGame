#include "Enemy.h"
#include <map>

std::map<std::string, std::vector<const char*>> EnemyTextureMap = {
	{"Goblin", {"../SourceArt/Characters/Goblin/Goblin_IdleBlinking_Sprite.png", "../SourceArt/Characters/Goblin/Goblin_Attacking_Sprite.png", "../SourceArt/Characters/Goblin/Goblin_Defend_Sprite.png", "../SourceArt/Characters/Goblin/Goblin_Parry_Sprite.png"}},
	{"Ogre", {"../SourceArt/Characters/Ogre/Ogre_IdleBlinking_Sprite.png", "../SourceArt/Characters/Ogre/Ogre_Attacking_Sprite.png", "../SourceArt/Characters/Ogre/Ogre_Defend_Sprite.png", "../SourceArt/Characters/Ogre/Ogre_Parry_Sprite.png"}},
	{"Skeleton", {"../SourceArt/Characters/Skeleton/Skeleton_Idle_Sprite.png", "../SourceArt/Characters/Skeleton/Skeleton_Attacking_Sprite.png", "../SourceArt/Characters/Skeleton/Skeleton_Defend_Sprite.png", "../SourceArt/Characters/Skeleton/Skeleton_Parry_Sprite.png"}},
	{"Cyclop", {"../SourceArt/Characters/Cyclop/Cyclop_IdleBlinking_Sprite.png", "../SourceArt/Characters/Cyclop/Cyclop_Attacking_Sprite.png", "../SourceArt/Characters/Cyclop/Cyclop_Defend_Sprite.png", "../SourceArt/Characters/Cyclop/Cyclop_Parry_Sprite.png"}},
};
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
		SwapTextureSet(EnemyTextureMap.at(Name));
		return;
	case 2:
		Name = "Ogre";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		SwapTextureSet(EnemyTextureMap.at(Name));
		break;
	case 3:
		Name = "Skeleton";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		SwapTextureSet(EnemyTextureMap.at(Name));
		break;
	case 4:
		Name = "Cyclop";
		Health = RoundNumber;
		AttackPower = RoundNumber;
		SwapTextureSet(EnemyTextureMap.at(Name));
		break;
	}
}
