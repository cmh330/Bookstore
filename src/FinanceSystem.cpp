//
// Created by Administrator on 2025/12/11.
//
#include <iostream>
#include <cstring>
#include <vector>
#include "FinanceSystem.h"
#include <iomanip>

using std::vector;


// ========== 构造 & 析构 ==========
FinanceSystem::FinanceSystem(AccountSystem* as) : transactionCounter(0), operationCounter(0), accountSystem(as) {
    transactionStorage = new BlockLinkedList<100, Transaction>("transaction");
    operationStorage = new BlockLinkedList<100, OperationLog>("operations");

    // 块状链表中用一个特殊的key-value计数
    // key：counter，value.index = 实际操作数
    vector<Transaction> trans_cnt = transactionStorage->Find("counter");
    if (!trans_cnt.empty()) {
        transactionCounter = trans_cnt[0].index;
    } else {
        transactionCounter = 0;
    }

    vector<OperationLog> op_cnt = operationStorage->Find("counter");
    if (!op_cnt.empty()) {
        operationCounter = op_cnt[0].index;
    } else {
        operationCounter = 0;
    }
}

FinanceSystem::~FinanceSystem() {
    // 保存一下计数器
    Transaction trans_cnt;
    trans_cnt.amount = 0;
    trans_cnt.index = transactionCounter;
    vector<Transaction> oldTrans = transactionStorage->Find("counter");
    if (!oldTrans.empty()) {
        transactionStorage->Delete("counter", oldTrans[0]);
    }
    transactionStorage->Insert("counter", trans_cnt);

    OperationLog op_cnt;
    op_cnt.index = operationCounter;
    vector<OperationLog> oldOps = operationStorage->Find("counter");
    if (!oldOps.empty()) {
        operationStorage->Delete("counter", oldOps[0]);
    }
    operationStorage->Insert("counter", op_cnt);

    delete transactionStorage;
    delete operationStorage;
}


// ========== 记录交易 ==========
void FinanceSystem::recordTransaction(double amount) {
    ++transactionCounter;
    Transaction newTransaction;
    newTransaction.amount = amount;
    newTransaction.index = transactionCounter;
    char key[30];
    sprintf(key, "%lld", transactionCounter);
    transactionStorage->Insert(key, newTransaction);
}


// ========== 记录用户操作日志 ==========
void FinanceSystem::recordOperation(const string& userID, const string& operation) {
    ++operationCounter;
    OperationLog newOperation;
    newOperation.index = operationCounter;

    strncpy(newOperation.userID, userID.c_str(), 30);
    newOperation.userID[30] = '\0';
    strncpy(newOperation.operation, operation.c_str(), 149);
    newOperation.operation[149] = '\0';

    char key[30];
    sprintf(key, "%lld", operationCounter);
    operationStorage->Insert(key, newOperation);
}


// ========== 财务记录查询 ==========
void FinanceSystem::showFinance(int count) {
    if (!accountSystem->hasPrivilege(7)) {
        std::cout << "Invalid\n";
        return;
    }

    if (count == 0) {
        std::cout << '\n';
        return;
    }

    // 获取所有操作
    vector<Transaction> allTrans;
    for (long long i = 1; i <= transactionCounter; ++i) {
        char key[30];
        sprintf(key, "%lld", i);
        vector<Transaction> newTransaction;
        newTransaction = transactionStorage->Find(key);
        if (!newTransaction.empty()) {
            allTrans.push_back(newTransaction[0]);
        }
    }

    if (count == -1) {
        count = allTrans.size();
    }

    if (count > allTrans.size()) {
        std::cout << "Invalid\n";
        return;
    }

    if (allTrans.empty()) {
        std::cout << "+ 0.00 - 0.00\n";
        return;
    }

    long long start = allTrans.size() - count;
    double income = 0.0;
    double expense = 0.0;
    for (long long i = start; i < allTrans.size(); ++i) {
        if (allTrans[i].amount > 0) {
            income += allTrans[i].amount;
        } else {
            expense += (-allTrans[i].amount);
        }
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "+ " << income << " - " << expense << "\n";
}


// ========== 打印财务报表 ==========
void FinanceSystem::printFinanceReport() {
    if (!accountSystem->hasPrivilege(7)) {
        std::cout << "Invalid\n";
        return;
    }

    std::cout << "Finance Report\n";
    vector<Transaction> allTrans;
    for (long long i = 1; i <= transactionCounter; ++i) {
        char key[30];
        sprintf(key, "%lld", i);
        vector<Transaction> newTransaction;
        newTransaction = transactionStorage->Find(key);
        if (!newTransaction.empty()) {
            allTrans.push_back(newTransaction[0]);
        }
    }

    if (allTrans.empty()) {
        std::cout << "No transaction recorded.\n";
        return;
    }

    double income = 0.0;
    double expense = 0.0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Transaction History:\n";

    for (long long i = 0; i < allTrans.size(); ++i) {
        char temp[20];
        sprintf(temp, "(%lld)", allTrans[i].index);
        std::cout << std::left << std::setw(7) << temp;
        if (allTrans[i].amount > 0 ) {
            std::cout << std::left << std::setw(10) << "Income:" << "+" << allTrans[i].amount << '\n';
            income += allTrans[i].amount;
        } else {
            std::cout << std::left << std::setw(10) << "Expense:" << "-" << (-allTrans[i].amount) << '\n';
            expense += (-allTrans[i].amount);
        }
    }

    std::cout << "Summary:\n";
    std::cout << std::left << std::setw(16) << "Total income:" << "+" << income << "\n";
    std::cout << std::left << std::setw(16) << "Total expense:" << "-" << expense << "\n";
    std::cout << std::left << std::setw(16) << "Balance:" << (income - expense) << '\n';
    std::cout << '\n' << '\n';
}


// ========== 打印员工工作情况 ==========
void FinanceSystem::printEmployeeReport() {
    if (!accountSystem->hasPrivilege(7)) {
        std::cout << "Invalid\n";
        return;
    }

    std::cout << "Employee Report\n";
    vector<OperationLog> allOps;
    for (long long i = 1; i <= operationCounter; ++i) {
        char key[30];
        sprintf(key, "%lld", i);
        vector<OperationLog> newOperation;
        newOperation = operationStorage->Find(key);
        if (!newOperation.empty()) {
            allOps.push_back(newOperation[0]);
        }
    }

    if (allOps.empty()) {
        std::cout << "No operation recorded.\n";
        return;
    }

    int maxLength = 0;
    for (long long i = 0; i < allOps.size(); ++i) {
        int length = strlen(allOps[i].userID);
        if (length > maxLength) {
            maxLength = length;
        }
    }

    std::cout << "Operation History:\n";
    for (long long i = 0; i < allOps.size(); ++i) {
        char temp[20];
        sprintf(temp, "(%lld)", allOps[i].index);
        std::cout << std::left << std::setw(7) << temp;
        std::cout << std::left << std::setw(maxLength + 3) << allOps[i].userID;
        std::cout << allOps[i].operation << '\n';
    }
    std::cout<< '\n' << '\n';
}


// ========== 打印日志 ==========
void FinanceSystem::printLog() {
    if (!accountSystem->hasPrivilege(7)) {
        std::cout << "Invalid\n";
        return;
    }

    printFinanceReport();
    printEmployeeReport();
}