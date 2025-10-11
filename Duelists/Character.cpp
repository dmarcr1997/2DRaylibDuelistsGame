#include "Character.h"
#include "Log.h"
#include "raylib.h"

Character::Character(int health, int attack, int armor, int stamina, std::string name)
	: MaxHealth(health), AttackPower(attack), Armor(armor), MaxStamina(stamina), Name(name)
{
	ResetStats();
}

void Character::UpdateHealth(int Amount)
{
    if (Armor > 0) {
        float armorValue = static_cast<float>(Armor);
        float blockPercent = armorValue / 100.f;
        int blockedAmount = static_cast<int>(Amount * blockPercent);
        Amount -= blockedAmount;

        DrawText(("Armor helped to block " + std::to_string(blockPercent * 100.0f) + "% of the attack").c_str(), 190, 240, 20, GREEN);
    }
    DrawText((Name + " health changed by: " + std::to_string(Amount)).c_str(), 190, 260, 20, GREEN);
    Health += Amount;
    DrawText((Name + " health is now: " + std::to_string(Health < 0 ? 0 : Health)).c_str(), 190, 280, 20, GREEN);
    DrawText((Name + " health changed by: " + std::to_string(Amount)).c_str(), 190, 300, 20, GREEN);
}

void Character::UpdateStamina(bool Increase)
{
	Stamina = (Increase) ? Stamina +1 : Stamina -1;
	if (Stamina > MaxStamina) Stamina = MaxStamina;
	else if (Stamina <= 0) Stamina = 0;
}

void Character::ResetStats()
{
	Health = MaxHealth;
	Stamina = MaxStamina;
}

Action Character::GetActionFromInput(int Input)
{
	switch (Input) {
	case 0:
		return ATTACK;
	case 1:
		UpdateStamina(false);
		return PARRY;
	case 2:
		UpdateStamina(true);
		return DEFEND;
	//TODO: ADD HEAL, DODGE, AND HEAVY ATTACK
	default:
		return NONE;
	}
}
