#include "Log.h"
#include <iostream>

std::vector<LogEntry> Log::LogEntries;

void Log::LogMessage(LogType Type, const std::string& Message)
{
	LogEntry Entry = ConstructLog(Type, Message);
	PrintLog(Entry);
}

LogEntry Log::ConstructLog(LogType Type, const std::string& Message)
{
	LogEntry Entry;
	Entry.Type = Type;
	Entry.Message = Message;
	LogEntries.push_back(Entry);
	return Entry;
}

void Log::SpacedMessage(LogType Type, const std::string& Message)
{
	std::string SpacedString = '\n' + Message;
	LogEntry Entry = ConstructLog(Type, SpacedString);
	PrintLog(Entry);
}

void Log::PrintLog(LogEntry& Entry)
{
	std::string color;
	switch (Entry.Type) {
		case L_LOG_INFO:
			color = "\x1B[32m";
			break;
		case L_LOG_WARNING:
			color = "\x1B[33m";
			break;
		case L_LOG_ERROR:
			color = "\x1B[91m";
			break;
		default:
			color = "\x1B[37m";
			break;
	}
	std::cout << color << Entry.Message << std::endl;
}

void Log::Line()
{
	std::cout << "\x1B[37m------------------------------------------\n";
}

void Log::NewLine()
{
	std::cout << std::endl;
}
