#pragma once
#include <iostream>
#include <string>
#include <tgbot/tgbot.h>
using namespace std;
using namespace TgBot;

string NoReactionToCommandsForClass(string unreaction);
void NoReactionToCommands(Bot& bot, Message::Ptr message);