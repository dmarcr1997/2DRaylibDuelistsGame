#include "Player.h"
#include <iostream>
#include "Log.h"
#include "InventoryItem.h"

Action Player::ChooseAction()
{
    int Input;
    while (true)
    {
		Log::LogMessage(LOG_DEFAULT, "Choose your action (1. Attack 2. Parry 3. Defend): ");
        std::cin >> Input;
        if (std::cin.fail() || Input < 1 || Input > 3)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			Log::LogMessage(LOG_ERROR, "Invalid input. Please enter a number between 1 and 3(Attack, Parry, Defend).\n");
        }
        else if (Input == 2 && Stamina == 0) {
            Log::LogMessage(LOG_ERROR, "You are Exhausted: You cannot Parry until you Defend(3)");
        }
        else {
            break;
        }
    }
    Input -= 1;
	return GetActionFromInput(Input);
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
    Log::NewLine();
    Log::LogMessage(LOG_DEFAULT, ("Equipped: " + newItem.name));
    Log::Line();
    Log::LogMessage(LOG_INFO, "STATS \nHealthIncr: " + std::to_string(newItem.healthIncrease / 100));
    Log::LogMessage(LOG_INFO, "AttackIncr: " + std::to_string(newItem.attackIncrease / 100));
    Log::LogMessage(LOG_INFO, "StaminaIncr: " + std::to_string(newItem.staminaIncrease / 100));
    Log::LogMessage(LOG_INFO, "ArmorIncr: " + std::to_string(newItem.armorIncrease / 100));
    Log::Line();
    items.push_back(newItem);
}
