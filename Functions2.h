#pragma once
//отсутствие обработки сообщений при вызове команд
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