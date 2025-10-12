// Duelists.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <random>
#include "Player.h"
#include "Enemy.h"
#include "Log.h"
#include "InventoryItem.h"
#include "raylib.h"

enum GameState {
	WAITING_FOR_INPUT,
	PROCESSING,
	GAME_OVER,
	INFO
};

bool CheckParrySuccess(int difficulty, int stamina);
void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction, int RoundNumber);
void FindItem(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber);

int main()
{
	//Raylib init
	const Vector2 screenDims = { 800, 600 };
	InitWindow(
		screenDims.x,
		screenDims.y,
		"Dueling Game"
	);
	SetTargetFPS(60);

	//Game init
	Player MainPlayer(5, 2, 10, 2, "Hero");
	Enemy MainEnemy(1, 1, 0, 1, "Goblin");
	int RoundNumber = 1;
	int WaitDuration = 3;
	GameState State = WAITING_FOR_INPUT;
	GameState StateBuffer = WAITING_FOR_INPUT;

	Action PlayerAction = NONE;

	while (!WindowShouldClose()) {
		if (State == INFO) {
			WaitTime(WaitDuration);
			State = StateBuffer;
			WaitDuration = 3;
		}
		BeginDrawing();
		ClearBackground(BLACK);
		if (State == GAME_OVER) {
			DrawText("Thanks for playing!\n", 190, 200, 20, GREEN);
			DrawText("Game Over. Press ESC to exit.", 190, 220, 20, LIGHTGRAY);
			EndDrawing();
			continue;
		}

		
		std::string RoundMessage = "ROUND " + std::to_string(RoundNumber);
		DrawText(RoundMessage.c_str(), 10, 15, 20, LIGHTGRAY);
		if (State == WAITING_FOR_INPUT) {
			DrawText("Choose an Action - (1. Attack, 2. Parry, 3. Defend)", 10, 30, 20, GREEN);
			switch (GetKeyPressed())
			{
				case KEY_ONE:
					PlayerAction = ATTACK;
					State = PROCESSING;
					break;
				case KEY_TWO:
					if (MainPlayer.GetStamina() > 0) {
						MainPlayer.UpdateStamina(false);
						PlayerAction = PARRY;
						State = PROCESSING;
					}
					else {
						DrawText("You are Exhausted: You cannot Parry until you Defend(3)", 10, 300, 20, RED);
						State = INFO;
						StateBuffer = WAITING_FOR_INPUT;
					}
					break;
				case KEY_THREE:
					MainPlayer.UpdateStamina(true);
					PlayerAction = DEFEND;
					State = PROCESSING;
					break;
				default:
					break;
			}
		} else if(State == PROCESSING) {
			ProcessOutcome(MainPlayer, MainEnemy, PlayerAction, RoundNumber);
			StateBuffer = WAITING_FOR_INPUT;

			if (!MainEnemy.IsAlive()) {
				WaitDuration = 6;
				std::string EnemyDefeatMessage = "You have defeated " + MainEnemy.GetName() + "!";
				DrawText(EnemyDefeatMessage.c_str(), 190, 200, 20, LIGHTGRAY);
				RoundNumber++;
				FindItem(MainPlayer, MainEnemy, RoundNumber);
				if (RoundNumber > 5) {
					DrawText("You have defeated all enemies! You win!", 190, 220, 20, GREEN);
					StateBuffer = GAME_OVER;
				} else {
					MainEnemy.IncreaseDifficulty(RoundNumber);

					MainPlayer.ResetStats();
					DrawText("You recover your stamina and energy for the next fight!", 190, 220, 20, GREEN);

					DrawText(("Now facing: " + MainEnemy.GetName() +
						" (Health: " + std::to_string(MainEnemy.GetHealth()) +
						", Attack: " + std::to_string(MainEnemy.GetAttackPower()) + ")\n").c_str(), 190, 240, 20, LIGHTGRAY);
					StateBuffer = WAITING_FOR_INPUT;
				}
				
			}

			if (!MainPlayer.IsAlive()) {
				DrawText("You have been defeated! Game Over!\n", 190, 220, 20, GREEN);
				StateBuffer = GAME_OVER;
			}
			State = INFO;
		}
		
		EndDrawing();
	}

	return 0;
}

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

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction, int RoundNumber)
{
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
	DrawText(("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + MainEnemyActionStr + "s").c_str(),  10, 60, 20, LIGHTGRAY);
	Log::Line();
	switch (PlayerAction) {
	case ATTACK:
		switch (MainEnemyAction) {
		case ATTACK:
			DrawText("Its a clash! The weapons ring as they hit eachother!", 10, 120, 20, YELLOW);
			break;
		case DEFEND:
			DrawText((MainEnemy.GetName() + " blocks your attack!").c_str(), 10, 120, 20, RED);
			MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() / 2));
			break;
		case PARRY:
			DrawText((MainEnemy.GetName() + " parries your attack and ").c_str(), 10, 120, 20, LIGHTGRAY);
			if (CheckParrySuccess(10 - (RoundNumber + 1), MainEnemy.GetStamina())) {
				DrawText("counters!", 10, 140, 20, RED);
				MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() * 2));
			}
			else {
				DrawText("fails to counter!", 10, 140, 20, GREEN);
				MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() * 2));
			}
			break;
		}
		break;
	case DEFEND:
		switch (MainEnemyAction) {
		case ATTACK:
			DrawText(("You block " + MainEnemy.GetName() + "'s attack!").c_str(), 10, 120, 20, LIGHTGRAY);
			MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() / 2));
			break;
		case DEFEND:
			DrawText(("You and " + MainEnemy.GetName() + " are recovering Stamina.").c_str(), 10, 120, 20, YELLOW);
			break;
		case PARRY:
			DrawText((MainEnemy.GetName() + " misreads your tell and drains stamina while you recover!").c_str(), 10, 120, 20, GREEN);
			break;
		}
		break;
	case PARRY:
		switch (MainEnemyAction) {
		case ATTACK:
			DrawText(("You parry " + MainEnemy.GetName() + "'s attack and ").c_str(), 10, 120, 20, LIGHTGRAY);
			if (CheckParrySuccess(10 - RoundNumber, MainPlayer.GetStamina())) {
				DrawText("counter!", 10, 140, 20, GREEN);
				MainEnemy.UpdateHealth(-(MainPlayer.GetAttackPower() * 2));
			}
			else {
				DrawText("fail to counter!", 10, 140, 20, RED);
				MainPlayer.UpdateHealth(-(MainEnemy.GetAttackPower() * 2));
			}
			break;
		case DEFEND:
			DrawText(("You misread the tell and drain your stamina, " + MainEnemy.GetName() + " recovers!").c_str(), 10, 120, 20, YELLOW);
			break;
		case PARRY:
			DrawText(("You and " + MainEnemy.GetName() + " both try to parry, and drain stamina!").c_str(), 10, 120, 20, LIGHTGRAY);
			break;
		}
		break;
	}
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
    DrawText((MainEnemy.GetName() + " dropped: " + ItemName + "!").c_str(), 190, 380, 20, DARKGREEN);
    MainPlayer.AddInventoryItem(NewItem);

}
