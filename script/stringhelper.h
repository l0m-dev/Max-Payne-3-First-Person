#pragma once
#include "stdafx.h"
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> ParseCommand(std::string Command)
{
	//std::string str(Command);
	std::string buf;                 // Have a buffer string
	std::stringstream ss(Command);       // Insert the string into a stream

	std::vector<std::string> tokens; // Create vector to hold our words

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}