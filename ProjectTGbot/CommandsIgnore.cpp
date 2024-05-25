#include "CommandsIgnore.h"
#include <string>
#include <tgbot/tgbot.h>


using namespace std;
using namespace TgBot;
string NoReactionToCommandsForClass(string unreaction) {
    if ((unreaction == "/start") ||
        (unreaction == "/MyExpense") ||
        (unreaction == "/MyIncome") ||
        (unreaction == "/TryTest") ||
        (unreaction == "/AddExpense") ||
        (unreaction == "/IncomeSortByAmountIncrease") ||
        (unreaction == "/IncomeSortByData") ||
        (unreaction == "/IncomeSortByCategory") ||
        (unreaction == "/ExpenseSortByAmountIncrease") ||
        (unreaction == "/ExpenseSortByData") ||
        (unreaction == "/ExpenseSortByCategory") ||
        (unreaction == "/AddIncome")) {
        return "you can't use any commands";
    }
    else {
        return "all right";
    }
}

void NoReactionToCommands(Bot& bot, Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/start") ||
        StringTools::startsWith(message->text, "/MyExpense") ||
        StringTools::startsWith(message->text, "/MyIncome") ||
        StringTools::startsWith(message->text, "/TryTest") ||
        StringTools::startsWith(message->text, "/AddExpense") ||
        StringTools::startsWith(message->text, "/IncomeSortByAmountIncrease") ||
        StringTools::startsWith(message->text, "/IncomeSortByData") ||
        StringTools::startsWith(message->text, "/IncomeSortByCategory") ||
        StringTools::startsWith(message->text, "/ExpenseSortByAmountIncrease") ||
        StringTools::startsWith(message->text, "/ExpenseSortByData") ||
        StringTools::startsWith(message->text, "/ExpenseSortByCategory") ||
        StringTools::startsWith(message->text, "/AddIncome")) {
        return;
    }
}