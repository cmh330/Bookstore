//
// Created by Administrator on 2025/12/16.
//
#include <iostream>
#include <string>
#include <vector>
#include "AccountSystem.h"
#include "FinanceSystem.h"
#include "BookSystem.h"

using std::string;
using std::vector;

vector<string> Split(const string& line) {
    vector<string> result;
    string current;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == ' ' || line[i] == '\t') {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += line[i];
        }
    }
    if (!current.empty()) {
        result.push_back(current);
    }
    return result;
}

string Extract(const string& line, const string& operation) {
    int start = line.find(operation);
    if (start == string::npos) {
        return "";
    }
    start += operation.size();
    while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) {
        ++start;
    }
    if (start >= line.size()) {
        return "";
    }
    string result = line.substr(start);
    return result;
}

int main() {
    AccountSystem* accountSystem = new AccountSystem();
    FinanceSystem* financeSystem = new FinanceSystem(accountSystem);
    BookSystem* bookSystem = new BookSystem(accountSystem, financeSystem);

    string line;
    while (getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> tokens = Split(line);
        string operation = tokens[0];
        if (operation == "quit" || operation == "exit") {
            break;
        }

        else if (operation == "su") {
            if (tokens.size() == 2) {
                accountSystem->su(tokens[1]);
            } else if (tokens.size() == 3) {
                accountSystem->su(tokens[1], tokens[2]);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "logout") {
            if (tokens.size() == 1) {
                accountSystem->logout();
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "register") {
            if (tokens.size() == 4) {
                accountSystem->registerAccount(tokens[1], tokens[2], tokens[3]);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "passwd") {
            if (tokens.size() == 3) {
                accountSystem->passwd(tokens[1], tokens[2]);
            } else if (tokens.size() == 4) {
                accountSystem->passwd(tokens[1], tokens[3], tokens[2]);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "useradd") {
            if (tokens.size() == 5) {
                int privilege = stoi(tokens[3]);
                accountSystem->useradd(tokens[1], tokens[2], privilege, tokens[4]);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "delete") {
            if (tokens.size() == 2) {
                accountSystem->deleteAccount(tokens[1]);
            } else {
                std::cout << "Invalid\n";
            }
        }


        else if (operation == "show") {
            string newLine = Extract(line, "show");
            if (newLine.find("finance") == 0) {
                string financeLine = Extract(newLine, "finance");
                if (financeLine.empty()) {
                    financeSystem->showFinance(-1);
                } else {
                    int count = stoi(financeLine);
                    financeSystem->showFinance(count);
                }
            } else {
                bookSystem->show(newLine);
            }
        }

        else if (operation == "buy") {
            if (tokens.size() == 3) {
                int quantity = stoi(tokens[2]);
                bookSystem->buy(tokens[1], quantity);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "select") {
            if (tokens.size() == 2) {
                bookSystem->select(tokens[1]);
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "modify") {
            string modify = Extract(line, "modify");
            bookSystem->modify(modify);
        }

        else if (operation == "import") {
            if (tokens.size() == 3) {
                int quantity = stoi(tokens[1]);
                double totalCost = stod(tokens[2]);
                bookSystem->import(quantity, totalCost);
            } else {
                std::cout << "Invalid\n";
            }
        }


        else if (operation == "report") {
            if (tokens.size() == 2) {
                if (tokens[1] == "finance") {
                    financeSystem->printFinanceReport();
                } else if (tokens[1] == "employee") {
                    financeSystem->printEmployeeReport();
                } else {
                    std::cout << "Invalid\n";
                }
            } else {
                std::cout << "Invalid\n";
            }
        }

        else if (operation == "log") {
            if (tokens.size() == 1) {
                financeSystem->printLog();
            } else {
                std::cout << "Invalid\n";
            }
        }

        else {
            std::cout << "Invalid\n";
        }
    }

    delete accountSystem;
    delete financeSystem;
    delete bookSystem;
    return 0;
}