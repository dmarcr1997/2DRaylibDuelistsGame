#pragma once
#include <string>
#include <vector>

enum LogType {
	L_LOG_DEFAULT,
	L_LOG_INFO,
	L_LOG_WARNING,
	L_LOG_ERROR
};

struct LogEntry
{
	LogType Type;
	std::string Message;
};

class Log
{
private:
	static LogEntry ConstructLog(LogType Type, const std::string& Message);
public:
	static std::vector<LogEntry> LogEntries;
	static void LogMessage(LogType Type, const std::string& Message);
	static void SpacedMessage(LogType Type, const std::string& Message);
	static void PrintLog(LogEntry& Entry);
	static void Line();
	static void NewLine();
};