#pragma once
#include <string>
#include <vector>

enum LogType {
	LOG_DEFAULT,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
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