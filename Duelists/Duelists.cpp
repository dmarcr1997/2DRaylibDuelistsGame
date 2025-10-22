#include <iostream>
#include <random>
#include <map>
#include <functional>
#include "Player.h"
#include "Enemy.h"
#include "CombatLog.h"
#include "InventoryItem.h"
#include "raylib.h"

float SHORT_MESSAGE_DURATION = 3.0f;
float LONG_MESSAGE_DURATION = 6.0f;
bool CAN_INPUT = true;
float INPUT_COOLDOWN = 3.0f;
float TIME_SINCE_LAST_INPUT = 0.0f;

enum GameState {
	WAITING_FOR_INPUT,
	PROCESSING,
	GAME_OVER,
};

struct CombatOutcome
{
	std::string OutcomeText;
	Color TextColor;
	std::function<void(Player* player, Enemy* enemy)> StatsAdjustments;
};

bool CheckParrySuccess(int difficulty, int stamina);
void ProcessOutcome(Player* playerPtr, Enemy* enemyPtr, Action PlayerAction, int RoundNumber);
void DrawOutcome(Player* playerPtr, Enemy* enemyPtr, Action& PlayerAction, GameState& State, int& RoundNumber, int& WaitDuration);
void DrawLootOutcome(Enemy* enemyPtr, int& RoundNumber, Player* playerPtr, GameState& State);
void FindItem(Player* playerPtr, Enemy* enemyPtr, int RoundNumber);
void DrawWaitForInput(Action& PlayerAction, GameState& State, Player* playerPtr);
void DrawFirstFrame(Texture2D texture, int posX, int posY, bool flipped);


int main()
{
	//Raylib init
	const Vector2 screenDims = { 800, 600 };
	InitWindow(
		screenDims.x,
		screenDims.y,
		"Dueling Game"
	);

	Texture2D Background = LoadTexture("../SourceArt/Arena.png");
	
	int FramesCounter = 0;
	int CurrentFrame = 0;
	int FramesSpeed = 6;

	SetTargetFPS(60);

	//Player init
	Player* playerPtr = new Player(5, 2, 10, 2, "Hero");
	playerPtr->AddTextureSprite("../SourceArt/Characters/Knight/Knight_IdleBlinking_Sprite.png");
	playerPtr->AddTextureSprite("../SourceArt/Characters/Knight/Knight_Attacking_Sprite.png");
	playerPtr->AddTextureSprite("../SourceArt/Characters/Knight/Knight_Defend_Sprite.png");
	playerPtr->AddTextureSprite("../SourceArt/Characters/Knight/Knight_Parry_Sprite.png");

	//Player Sprite Testing
	Vector2 PlayerPosition = { 250.0f, 150.0f };
	Rectangle PlayerRect = { 0.0f, 0.0f, static_cast<float>(playerPtr->GetCurrentTexture().width / 4), static_cast<float>(playerPtr->GetCurrentTexture().height / 3)};


	//Enemy init
	Enemy* enemyPtr = new Enemy(1, 1, 0, 1, "Goblin");
	enemyPtr->IncreaseDifficulty(1);

	//Player Sprite 
	Vector2 EnemyPosition = {350.0f, 160.0f };
	Rectangle EnemyRect = { 0.0f, 0.0f, static_cast<float>(enemyPtr->GetCurrentTexture().width / 4), static_cast<float>(enemyPtr->GetCurrentTexture().height / 3) };
	EnemyRect.width = -EnemyRect.width;

	int RoundNumber = 1;
	int WaitDuration = 3;
	GameState State = WAITING_FOR_INPUT;

	Action PlayerAction = NONE;

	while (!WindowShouldClose()) {
		FramesCounter++;
		TIME_SINCE_LAST_INPUT += GetFrameTime();

		if (FramesCounter >= (60 / FramesSpeed)) {
			FramesCounter = 0;
			CurrentFrame++;
			if (CurrentFrame > 3)
			{
				CurrentFrame = 0;

			}
			PlayerRect.x = static_cast<float>(CurrentFrame) *playerPtr->GetCurrentTexture().width / 4;
			PlayerRect.y = static_cast<float>(CurrentFrame) *playerPtr->GetCurrentTexture().height / 3;

			EnemyRect.x = static_cast<float>(CurrentFrame) * enemyPtr->GetCurrentTexture().width / 4;
			EnemyRect.y = static_cast<float>(CurrentFrame) * enemyPtr->GetCurrentTexture().height / 3;

		}
		BeginDrawing();
		ClearBackground(BLACK);
		DrawTexture(Background, 0, 0, WHITE);
		DrawTextureRec(playerPtr->GetCurrentTexture(), PlayerRect, PlayerPosition, WHITE);
		DrawTextureRec(enemyPtr->GetCurrentTexture(), EnemyRect, EnemyPosition, WHITE);

		CombatLog::DrawMessage();
		
		if (State == GAME_OVER) {
			DrawText("Thanks for playing!\n", 190, 200, 20, GREEN);
			DrawText("Game Over. Press ESC to exit.", 190, 220, 20, LIGHTGRAY);
			EndDrawing();
			continue;
		}

		
		std::string RoundMessage = "ROUND " + std::to_string(RoundNumber) + "\t|\tHEALTH: " + std::to_string(playerPtr->GetHealth()) + "\t|\tSTAMINA: " + std::to_string(playerPtr->GetStamina());
		DrawText(RoundMessage.c_str(), 10, 10, 20, WHITE);
		
		if (State == WAITING_FOR_INPUT) {
			DrawWaitForInput(PlayerAction, State, playerPtr);
		} else if(State == PROCESSING) {
			DrawOutcome(playerPtr, enemyPtr, PlayerAction, State, RoundNumber, WaitDuration);
		}

		if (CAN_INPUT) {
			playerPtr->Sprites.CurrentSprite = 0;
			enemyPtr->Sprites.CurrentSprite = 0;		
		}

		EndDrawing();
	}
	UnloadTexture(Background);
	playerPtr->UnloadAllTextures();
	enemyPtr->UnloadAllTextures();
	delete playerPtr;
	delete enemyPtr;
	return 0;
}

