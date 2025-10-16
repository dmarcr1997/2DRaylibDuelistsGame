#include "Player.h"
#include <iostream>
#include "CombatLog.h"
#include "InventoryItem.h"
#include "raylib.h"

Action Player::ChooseAction()
{
    return GetActionFromInput(3);
}

void Player::AddInventoryItem(InventoryItem newItem)
{
    int newHealth = MaxHealth + (newItem.healthIncrease / 100) * MaxHealth;
    int newAttackPower = AttackPower + (newItem.attackIncrease / 100) * AttackPower;
    int newStamina = Stamina + (newItem.staminaIncrease / 100) * MaxStamina;
    int newArmor = Armor + (newItem.armorIncrease/ 100) * Armor;
    MaxHealth = newHealth;
    AttackPower = newAttackPower;
    Stamina = newStamina;
    Armor = newArmor;
	CombatLog::AddMessage(("Equipped: " + newItem.name), LIGHTGRAY, 1.5f);
	CombatLog::AddMessage(("STATS(H: " + std::to_string(newItem.healthIncrease / 100) + ", ATK: " + std::to_string(newItem.attackIncrease / 100)).c_str(), LIGHTGRAY, 1.5f);
    CombatLog::AddMessage(("S: " + std::to_string(newItem.staminaIncrease / 100) + ", ARM: " + std::to_string(newItem.armorIncrease / 100) + ")").c_str(), LIGHTGRAY, 1.5f);
    items.push_back(newItem);
}
