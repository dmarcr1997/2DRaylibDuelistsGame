#include "Player.h"
#include <iostream>
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
    DrawText(("Equipped: " + newItem.name).c_str(), 190, 420, 20, LIGHTGRAY);
    DrawText(("STATS \nHealthIncr: " + std::to_string(newItem.healthIncrease / 100)).c_str(), 190, 440, 20,LIGHTGRAY);
    DrawText(("AttackIncr: " + std::to_string(newItem.attackIncrease / 100)).c_str(), 190, 480, 20,LIGHTGRAY);
    DrawText(("StaminaIncr: " + std::to_string(newItem.staminaIncrease / 100)).c_str(), 190, 500, 20,LIGHTGRAY);
    DrawText(("ArmorIncr: " + std::to_string(newItem.armorIncrease / 100)).c_str(), 190, 520, 20,LIGHTGRAY);
    items.push_back(newItem);
}