void DrawFirstFrame(Texture2D texture, int posX, int posY, bool flipped) {
    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 3;
    Rectangle sourceRec = { 0, 0, (float)frameWidth, (float)frameHeight };
    Vector2 position = { (float)posX, (float)posY };
	if (flipped) {
		sourceRec.x = (float)frameWidth; // start at right edge
		sourceRec.width = -(float)frameWidth; // negative width flips horizontally
		Vector2 enemyPosition = { 600.0f, 100.0f };
	}
    DrawTextureRec(texture, sourceRec, position, WHITE);
}

void DrawWaitForInput(Action& PlayerAction, GameState& State, Player* playerPtr)
{
	if (State != WAITING_FOR_INPUT) return;

	DrawText("Choose an Action - \n(1. Attack, 2. Parry, 3. Defend, 4. Heal, 5. Heavy Attack, 6. Dodge)", 10, 30, 20, LIGHTGRAY);
	if (CAN_INPUT) {
		switch (GetKeyPressed())
		{
		case KEY_ONE:
			PlayerAction = ATTACK;
			State = PROCESSING;
			CAN_INPUT = false;
			TIME_SINCE_LAST_INPUT = 0.0f;
			playerPtr->Sprites.CurrentSprite = 1;
			break;
		case KEY_TWO:
			if (playerPtr->GetStamina() > 0) {
				playerPtr->UpdateStamina(false);
				PlayerAction = PARRY;
				State = PROCESSING;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;

				playerPtr->Sprites.CurrentSprite = 3;
			}
			else {
				CombatLog::AddMessage("You are Exhausted : You cannot Parry until you Defend(3)", RED, SHORT_MESSAGE_DURATION);
				//DrawText("You are Exhausted: You cannot Parry until you Defend(3)", 10, 300, 20, RED);
				State = WAITING_FOR_INPUT;
				CAN_INPUT = false;
				TIME_SINCE_LAST_INPUT = 0.0f;
			}
			break;
		case KEY_THREE:
			playerPtr->UpdateStamina(true);
			PlayerAction = DEFEND;
			State = PROCESSING;
			CAN_INPUT = false;
			TIME_SINCE_LAST_INPUT = 0.0f;

			playerPtr->Sprites.CurrentSprite = 2;
			break;
		case KEY_FOUR:
			playerPtr->UpdateHealth(playerPtr->GetHealth() / 2);
			PlayerAction = HEAL;
			State = PROCESSING;
			CAN_INPUT = false;
			TIME_SINCE_LAST_INPUT = 0.0f;

			playerPtr->Sprites.CurrentSprite = 2;
			break;
		case KEY_FIVE:
			if (playerPtr->GetStamina() > 0) {
				playerPtr->UpdateStamina(false);
				PlayerAction = HEAVY_ATTACK;
				State = PROCESSING;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;

				playerPtr->Sprites.CurrentSprite = 1;
			}
			else {

				CombatLog::AddMessage("You are Exhausted : You cannot Heavy Attack until you Defend(3)", RED, SHORT_MESSAGE_DURATION);
				//DrawText("You are Exhausted: You cannot do that until you Defend(3)", 10, 300, 20, RED);
				State = WAITING_FOR_INPUT;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;
			}
			break;
		case KEY_SIX:
			if (playerPtr->GetStamina() > 0) {
				playerPtr->UpdateStamina(false, true);
				PlayerAction = DODGE;
				State = PROCESSING;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;

				//DODGE HANDLE (MOVE PLAYER SPRITE)
			}
			else {

				CombatLog::AddMessage("You are Exhausted : You cannot Dodge until you Defend(3)", RED, SHORT_MESSAGE_DURATION);
				//DrawText("You are Exhausted: You cannot do that until you Defend(3)", 10, 300, 20, RED);
				State = WAITING_FOR_INPUT;
				TIME_SINCE_LAST_INPUT = 0.0f;
			}
			break;
		default:
			break;
		}
	}
	if(TIME_SINCE_LAST_INPUT > INPUT_COOLDOWN) {
		CAN_INPUT = true;	
	}
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

void DrawOutcome(Player* playerPtr, Enemy* enemyPtr, Action& PlayerAction, GameState& State, int& RoundNumber, int& WaitDuration)
{
	ProcessOutcome(playerPtr, enemyPtr, PlayerAction, RoundNumber);
	State = WAITING_FOR_INPUT;
	if (!enemyPtr->IsAlive()) {
		DrawLootOutcome(enemyPtr, RoundNumber, playerPtr, State);
		return;
	}

	if (!playerPtr->IsAlive()) {
		CombatLog::AddMessage("You have been defeated! Game Over!", RED, 3.0f);
		//DrawText("You have been defeated! Game Over!\n", 400, 300, 20, RED);
		State = GAME_OVER;
	}
}

void DrawLootOutcome(Enemy* enemyPtr, int& RoundNumber, Player* playerPtr, GameState& State)
{
	std::string EnemyDefeatMessage = "You have defeated " + enemyPtr->GetName() + "!";
	CombatLog::AddMessage(EnemyDefeatMessage, GREEN, LONG_MESSAGE_DURATION);
	RoundNumber++;
	FindItem(playerPtr, enemyPtr, RoundNumber);
	if (RoundNumber > 4) {
		CombatLog::AddMessage("You have defeated all enemies! You win!", GREEN, LONG_MESSAGE_DURATION);
		State = GAME_OVER;
	}
	else {
		enemyPtr->IncreaseDifficulty(RoundNumber);

		playerPtr->ResetStats();
		CombatLog::AddMessage("You recover your stamina and energy for the next fight!", LIGHTGRAY, LONG_MESSAGE_DURATION);

		CombatLog::AddMessage(("Now facing: " + enemyPtr->GetName() +
			" (Health: " + std::to_string(enemyPtr->GetHealth()) +
			", Attack: " + std::to_string(enemyPtr->GetAttackPower()) + ")\n").c_str(), LIGHTGRAY, LONG_MESSAGE_DURATION);
		State = WAITING_FOR_INPUT;
	}
}

void ProcessOutcome(Player* playerPtr, Enemy* enemyPtr, Action PlayerAction, int RoundNumber)
{
	Action enemyPtrAction = enemyPtr->ChooseAction();

	auto GetActionString = [](Action action) -> std::string {
		switch (action) {
			case ATTACK: return "Attack";
			case DEFEND: return "Defend";
			case PARRY: return "Parry";
			case HEAL: return "Heal";
			case HEAVY_ATTACK: return "Heavy Attack";
			case DODGE: return "Dodge";
			default: return "None";
		}
	};

	std::string PlayerActionStr = GetActionString(PlayerAction);
	std::string enemyPtrActionStr = GetActionString(enemyPtrAction);
	
	CombatLog::AddMessage(("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + enemyPtrActionStr + "s"), LIGHTGRAY, SHORT_MESSAGE_DURATION);
	//DrawText(("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + enemyPtrActionStr + "s").c_str(),  10, 10, 20, LIGHTGRAY);

	const std::map<std::pair<Action, Action>, CombatOutcome> OutcomeMap = {
		{ {ATTACK, ATTACK}, {"Its a clash! The weapons ring as they hit eachother!", DARKGRAY, [](Player* player , Enemy* enemy) {}}},
		{ {ATTACK, DEFEND}, {enemyPtr->GetName() + " blocks your attack!", RED, [](Player* player , Enemy* enemy) {
			enemy->UpdateHealth(-(player->GetAttackPower() / 2));
		}} },
		{ {ATTACK, PARRY}, {enemyPtr->GetName() + " parries your attack and ", LIGHTGRAY, [RoundNumber](Player* player , Enemy* enemy) {
			if (CheckParrySuccess(10 - (RoundNumber + 1), enemy->GetStamina())) {
				CombatLog::AddMessage("counters!", RED, SHORT_MESSAGE_DURATION);
				player->UpdateHealth(-(enemy->GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy->UpdateHealth(-(player->GetAttackPower() * 2));
			}
		}} },
		{ {ATTACK, HEAL}, {"You strike while " + enemyPtr->GetName() + " heals!", GREEN, [](Player* player , Enemy* enemy) {
			enemy->UpdateHealth(-(player->GetAttackPower()));
		}} },
		{ {ATTACK, HEAVY_ATTACK}, {enemyPtr->GetName() + "'s Heavy Attack overpowers your attack!", RED, [](Player* player , Enemy* enemy) {
			player->UpdateHealth(-(enemy->GetAttackPower() / 2));
		}} },
		{ {ATTACK, DODGE}, {enemyPtr->GetName() + " dodges your attack!", YELLOW, [](Player* player , Enemy* enemy) {}} },
		{ {DEFEND, ATTACK}, {"You block " + enemyPtr->GetName() + "'s attack!", GREEN, [](Player* player , Enemy* enemy) {
			player->UpdateHealth(-(enemy->GetAttackPower() / 2));
		}} },
		{ {DEFEND, DEFEND}, {"You and " + enemyPtr->GetName() + " are recovering Stamina.", LIGHTGRAY, [](Player* player , Enemy* enemy) {}} },
		{ {DEFEND, PARRY}, {enemyPtr->GetName() + " misreads your tell and drains stamina while you recover!", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {DEFEND, HEAL}, {"You block while " + enemyPtr->GetName() + " heals.", LIGHTGRAY, [](Player* player , Enemy* enemy) {}} },
		{ {DEFEND, HEAVY_ATTACK}, {enemyPtr->GetName() + "'s Heavy Attack crushes your defense!", RED, [](Player* player , Enemy* enemy) {
			player->UpdateHealth(-(enemy->GetAttackPower()));
		}} },
		{ {DEFEND, DODGE}, {enemyPtr->GetName() + " misread the tell and drains stamina while you recover!", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {PARRY, ATTACK}, {"You parry " + enemyPtr->GetName() + "'s attack and ", LIGHTGRAY, [RoundNumber](Player* player , Enemy* enemy) {
			if (CheckParrySuccess(10 - RoundNumber, player->GetStamina())) {
				CombatLog::AddMessage("counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy->UpdateHealth(-(player->GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fail to counter!", RED, SHORT_MESSAGE_DURATION);
				player->UpdateHealth(-(enemy->GetAttackPower() * 2));
			}
		}} },
		{ {PARRY, DEFEND}, {"You misread the tell and drain your stamina, " + enemyPtr->GetName() + " recovers!", YELLOW, [](Player* player , Enemy* enemy) {}} },
		{ {PARRY, PARRY}, {"You and " + enemyPtr->GetName() + " both try to parry, and drain stamina!", LIGHTGRAY, [](Player* player , Enemy* enemy) {}} },
		{ {PARRY, HEAL}, {"You misread the tell and drain your stamina, " + enemyPtr->GetName() + " heals!", YELLOW, [](Player* player , Enemy* enemy) {}} },
		{ {PARRY, HEAVY_ATTACK}, {"You parry " + enemyPtr->GetName() + "'s heavy attack and ", LIGHTGRAY, [RoundNumber](Player* player , Enemy* enemy) {
			if (CheckParrySuccess(6 - RoundNumber, enemy->GetStamina())) {
				CombatLog::AddMessage("counter!", RED, SHORT_MESSAGE_DURATION);
				player->UpdateHealth(-(enemy->GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy->UpdateHealth(-(player->GetAttackPower() * 2));
			}
		}} },
		{ {PARRY, DODGE}, {"You parry and " + enemyPtr->GetName() + " dodges. You both drain stamina!", LIGHTGRAY, [](Player* player , Enemy* enemy) {}} },
		{ {HEAL, ATTACK}, {enemyPtr->GetName() + " attacks while you heal!", RED, [](Player* player , Enemy* enemy) {
			player->UpdateHealth(-(enemy->GetAttackPower()));
		}} },
		{ {HEAL, DEFEND}, {"You Heal and " + enemyPtr->GetName() + " recover.", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {HEAL, PARRY}, {enemyPtr->GetName() + " misreads the tell and drains stamina while you heal.", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {HEAL, HEAL}, {"You and the enemy both heal.", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {HEAL, HEAVY_ATTACK}, {enemyPtr->GetName() + "'s Heavy Attack crushes you while you heal!", RED, [](Player* player , Enemy* enemy) {
			player->UpdateHealth(-(enemy->GetAttackPower() * 2));
		}} },
		{ {HEAL, DODGE}, {enemyPtr->GetName() + " misread the tell drains stamina while you heal ", GREEN, [](Player* player , Enemy* enemy) {}} },
		{ {HEAVY_ATTACK, ATTACK}, {"Your heavy attack overpowers the attack of " + enemyPtr->GetName(), GREEN, [](Player* player , Enemy* enemy) {
			enemy->UpdateHealth(-(player->GetAttackPower() / 2));
		}} },
		{ {HEAVY_ATTACK, DEFEND}, {"Your heavy attack crushes the defense of " + enemyPtr->GetName(), GREEN, [](Player* player , Enemy* enemy) {
			enemy->UpdateHealth(-(player->GetAttackPower()));
		}} },
		{ {HEAVY_ATTACK, PARRY}, {enemyPtr->GetName() + " parries your heavy attack and ", LIGHTGRAY, [RoundNumber](Player* player , Enemy* enemy) {
			if (CheckParrySuccess(6 - RoundNumber, enemy->GetStamina())) {
				CombatLog::AddMessage("counters!", RED, SHORT_MESSAGE_DURATION);
				player->UpdateHealth(-(enemy->GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy->UpdateHealth(-(player->GetAttackPower() * 2));
			}
		}} },
		{ {HEAVY_ATTACK, HEAL}, {"You strike while " + enemyPtr->GetName() + " heals!", GREEN, [](Player* player , Enemy* enemy) {
			enemy->UpdateHealth(-(player->GetAttackPower() * 2));
		}} },
		{ {HEAVY_ATTACK, HEAVY_ATTACK}, {"Both heavy attacks collide with a massive shockwave!", YELLOW, [](Player* player , Enemy* enemy) {
			player->UpdateStamina(false, true);
			enemy->UpdateStamina(false, true);
		}} },
		{ {HEAVY_ATTACK, DODGE}, {enemyPtr->GetName() + " dodged your heavy attack!", YELLOW, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, ATTACK}, {"You dodged " + enemyPtr->GetName() + "'s  attack", GREEN, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, DEFEND}, {"You misread the tell and drain your stamina, " + enemyPtr->GetName() + " recovers!", LIGHTGRAY, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, PARRY}, {"You dodge and " + enemyPtr->GetName() + " tries to parry. You both drain stamina!", LIGHTGRAY, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, HEAL}, {"You misread the tell and drain your stamina, " + enemyPtr->GetName() + " heals!", YELLOW, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, HEAVY_ATTACK}, {"You dodged " + enemyPtr->GetName() + "'s  heavy attack", GREEN, [](Player*  player , Enemy* enemy) {}} },
		{ {DODGE, DODGE}, {"You both dodge and drain stamina!", YELLOW, [](Player*  player , Enemy* enemy) {
			player->UpdateStamina(false, true);
			enemy->UpdateStamina(false, true);
		}} }
	};
	
	CombatOutcome outcome = OutcomeMap.at(std::make_pair(PlayerAction, enemyPtrAction));
	CombatLog::AddMessage(outcome.OutcomeText, outcome.TextColor, SHORT_MESSAGE_DURATION);
	//DrawText(outcome.OutcomeText.c_str(), 10, 300, 20, outcome.TextColor);
	outcome.StatsAdjustments(playerPtr, enemyPtr);
}

void FindItem(Player* playerPtr, Enemy* enemyPtr, int RoundNumber)
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
    DrawText((enemyPtr->GetName() + " dropped: " + ItemName + "!").c_str(), 10, 380, 20, YELLOW);
    playerPtr->AddInventoryItem(NewItem);
}
