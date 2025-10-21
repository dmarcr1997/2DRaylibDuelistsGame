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
	std::function<void(Player& player, Enemy& enemy)> StatsAdjustments;
};

bool CheckParrySuccess(int difficulty, int stamina);
void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction, int RoundNumber);
void DrawOutcome(Player& MainPlayer, Enemy& MainEnemy, Action& PlayerAction, GameState& State, int& RoundNumber, int& WaitDuration);
void DrawLootOutcome(Enemy& MainEnemy, int& RoundNumber, Player& MainPlayer, GameState& State);
void FindItem(Player& MainPlayer, Enemy& MainEnemy, int RoundNumber);
void DrawWaitForInput(Action& PlayerAction, GameState& State, Player& MainPlayer);
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
	Player MainPlayer(5, 2, 10, 2, "Hero");
	MainPlayer.AddTextureSprite("../SourceArt/Characters/Knight/Knight_IdleBlinking_Sprite.png");
	MainPlayer.AddTextureSprite("../SourceArt/Characters/Knight/Knight_Attacking_Sprite.png");
	MainPlayer.AddTextureSprite("../SourceArt/Characters/Knight/Knight_Defend_Sprite.png");
	MainPlayer.AddTextureSprite("../SourceArt/Characters/Knight/Knight_Parry_Sprite.png");

	//Player Sprite Testing
	Vector2 PlayerPosition = { 250.0f, 150.0f };
	Rectangle PlayerRect = { 0.0f, 0.0f, static_cast<float>(MainPlayer.GetCurrentTexture().width / 4), static_cast<float>(MainPlayer.GetCurrentTexture().height / 3)};


	//Enemy init
	Enemy MainEnemy(1, 1, 0, 1, "Goblin");
	MainEnemy.IncreaseDifficulty(1);

	//Player Sprite 
	Vector2 EnemyPosition = {350.0f, 160.0f };
	Rectangle EnemyRect = { 0.0f, 0.0f, static_cast<float>(MainEnemy.GetCurrentTexture().width / 4), static_cast<float>(MainEnemy.GetCurrentTexture().height / 3) };
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
			PlayerRect.x = static_cast<float>(CurrentFrame) *MainPlayer.GetCurrentTexture().width / 4;
			PlayerRect.y = static_cast<float>(CurrentFrame) *MainPlayer.GetCurrentTexture().height / 3;

			EnemyRect.x = static_cast<float>(CurrentFrame) * MainEnemy.GetCurrentTexture().width / 4;
			EnemyRect.y = static_cast<float>(CurrentFrame) * MainEnemy.GetCurrentTexture().height / 3;

		}
		BeginDrawing();
		ClearBackground(BLACK);
		DrawTexture(Background, 0, 0, WHITE);
		DrawTextureRec(MainPlayer.GetCurrentTexture(), PlayerRect, PlayerPosition, WHITE);
		DrawTextureRec(MainEnemy.GetCurrentTexture(), EnemyRect, EnemyPosition, WHITE);

		CombatLog::DrawMessage();
		
		if (State == GAME_OVER) {
			DrawText("Thanks for playing!\n", 190, 200, 20, GREEN);
			DrawText("Game Over. Press ESC to exit.", 190, 220, 20, LIGHTGRAY);
			EndDrawing();
			continue;
		}

		
		std::string RoundMessage = "ROUND " + std::to_string(RoundNumber) + "\t|\tHEALTH: " + std::to_string(MainPlayer.GetHealth()) + "\t|\tSTAMINA: " + std::to_string(MainPlayer.GetStamina());
		DrawText(RoundMessage.c_str(), 10, 10, 20, WHITE);
		
		if (State == WAITING_FOR_INPUT) {
			DrawWaitForInput(PlayerAction, State, MainPlayer);
		} else if(State == PROCESSING) {
			DrawOutcome(MainPlayer, MainEnemy, PlayerAction, State, RoundNumber, WaitDuration);
		}

		if (CAN_INPUT) {
			MainPlayer.Sprites.CurrentSprite = 0;
			MainEnemy.Sprites.CurrentSprite = 0;		
		}

		EndDrawing();
	}
	UnloadTexture(Background);
	MainPlayer.UnloadAllTextures();
	MainEnemy.UnloadAllTextures();
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

