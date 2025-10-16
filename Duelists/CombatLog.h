#pragma once
#include <string>
#include <vector>
#include "raylib.h"

struct CombatMessage
{
	std::string Message;
	Color TextColor;
	float Duration;
	float Timer = 0;
};

class CombatLog
{
private:
	static std::vector<CombatMessage> MessageBuffer;
public:
	static void AddMessage(const std::string& Message, Color TextColor, float Duration);
	static void DrawMessage();
	static void ClearMessages();
};