#include "Character.h"
#include "CombatLog.h"
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
    Health += Amount;
}

void Character::UpdateStamina(bool Increase, bool Heavy)
{
	int change = 1;
	if (Heavy) change = 2;
	Stamina = (Increase) ? Stamina +change : Stamina -change;
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
	case 3:
		UpdateHealth(MaxHealth / 2);
		return HEAL;
	case 4:
		return HEAVY_ATTACK;
	case 5:	
		UpdateStamina(false, true);
		return DODGE;
	default:
		return NONE;
	}
}