void DrawWaitForInput(Action& PlayerAction, GameState& State, Player& MainPlayer)
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
			MainPlayer.Sprites.CurrentSprite = 1;
			break;
		case KEY_TWO:
			if (MainPlayer.GetStamina() > 0) {
				MainPlayer.UpdateStamina(false);
				PlayerAction = PARRY;
				State = PROCESSING;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;

				MainPlayer.Sprites.CurrentSprite = 3;
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
			MainPlayer.UpdateStamina(true);
			PlayerAction = DEFEND;
			State = PROCESSING;
			CAN_INPUT = false;
			TIME_SINCE_LAST_INPUT = 0.0f;

			MainPlayer.Sprites.CurrentSprite = 2;
			break;
		case KEY_FOUR:
			MainPlayer.UpdateHealth(MainPlayer.GetHealth() / 2);
			PlayerAction = HEAL;
			State = PROCESSING;
			CAN_INPUT = false;
			TIME_SINCE_LAST_INPUT = 0.0f;

			MainPlayer.Sprites.CurrentSprite = 2;
			break;
		case KEY_FIVE:
			if (MainPlayer.GetStamina() > 0) {
				MainPlayer.UpdateStamina(false);
				PlayerAction = HEAVY_ATTACK;
				State = PROCESSING;
				CAN_INPUT = false;	
				TIME_SINCE_LAST_INPUT = 0.0f;

				MainPlayer.Sprites.CurrentSprite = 1;
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
			if (MainPlayer.GetStamina() > 0) {
				MainPlayer.UpdateStamina(false, true);
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

void DrawOutcome(Player& MainPlayer, Enemy& MainEnemy, Action& PlayerAction, GameState& State, int& RoundNumber, int& WaitDuration)
{
	ProcessOutcome(MainPlayer, MainEnemy, PlayerAction, RoundNumber);
	State = WAITING_FOR_INPUT;
	if (!MainEnemy.IsAlive()) {
		DrawLootOutcome(MainEnemy, RoundNumber, MainPlayer, State);
		return;
	}

	if (!MainPlayer.IsAlive()) {
		CombatLog::AddMessage("You have been defeated! Game Over!", RED, 3.0f);
		//DrawText("You have been defeated! Game Over!\n", 400, 300, 20, RED);
		State = GAME_OVER;
	}
}

void DrawLootOutcome(Enemy& MainEnemy, int& RoundNumber, Player& MainPlayer, GameState& State)
{
	std::string EnemyDefeatMessage = "You have defeated " + MainEnemy.GetName() + "!";
	CombatLog::AddMessage(EnemyDefeatMessage, GREEN, LONG_MESSAGE_DURATION);
	RoundNumber++;
	FindItem(MainPlayer, MainEnemy, RoundNumber);
	if (RoundNumber > 4) {
		CombatLog::AddMessage("You have defeated all enemies! You win!", GREEN, LONG_MESSAGE_DURATION);
		State = GAME_OVER;
	}
	else {
		MainEnemy.IncreaseDifficulty(RoundNumber);

		MainPlayer.ResetStats();
		CombatLog::AddMessage("You recover your stamina and energy for the next fight!", LIGHTGRAY, LONG_MESSAGE_DURATION);

		CombatLog::AddMessage(("Now facing: " + MainEnemy.GetName() +
			" (Health: " + std::to_string(MainEnemy.GetHealth()) +
			", Attack: " + std::to_string(MainEnemy.GetAttackPower()) + ")\n").c_str(), LIGHTGRAY, LONG_MESSAGE_DURATION);
		State = WAITING_FOR_INPUT;
	}
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction, int RoundNumber)
{
	Action MainEnemyAction = MainEnemy.ChooseAction();

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
	std::string MainEnemyActionStr = GetActionString(MainEnemyAction);
	
	CombatLog::AddMessage(("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + MainEnemyActionStr + "s"), LIGHTGRAY, SHORT_MESSAGE_DURATION);
	//DrawText(("Player " + PlayerActionStr + "s \t | \t" + "Enemy " + MainEnemyActionStr + "s").c_str(),  10, 10, 20, LIGHTGRAY);

	const std::map<std::pair<Action, Action>, CombatOutcome> OutcomeMap = {
		{ {ATTACK, ATTACK}, {"Its a clash! The weapons ring as they hit eachother!", DARKGRAY, [](Player& player , Enemy& enemy) {}}},
		{ {ATTACK, DEFEND}, {MainEnemy.GetName() + " blocks your attack!", RED, [](Player& player , Enemy& enemy) {
			enemy.UpdateHealth(-(player.GetAttackPower() / 2));
		}} },
		{ {ATTACK, PARRY}, {MainEnemy.GetName() + " parries your attack and ", LIGHTGRAY, [RoundNumber](Player& player , Enemy& enemy) {
			if (CheckParrySuccess(10 - (RoundNumber + 1), enemy.GetStamina())) {
				CombatLog::AddMessage("counters!", RED, SHORT_MESSAGE_DURATION);
				player.UpdateHealth(-(enemy.GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy.UpdateHealth(-(player.GetAttackPower() * 2));
			}
		}} },
		{ {ATTACK, HEAL}, {"You strike while " + MainEnemy.GetName() + " heals!", GREEN, [](Player& player , Enemy& enemy) {
			enemy.UpdateHealth(-(player.GetAttackPower()));
		}} },
		{ {ATTACK, HEAVY_ATTACK}, {MainEnemy.GetName() + "'s Heavy Attack overpowers your attack!", RED, [](Player& player , Enemy& enemy) {
			player.UpdateHealth(-(enemy.GetAttackPower() / 2));
		}} },
		{ {ATTACK, DODGE}, {MainEnemy.GetName() + " dodges your attack!", YELLOW, [](Player& player , Enemy& enemy) {}} },
		{ {DEFEND, ATTACK}, {"You block " + MainEnemy.GetName() + "'s attack!", GREEN, [](Player& player , Enemy& enemy) {
			player.UpdateHealth(-(enemy.GetAttackPower() / 2));
		}} },
		{ {DEFEND, DEFEND}, {"You and " + MainEnemy.GetName() + " are recovering Stamina.", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {DEFEND, PARRY}, {MainEnemy.GetName() + " misreads your tell and drains stamina while you recover!", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {DEFEND, HEAL}, {"You block while " + MainEnemy.GetName() + " heals.", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {DEFEND, HEAVY_ATTACK}, {MainEnemy.GetName() + "'s Heavy Attack crushes your defense!", RED, [](Player& player , Enemy& enemy) {
			player.UpdateHealth(-(enemy.GetAttackPower()));
		}} },
		{ {DEFEND, DODGE}, {MainEnemy.GetName() + " misread the tell and drains stamina while you recover!", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {PARRY, ATTACK}, {"You parry " + MainEnemy.GetName() + "'s attack and ", LIGHTGRAY, [RoundNumber](Player& player , Enemy& enemy) {
			if (CheckParrySuccess(10 - RoundNumber, player.GetStamina())) {
				CombatLog::AddMessage("counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy.UpdateHealth(-(player.GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fail to counter!", RED, SHORT_MESSAGE_DURATION);
				player.UpdateHealth(-(enemy.GetAttackPower() * 2));
			}
		}} },
		{ {PARRY, DEFEND}, {"You misread the tell and drain your stamina, " + MainEnemy.GetName() + " recovers!", YELLOW, [](Player& player , Enemy& enemy) {}} },
		{ {PARRY, PARRY}, {"You and " + MainEnemy.GetName() + " both try to parry, and drain stamina!", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {PARRY, HEAL}, {"You misread the tell and drain your stamina, " + MainEnemy.GetName() + " heals!", YELLOW, [](Player& player , Enemy& enemy) {}} },
		{ {PARRY, HEAVY_ATTACK}, {"You parry " + MainEnemy.GetName() + "'s heavy attack and ", LIGHTGRAY, [RoundNumber](Player& player , Enemy& enemy) {
			if (CheckParrySuccess(6 - RoundNumber, enemy.GetStamina())) {
				CombatLog::AddMessage("counter!", RED, SHORT_MESSAGE_DURATION);
				player.UpdateHealth(-(enemy.GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy.UpdateHealth(-(player.GetAttackPower() * 2));
			}
		}} },
		{ {PARRY, DODGE}, {"You parry and " + MainEnemy.GetName() + " dodges. You both drain stamina!", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {HEAL, ATTACK}, {MainEnemy.GetName() + " attacks while you heal!", RED, [](Player& player , Enemy& enemy) {
			player.UpdateHealth(-(enemy.GetAttackPower()));
		}} },
		{ {HEAL, DEFEND}, {"You Heal and " + MainEnemy.GetName() + " recover.", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {HEAL, PARRY}, {MainEnemy.GetName() + " misreads the tell and drains stamina while you heal.", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {HEAL, HEAL}, {"You and the enemy both heal.", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {HEAL, HEAVY_ATTACK}, {MainEnemy.GetName() + "'s Heavy Attack crushes you while you heal!", RED, [](Player& player , Enemy& enemy) {
			player.UpdateHealth(-(enemy.GetAttackPower() * 2));
		}} },
		{ {HEAL, DODGE}, {MainEnemy.GetName() + " misread the tell drains stamina while you heal ", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {HEAVY_ATTACK, ATTACK}, {"Your heavy attack overpowers the attack of " + MainEnemy.GetName(), GREEN, [](Player& player , Enemy& enemy) {
			enemy.UpdateHealth(-(player.GetAttackPower() / 2));
		}} },
		{ {HEAVY_ATTACK, DEFEND}, {"Your heavy attack crushes the defense of " + MainEnemy.GetName(), GREEN, [](Player& player , Enemy& enemy) {
			enemy.UpdateHealth(-(player.GetAttackPower()));
		}} },
		{ {HEAVY_ATTACK, PARRY}, {MainEnemy.GetName() + " parries your heavy attack and ", LIGHTGRAY, [RoundNumber](Player& player , Enemy& enemy) {
			if (CheckParrySuccess(6 - RoundNumber, enemy.GetStamina())) {
				CombatLog::AddMessage("counters!", RED, SHORT_MESSAGE_DURATION);
				player.UpdateHealth(-(enemy.GetAttackPower() * 2));
			}
			else {
				CombatLog::AddMessage("fails to counter!", GREEN, SHORT_MESSAGE_DURATION);
				enemy.UpdateHealth(-(player.GetAttackPower() * 2));
			}
		}} },
		{ {HEAVY_ATTACK, HEAL}, {"You strike while " + MainEnemy.GetName() + " heals!", GREEN, [](Player& player , Enemy& enemy) {
			enemy.UpdateHealth(-(player.GetAttackPower() * 2));
		}} },
		{ {HEAVY_ATTACK, HEAVY_ATTACK}, {"Both heavy attacks collide with a massive shockwave!", YELLOW, [](Player& player , Enemy& enemy) {
			player.UpdateStamina(false, true);
			enemy.UpdateStamina(false, true);
		}} },
		{ {HEAVY_ATTACK, DODGE}, {MainEnemy.GetName() + " dodged your heavy attack!", YELLOW, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, ATTACK}, {"You dodged " + MainEnemy.GetName() + "'s  attack", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, DEFEND}, {"You misread the tell and drain your stamina, " + MainEnemy.GetName() + " recovers!", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, PARRY}, {"You dodge and " + MainEnemy.GetName() + " tries to parry. You both drain stamina!", LIGHTGRAY, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, HEAL}, {"You misread the tell and drain your stamina, " + MainEnemy.GetName() + " heals!", YELLOW, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, HEAVY_ATTACK}, {"You dodged " + MainEnemy.GetName() + "'s  heavy attack", GREEN, [](Player& player , Enemy& enemy) {}} },
		{ {DODGE, DODGE}, {"You both dodge and drain stamina!", YELLOW, [](Player& player , Enemy& enemy) {
			player.UpdateStamina(false, true);
			enemy.UpdateStamina(false, true);
		}} }
	};
	
	CombatOutcome outcome = OutcomeMap.at(std::make_pair(PlayerAction, MainEnemyAction));
	CombatLog::AddMessage(outcome.OutcomeText, outcome.TextColor, SHORT_MESSAGE_DURATION);
	//DrawText(outcome.OutcomeText.c_str(), 10, 300, 20, outcome.TextColor);
	outcome.StatsAdjustments(MainPlayer, MainEnemy);
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
    DrawText((MainEnemy.GetName() + " dropped: " + ItemName + "!").c_str(), 10, 380, 20, YELLOW);
    MainPlayer.AddInventoryItem(NewItem);
}
