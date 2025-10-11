// Duelists.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include "Player.h"
#include "Enemy.h"
#include "Log.h"
#include "InventoryItem.h"

bool CheckParrySuccess(int difficulty, int stamina)
{
    int successChance = 30 + (5 * stamina) - (2 * difficulty);
    if (successChance < 5) successChance = 5;
    if (successChance > 95) successChance = 95;

    std::mt19937 Generator(std::random_device{}());
    std::uniform_int_distribution<> Distribution(1, 100);
    int Roll = Distribution(Generator);
    return Roll <= successChance;
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber);
void FindItem(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber);

int main()
{
    Player MainPlayer(5, 2, 10, 2, "Hero");
	Enemy MainEnemy(1, 1, 0, 1, "Goblin");
	int RoundNumber = 1;
	while (MainPlayer.IsAlive()) { 
		Log::Line();
		Log::LogMessage(LOG_DEFAULT, ("ROUND " + std::to_string(RoundNumber)));
		Log::Line();
		Log::NewLine();

		ProcessOutcome(MainPlayer, MainEnemy, RoundNumber);
		if (!MainEnemy.IsAlive()) {
			Log::LogMessage(LOG_INFO, ("You have defeated " + MainEnemy.GetName() + "!"));
			RoundNumber++;
			FindItem(MainPlayer, MainEnemy, RoundNumber);
			if (RoundNumber > 5) {
				Log::LogMessage(LOG_INFO, "You have defeated all enemies! You win!");
				break;
			}
			MainEnemy.IncreaseDifficulty(RoundNumber);

			MainPlayer.ResetStats();
			Log::LogMessage(LOG_INFO, "You recover your stamina and energy for the next fight!");
			Log::Line();
            Log::LogMessage(LOG_WARNING, ("Now facing: " + MainEnemy.GetName() +
                " (Health: " + std::to_string(MainEnemy.GetHealth()) +
                ", Attack: " + std::to_string(MainEnemy.GetAttackPower()) + ")\n"));

		}
	}
	Log::Line();
	if(!MainPlayer.IsAlive()) {
		Log::LogMessage(LOG_ERROR, "You have been defeated! Game Over!\n");
	}
	Log::LogMessage(LOG_DEFAULT, "Thanks for playing!\n");
	return 0;
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber)
{
	Action PlayerAction = MainPlayer.ChooseAction();
	Action MainEnemyAction = MainEnemy.ChooseAction();
	std::string PlayerActionStr = (PlayerAction == ATTACK) ? 
		"Attack" : 
		(PlayerAction == DEFEND) ? 
			"Defend" : (PlayerAction == PARRY) ? "Parry" : "None";
	std::string MainEnemyActionStr = (MainEnemyAction == ATTACK) ?
		"Attack" :
		(MainEnemyAction == DEFEND) ?
		"Defend" : (MainEnemyAction == PARRY) ? "Parry" : "None";
	Log::Line();
	Log::SpacedMessage(LOG_DEFAULT, ("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + MainEnemyActionStr + "s"));
	Log::Line();
	switch (PlayerAction) {
	case ATTACK:
		switch (MainEnemyAction) {
			case ATTACK:
				Log::LogMessage(LOG_WARNING, "Its a clash! The weapons ring as they hit eachother!");
				break;
			case DEFEND:
					Log::LogMessage(LOG_ERROR, (MainEnemy.GetName() + " blocks your attack!"));
					MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() / 2));
					break;
			case PARRY:
				Log::LogMessage(LOG_INFO, (MainEnemy.GetName() + " parries your attack and "));
				if (CheckParrySuccess(10 - (RoundNumber + 1), MainEnemy.GetStamina())) {
					Log::LogMessage(LOG_ERROR, "counters!");
					MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() * 2));
				}
				else {
					Log::LogMessage(LOG_INFO, "fails to counter!");
					MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() * 2));
				}
				break;
		}
		break;
	case DEFEND:
		switch (MainEnemyAction) {
			case ATTACK:
				Log::LogMessage(LOG_INFO, ("You block " + MainEnemy.GetName() + "'s attack!"));
				MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() / 2));
				break;
			case DEFEND:
				Log::LogMessage(LOG_DEFAULT, ("You and " + MainEnemy.GetName() + " are recovering Stamina."));
				break;
			case PARRY:
				Log::LogMessage(LOG_DEFAULT, (MainEnemy.GetName() + " misreads your tell and drains stamina while you recover!"));
				break;
		}
		break;
	case PARRY:
		switch (MainEnemyAction) {
			case ATTACK:
				Log::LogMessage(LOG_DEFAULT, ("You parry " + MainEnemy.GetName() + "'s attack and "));
				if (CheckParrySuccess(10 - RoundNumber, MainPlayer.GetStamina())) {
					Log::LogMessage(LOG_INFO, "counter!");
					MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() * 2));
				}
				else {
					Log::LogMessage(LOG_ERROR, "fail to counter!");
					MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() * 2));
				}
				break;
			case DEFEND:
				Log::LogMessage(LOG_DEFAULT, ("You misread the tell and drain your stamina, " + MainEnemy.GetName() + " recovers!"));
				break;
			case PARRY:
				Log::LogMessage(LOG_DEFAULT, ("You and " + MainEnemy.GetName() + " both try to parry, and drain stamina!"));
				break;
		}
		break;
	}
	Log::NewLine();
}

void FindItem(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber)
{
    static const std::vector<std::string> EquipmentTypes = {
        "Sword", "Shield", "Helmet", "Armor"
    };
    static const std::vector<std::string> EquipmentDescriptions = {
        "of Valor", "of Shadows", "of the Ancients", "of Fury"
    };

    std::mt19937 Generator(std::random_device{}());
    std::uniform_int_distribution<> TypeDist(0, EquipmentTypes.size() - 1);
    std::uniform_int_distribution<> DescDist(0, EquipmentDescriptions.size() - 1);
	std::uniform_int_distribution<> HealthDist(0.1, RoundNumber * 10);
	std::uniform_int_distribution<> ArmorDist(0.1, RoundNumber * 5);
	std::uniform_int_distribution<> AttackDist(0.1, RoundNumber * 10);
	std::uniform_int_distribution<> StaminaDist(0.1, RoundNumber * 5);

    std::string ItemName = EquipmentTypes[TypeDist(Generator)] + " " + EquipmentDescriptions[DescDist(Generator)];
	
	float ItemPowerIncrease = AttackDist(Generator);
	float ItemHealthIncrease = HealthDist(Generator);
	float ItemArmorIncrease = ArmorDist(Generator);
	float ItemStaminaIncrease = StaminaDist(Generator);

	InventoryItem NewItem = { ItemName, ItemHealthIncrease, ItemArmorIncrease, ItemStaminaIncrease, ItemPowerIncrease };
    Log::LogMessage(LOG_INFO, (MainEnemy.GetName() + " dropped: " + ItemName + "!"));
    MainPlayer.AddInventoryItem(NewItem);

}
