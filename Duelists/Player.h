#pragma once
#include "Character.h"
#include <vector>

struct InventoryItem;
class Player : public Character {
protected:
	std::vector<InventoryItem> items;
public:
	Player(int MaxHealth, int AttackPower, int Armor, int MaxStamina, std::string Name) : Character(MaxHealth, AttackPower, Armor, MaxStamina, Name) {};
	Action ChooseAction() override;
	void AddInventoryItem(InventoryItem newItem);

};