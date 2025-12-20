//
// Created by Administrator on 2025/12/11.
//

#ifndef BOOKSTORE_FINANCESYSTEM_H
#define BOOKSTORE_FINANCESYSTEM_H

#include <vector>
#include <fstream>
#include "storage.h"
#include "AccountSystem.h"
using std::vector;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

struct Transaction {
    double amount;      // 正数为收入，负数为支出
    long long index;    // 交易序号（第几笔交易）

    Transaction() : amount(0), index(0) {}
    Transaction(double a, long long b) : amount(a), index(b) {}

    bool operator<(const Transaction& other) const {
        return index < other.index;
    }
    bool operator==(const Transaction& other) const {
        return index == other.index;
    }
    bool operator<=(const Transaction& other) const {
        return index <= other.index;
    }
};

struct OperationLog {
    char userID[31];
    char operation[150];
    long long index;    // 操作序号（第几次操作）

    OperationLog() : index(0) {
        for (int i = 0; i < 31; ++i) userID[i] = 0;
        for (int i = 0; i < 150; ++i) operation[i] = 0;
    }

    bool operator<(const OperationLog& other) const {
        return index < other.index;
    }
    bool operator==(const OperationLog& other) const {
        return index == other.index;
    }
    bool operator<=(const OperationLog& other) const {
        return index <= other.index;
    }
};

class FinanceSystem {
private:
    BlockLinkedList<100, Transaction>* transactionStorage;
    BlockLinkedList<100, OperationLog>* operationStorage;
    long long transactionCounter;
    long long operationCounter;
    AccountSystem* accountSystem;
    fstream financeFile;
    fstream logFile;

public:
    // 打开文件
    FinanceSystem(AccountSystem* as);

    // 保存、关闭文件
    ~FinanceSystem();

    // 记录交易，分配序号（++transactionCounter）
    void recordTransaction(double amount);

    // 记录用户操作日志，分配序号（++operationCounter）
    void recordOperation(const string& userID, const string& operation);

    // count = -1 输出全部 / count = 0 输出空行
    // {7}
    void showFinance(int count = -1);

    // {7}
    void printFinanceReport();
    void printEmployeeReport();
    void printLog();

};

#endif //BOOKSTORE_FINANCESYSTEM_H