#include <iostream>
#include <curl/curl.h>
#include <tgbot/tgbot.h>
#include <string>
#include <fstream>
#include <cctype>
#include <vector> 
#include <stdio.h>
#include "CommandsIgnore.h"

#define SQLITECPP_COMPILE_DLL
#include <SQLiteCpp/SQLiteCpp.h>

using namespace std;
using namespace TgBot;

//создание базы данных SQLite
SQLite::Database db("example.db3", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

struct Myfinance {
    string Type; //расходы или доходы
    string Category;
    string Amount;
    string Comment;
    string Data;
};


// Переменная для отслеживания состояния диалога
enum class ExpenseState {
    WAITING_FOR_COMMAND,
    WAITING_FOR_EXPENSECATEGORY,
    WAITING_FOR_EXPENSEAMOUNT,
    WAITING_FOR_EXPENSECOMMENT,
    WAITING_FOR_EXPENSEDATA,
    WAITING_FOR_INCOMECATEGORY,
    WAITING_FOR_INCOMEAMOUNT,
    WAITING_FOR_INCOMECOMMENT,
    WAITING_FOR_INCOMEDATA,
};



int main() {
   setlocale(LC_ALL, "Russian");



    //Запуск бота
    TgBot::Bot bot("7083052113:AAEWnl15pmnb0Vu3CUAuApRfCWpPkZAbGVU");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message)
        {
        bot.getApi().sendMessage(message->chat->id, u8"Привет! \n Я телеграмм бот Financier_Bot");
        bot.getApi().sendMessage(message->chat->id, u8"Вот список команд, которые вы можете использовать: \n 1. /MyExpense - отсортировать ваши расходы и узнать информацию о  них; \n 2. /MyIncome - отсортировать ваши доходы и узнать информацию о  них; \n 3. /AddExpense - добавить ваши расходы и информацию о них (сумму, дату, время); \n 4. /AddIncome - добавить ваши доходы и информацию о них (сумму, дату, время);");
        string ID = to_string(message->chat->id);
        cout << ID << endl;



        //Проверка использовал ли пользователь бота раньше
        try {
            SQLite::Statement query(db, "SELECT COUNT(*) FROM users WHERE IDTg = :userId");
            query.bind(":userId", message->chat->id);
            if (query.executeStep()) {
                int count = query.getColumn(0).getInt();
                if (count > 0) {
                    cout << "ID " << message->chat->id << " found." << endl;
                }
                else {
                    cout << "ID " << message->chat->id << " not found." << endl;
                    SQLite::Statement query(db, "INSERT INTO users (IDTg) VALUES (?)");
                    query.bind(1, message->chat->id);
                    query.exec();
                    }
                }
             } 

        catch (exception& e) {
            cerr << e.what() << endl;
        }

      
        //создание индивидуального файла для пользователя 
        ofstream fout(ID + ".txt", ofstream::app);
        if (!fout.is_open()) {
            cout << "Не удалось открыть файл для записи!" << endl;
            return 1;
        }
        fout.close();
        return 1;
        });







    //обработка команды MyExpense
    bot.getEvents().onCommand("MyExpense", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Выберите, по какому признаку вы хотите сортировать ваши расходы: \n  1. /ExpenseSortByCategory - отсортировать ваши расходы по категории; \n 2. /ExpenseSortByData - отсортировать ваши расходы по дате ; \n 3. /ExpenseSortByAmountIncrease - увидеть всю потраченную на расходы сумму;");
        });


    //обработка команды ExpenseSortByCategory (MyExpense)
    bot.getEvents().onCommand("ExpenseSortByCategory", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Введите категорию чтобы увидеть все расходы которые вы совершили  в ней");
            bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
                NoReactionToCommands(bot, message);

                string CategoryStr = "Category:" + message->text;
                cout << message->text << endl;

                string ID = to_string(message->chat->id);
                vector<string> lines;
                ifstream fin(ID + ".txt");
                if (!fin.is_open()) {
                    cout << "Не удалось открыть файл для чтения!" << endl;
                    return 1;
                }
                else if (fin.peek() == EOF) {
                    bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали расходов в этой категории");
                } else {

                string ForFile;
        
                while (getline(fin, ForFile)) {
                    lines.push_back(ForFile);
                }
                fin.close();
                int Cheking = 0;
                    for (int j = 0; j < lines.size(); j++) {
                        if (lines[j] == CategoryStr && lines[j - 1] == "Type:Expense") {
                            Cheking++;
                            bot.getApi().sendMessage(message->chat->id, u8"Категория: " + lines[j].erase(0, 9) + u8" \n Сумма: " + lines[j + 1].erase(0, 7) + u8"\n Коммантарий: " + lines[j + 2].erase(0, 8) + u8"\n Дата: " + lines[j + 3].erase(0, 5));
                        }
                    }
                    lines.clear();

                }
                });
            
        });







    //обработка команды ExpenseSortByData (MyExpense)
    bot.getEvents().onCommand("ExpenseSortByData", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Выберите дату совершения расхода");
            bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
                NoReactionToCommands(bot, message);

                string CategoryStr = "Data:" + message->text;
                cout << message->text << endl;

                string ID = to_string(message->chat->id);
                vector<string> lines;
                ifstream fin(ID + ".txt");
                if (!fin.is_open()) {
                    cout << "Не удалось открыть файл для чтения!" << endl;
                    return 1;
                }
                else if (fin.peek() == EOF) {
                    bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали расходов в эту дату");
                }
                else {

                    string ForFile;

                    while (getline(fin, ForFile)) {
                        lines.push_back(ForFile);
                    }
                    fin.close();
                    for (int j = 0; j < lines.size(); j++) {
                        if (lines[j] == CategoryStr && lines[j - 4] == "Type:Expense") {
                            bot.getApi().sendMessage(message->chat->id, u8"Категория: " + lines[j - 3].erase(0, 9) + u8" \n Сумма: " + lines[j - 2].erase(0, 7) + u8"\n Коммантарий: " + lines[j - 1].erase(0, 8) + u8"\n Дата: " + lines[j].erase(0, 5));
                        }
                    }
                    lines.clear();

                }
                });
        });










    //обработка команды ExpenseSortByAmountIncrease (MyExpense)
    bot.getEvents().onCommand("ExpenseSortByAmountIncrease", [&bot](TgBot::Message::Ptr message)
        {
                string ID = to_string(message->chat->id);
                vector<string> lines;
                vector<int> numbers;
                ifstream fin(ID + ".txt");
                if (!fin.is_open()) {
                    cout << "Не удалось открыть файл для чтения!" << endl;
                    return 1;
                }
                else if (fin.peek() == EOF) {
                    bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали расходов");
                }
                else {

                    string ForFile;

                    while (getline(fin, ForFile)) {
                        lines.push_back(ForFile);
                    }

                    fin.close();
                    for (int j = 0; j < lines.size(); j++) {
                        if (lines[j].substr(0, 7) == "Amount:" && lines[j - 2] == "Type:Expense") {
                            numbers.push_back(stoi(lines[j].erase(0, 7)));
                        }
                       
                    }
                   int count = 0;
                    for (int j = 0; j < numbers.size(); j++) {
                        count += numbers[j];
                    }
                    bot.getApi().sendMessage(message->chat->id, u8"За всё время вы потратили: " + to_string(count));
                }
                lines.clear();
                });












    
    //обработка команды MyIncome
    bot.getEvents().onCommand("MyIncome", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Выберите, по какому признаку вы хотите сортировать ваши доходы: \n  1. /IncomeSortByCategory - отсортировать ваши доходы по категории; \n 2. /IncomeSortByData - отсортировать ваши доходы по полученной сумме ; \n 3. /IncomeSortByAmountIncrease - увидеть всю полученную сумму на ваши доходы;");
        

        });


    //обработка команды IncomeSortByCategory (MyIncome)
    bot.getEvents().onCommand("IncomeSortByCategory", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Введите категорию чтобы увидеть все доходы которые вы совершили в ней ");
              bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
                  NoReactionToCommands(bot, message);

            string CategoryStr = "Category:" + message->text;
            cout << message->text << endl;

            string ID = to_string(message->chat->id);
            vector<string> lines;
            ifstream fin(ID + ".txt");
            if (!fin.is_open()) {
                cout << "Не удалось открыть файл для чтения!" << endl;
                bot.getApi().sendMessage(message->chat->id, u8"Техническая ошибка, повторите попытку снова, в случае повторения проблемы пишите: https://t.me/Suguru_Geto_4");
                return 1;
            }
            else if (fin.peek() == EOF) {
                bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали доходов в этой категории");
                return 1;
            }
            else {

                string ForFile;

                while (getline(fin, ForFile)) {
                    lines.push_back(ForFile);
                  
                }
                fin.close();
                for (int j = 0; j < lines.size(); j++) {
                    if (lines[j] == CategoryStr && lines[j - 1] == "Type:Income") {
                        bot.getApi().sendMessage(message->chat->id, u8"Категория: " + lines[j].erase(0, 9) + u8" \n Сумма: " + lines[j + 1].erase(0, 7) + u8"\n Коммантарий: " + lines[j + 2].erase(0, 8) + u8"\n Дата: " + lines[j + 3].erase(0, 5));
                    }
                }
                lines.clear();
               
            }
        });
        });



























    //обработка команды IncomeSortByData (MyIncome)
    bot.getEvents().onCommand("IncomeSortByData", [&bot](TgBot::Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, u8"Выберите дату совершения дохода");
            bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
                NoReactionToCommands(bot, message);

                string CategoryStr = "Data:" + message->text;
                cout << message->text << endl;

                string ID = to_string(message->chat->id);
                vector<string> lines;
                ifstream fin(ID + ".txt");
                if (!fin.is_open()) {
                    cout << "Не удалось открыть файл для чтения!" << endl;
                    bot.getApi().sendMessage(message->chat->id, u8"Техническая ошибка, повторите попытку снова, в случае повторения проблемы пишите: https://t.me/Suguru_Geto_4");
                    return 1;
                }
                else if (fin.peek() == EOF) {
                    bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали доходов в эту дату");
                }
                else {

                    string ForFile;

                    while (getline(fin, ForFile)) {
                        lines.push_back(ForFile);
                    }
                    fin.close();
                    for (int j = 0; j < lines.size(); j++) {
                        if (lines[j] == CategoryStr && lines[j - 4] == "Type:Income") {
                            bot.getApi().sendMessage(message->chat->id, u8"Категория: " + lines[j - 3].erase(0, 9) + u8" \n Сумма: " + lines[j - 2].erase(0, 7) + u8"\n Коммантарий: " + lines[j - 1].erase(0, 8) + u8"\n Дата: " + lines[j].erase(0, 5));
                        }
                    }
                    lines.clear();

                }
                });
        });



    //обработка команды IncomeSortByAmountIncrease (MyExpense)
    bot.getEvents().onCommand("IncomeSortByAmountIncrease", [&bot](TgBot::Message::Ptr message)
        {
            string ID = to_string(message->chat->id);
            vector<string> lines;
            vector<int> numbers;
            ifstream fin(ID + ".txt");
            if (!fin.is_open()) {
                cout << "Не удалось открыть файл для чтения!" << endl;
                return 1;
            }
            else if (fin.peek() == EOF) {
                bot.getApi().sendMessage(message->chat->id, u8"похоже вы еще не совершали расходов");
            }
            else {

                string ForFile;

                while (getline(fin, ForFile)) {
                    lines.push_back(ForFile);
                }

                fin.close();
                for (int j = 0; j < lines.size(); j++) {
                    if (lines[j].substr(0, 7) == "Amount:" && lines[j - 2] == "Type:Income") {
                        numbers.push_back(stoi(lines[j].erase(0, 7)));
                    }
                }
                int count = 0;
                for (int j = 0; j < numbers.size(); j++) {
                    count += numbers[j];
                }
                bot.getApi().sendMessage(message->chat->id, u8"За всё время вы получили: " + to_string(count));
            }
            lines.clear();
        });



    //обработка команды AddExpense
    ExpenseState currentState = ExpenseState::WAITING_FOR_COMMAND;
    Myfinance currentExpense;
    
    bot.getEvents().onAnyMessage([&bot, &currentState, &currentExpense](TgBot::Message::Ptr message) {
        if (currentState == ExpenseState::WAITING_FOR_COMMAND && message->text == "/AddExpense") {

            bot.getApi().sendMessage(message->chat->id, u8"Вы выбрали команду 'AddExpense': введите информацию о расходе, которую вы хотите сохранить: ");
            bot.getApi().sendMessage(message->chat->id, u8"Укажите категорию расхода (например: школа, развлечения)");
            currentState = ExpenseState::WAITING_FOR_EXPENSECATEGORY;
        }
        else if (currentState == ExpenseState::WAITING_FOR_EXPENSECATEGORY) {
            string ForAnswer;
           ForAnswer = NoReactionToCommandsForClass(message->text);

           if (ForAnswer == "you can't use any commands") {
               bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
               currentState = ExpenseState::WAITING_FOR_EXPENSECATEGORY;
           }
           else if (ForAnswer == "all right"){
               currentExpense.Category = message->text;
               bot.getApi().sendMessage(message->chat->id, u8"Вы успешно добавили: «" + message->text + u8"» в категорию этого расхода");
               bot.getApi().sendMessage(message->chat->id, u8"Укажите потраченную сумму");
               currentState = ExpenseState::WAITING_FOR_EXPENSEAMOUNT;
           }
        }
        else if (currentState == ExpenseState::WAITING_FOR_EXPENSEAMOUNT) {
            string ForAnswer;
            ForAnswer = NoReactionToCommandsForClass(message->text);

            if (ForAnswer == "you can't use any commands") {
                bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                currentState = ExpenseState::WAITING_FOR_EXPENSEAMOUNT;
            }
            else if (ForAnswer == "all right") {
                currentExpense.Amount = message->text;

                bool hasNonDigit = false;

                for (char c : currentExpense.Amount) {
                    if (!std::isdigit(c)) {
                        hasNonDigit = true;
                        break; // Прерываем цикл, если нашли не цифру
                    }
                }
                if (hasNonDigit) {
                    bot.getApi().sendMessage(message->chat->id, u8"Поле с суммой расхода должно сотоять только из цифр, пожалуйста, введите сумму еще раз");
                    currentState = ExpenseState::WAITING_FOR_EXPENSEAMOUNT;
                }
                else {
                    bot.getApi().sendMessage(message->chat->id, u8"Вы успешно добавили: «" + message->text + +u8"» как сумму, потраченную на этот расход");
                    bot.getApi().sendMessage(message->chat->id, u8"при желании укажите комментарий (пояснение) к расходу");
                    currentState = ExpenseState::WAITING_FOR_EXPENSECOMMENT;
                }


               
            }
        }
        else if (currentState == ExpenseState::WAITING_FOR_EXPENSECOMMENT) {

            string ForAnswer;
            ForAnswer = NoReactionToCommandsForClass(message->text);

            if (ForAnswer == "you can't use any commands") {
                bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                currentState = ExpenseState::WAITING_FOR_EXPENSECOMMENT;
            }
            else if (ForAnswer == "all right") {
                currentExpense.Comment = message->text;
                bot.getApi().sendMessage(message->chat->id, u8"Вы успешно указали: «" + message->text + +u8"» как комментарий (пояснение) к этому расходу");
                bot.getApi().sendMessage(message->chat->id, u8"Укажите дату (в будущем это поможет отсортировать ваши расходы)");
                currentState = ExpenseState::WAITING_FOR_EXPENSEDATA;
            }
         
        }   
        else if (currentState == ExpenseState::WAITING_FOR_EXPENSEDATA) {
            string ForAnswer;
            ForAnswer = NoReactionToCommandsForClass(message->text);
            if (ForAnswer == "you can't use any commands") {
                bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                currentState = ExpenseState::WAITING_FOR_EXPENSEDATA;
            }
            else if (ForAnswer == "all right") {
                currentExpense.Data = message->text;
                bot.getApi().sendMessage(message->chat->id, u8"Вы успешно указали: «" + message->text + +u8"» как дату этого расхода");
                bot.getApi().sendMessage(message->chat->id, u8"Расход успешно добавлен");
                currentState = ExpenseState::WAITING_FOR_COMMAND;
                bot.getApi().sendMessage(message->chat->id, u8"1. Категория вашего расхода: \n «" + currentExpense.Category + u8"» \n 2. Сумма потраченная на расход: \n «" + currentExpense.Amount + u8"» \n 3. Дополнительная информация о расходе: \n «" + currentExpense.Comment + u8"» \n 4. Дата совершения расхода: \n «" + currentExpense.Data + u8"» \n ");

                string ID = to_string(message->chat->id);

                //внесение данных о расходе в файл
                ofstream fout(ID + ".txt", ofstream::app);
                if (!fout.is_open()) {
                    cout << "Не удалось открыть файл для записи!" << endl;
                    return 1;
                }
                fout << "Type:Expense" << endl;
                fout << "Category:" + currentExpense.Category << endl;
                fout << "Amount:" + currentExpense.Amount << endl;
                fout << "Comment:" + currentExpense.Comment << endl;
                fout << "Data:" + currentExpense.Data << endl;
                fout.close();
                return 1;
            }
        }
  });
            


     //обработка команды AddIncome
            bot.getEvents().onAnyMessage([&bot, &currentState, &currentExpense](TgBot::Message::Ptr message) {
                if (currentState == ExpenseState::WAITING_FOR_COMMAND && message->text == "/AddIncome") {
                    bot.getApi().sendMessage(message->chat->id, u8"Вы выбрали команду 'AddIncome': введите информацию о доходе, которую вы хотите сохранить: ");
                    bot.getApi().sendMessage(message->chat->id, u8"Укажите категорию (например: работа, стипендия)");
                    currentState = ExpenseState::WAITING_FOR_INCOMECATEGORY;
                }
                else if (currentState == ExpenseState::WAITING_FOR_INCOMECATEGORY) {


                    string ForAnswer;
                    ForAnswer = NoReactionToCommandsForClass(message->text);

                    if (ForAnswer == "you can't use any commands") {
                        bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                        currentState = ExpenseState::WAITING_FOR_INCOMECATEGORY;
                    }
                    else if (ForAnswer == "all right") {
                        currentExpense.Category = message->text;
                        bot.getApi().sendMessage(message->chat->id, u8"Вы успешно добавили: «" + message->text + +u8"» как категорию данного дохода");
                        bot.getApi().sendMessage(message->chat->id, u8"Укажите полученную сумму ");
                        currentState = ExpenseState::WAITING_FOR_INCOMEAMOUNT;
                    }
                }
                else if (currentState == ExpenseState::WAITING_FOR_INCOMEAMOUNT) {

                    string ForAnswer;
                    ForAnswer = NoReactionToCommandsForClass(message->text);

                    if (ForAnswer == "you can't use any commands") {
                        bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                        currentState = ExpenseState::WAITING_FOR_INCOMEAMOUNT;
                    }
                    else if (ForAnswer == "all right") {
                        currentExpense.Amount = message->text;

                        bool hasNonDigit = false;

                        for (char c : currentExpense.Amount) {
                            if (!std::isdigit(c)) {
                                hasNonDigit = true;
                                break; // Прерываем цикл, если нашли не цифру
                            }
                        } if (hasNonDigit) {
                            bot.getApi().sendMessage(message->chat->id, u8"Поле с суммой расхода должно сотоять только из цифр, пожалуйста, введите сумму еще раз");
                            currentState = ExpenseState::WAITING_FOR_EXPENSEAMOUNT;
                        }
                        else {
                            bot.getApi().sendMessage(message->chat->id, u8"Вы успешно добавили: «" + message->text + +u8"» как сумму, полученную при этом доходе");
                            bot.getApi().sendMessage(message->chat->id, u8"При желании укажите комментарий (пояснение) к доходу");
                            currentState = ExpenseState::WAITING_FOR_INCOMECOMMENT;
                        }



                    }
                }
                else if (currentState == ExpenseState::WAITING_FOR_INCOMECOMMENT) {

                    string ForAnswer;
                    ForAnswer = NoReactionToCommandsForClass(message->text);

                    if (ForAnswer == "you can't use any commands") {
                        bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                        currentState = ExpenseState::WAITING_FOR_INCOMECOMMENT;
                        }
                        else if (ForAnswer == "all right") {
                        currentExpense.Comment = message->text;
                        bot.getApi().sendMessage(message->chat->id, u8"Вы успешно указали: «" + message->text + +u8"» как комментарий (пояснение) к этому доходу");
                        bot.getApi().sendMessage(message->chat->id, u8"Укажите дату (в будущем это поможет отсортировать ваши доходы)");
                        currentState = ExpenseState::WAITING_FOR_INCOMEDATA;
                        }
                }
                else if (currentState == ExpenseState::WAITING_FOR_INCOMEDATA) {

                    string ForAnswer;
                    ForAnswer = NoReactionToCommandsForClass(message->text);

                    if (ForAnswer == "you can't use any commands") {
                        bot.getApi().sendMessage(message->chat->id, u8"Вы не можете отправлять боту команды во время заполнения информации, процесс заполнения приостановлен, чтобы повторить операцию начните заполнять информацию повторно");
                        currentState = ExpenseState::WAITING_FOR_INCOMEDATA;
                        }
                        else if (ForAnswer == "all right") {
                        currentExpense.Data = message->text;
                        bot.getApi().sendMessage(message->chat->id, u8"Вы успешно указали: «" + message->text + +u8"» как дату этого дохода");
                        bot.getApi().sendMessage(message->chat->id, u8"Доход успешно добавлен");
                        currentState = ExpenseState::WAITING_FOR_COMMAND;
                        bot.getApi().sendMessage(message->chat->id, u8"1. Категория вашего дохода: \n «" + currentExpense.Category + u8"» \n 2. Сумма полученная при доходе: \n «" + currentExpense.Amount + u8"» \n 3. Дополнительная информация о доходе: \n «" + currentExpense.Comment + u8"» \n 4. Дата дохода: \n «" + currentExpense.Data + u8"» \n ");

                        string ID = to_string(message->chat->id);
                        cout << ID << endl;

                        //внесение данных о доходе в файл
                        ofstream fout(ID + ".txt", ofstream::app);
                        if (!fout.is_open()) {
                            cout << "Не удалось открыть файл для записи!" << endl;
                            return 1;
                        }
                        fout << "Type:Income" << endl;
                        fout << "Category" + currentExpense.Category << endl;
                        fout << "Amount:" + currentExpense.Amount << endl;
                        fout << "Comment:" + currentExpense.Comment << endl;
                        fout << "Data:" + currentExpense.Data << endl;
                        fout.close();
                    }
        }
        });


    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}    