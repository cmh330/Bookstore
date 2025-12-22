//
// Created by Administrator on 2025/12/8.
//
#include "AccountSystem.h"
#include <iostream>
#include <cstring>

void AccountSystem::initializeRoot() {
    vector<Account> nowExisting = accountStorage->Find("root");
    if (!nowExisting.empty()) {
        return;
    }
    Account root;
    strcpy(root.userID, "root");
    strcpy(root.password, "sjtu");
    strcpy(root.username, "root");
    root.privilege = 7;
    accountStorage->Insert("root", root);
}



// ========== 构造 & 析构 ==========
AccountSystem::AccountSystem() {
    accountStorage = new BlockLinkedList<30, Account>("account");
    loginStack.clear();
    selectedBooks.clear();

    initializeRoot();
}
AccountSystem::~AccountSystem() {
    delete accountStorage;
}



// =========== 辅助 ============
bool AccountSystem::isValidPasswordOrID(const string& s) {
    if (s.empty() || s.length() > 30) {
        return false;
    }
    for (char c : s) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))) {
            return false;
        }
    }
    return true;
}
bool AccountSystem::isValidUsername(const string& s) {
    if (s.empty() || s.length() > 30) {
        return false;
    }
    for (char c : s) {
        if (!(c >= 32 && c <= 126)) {
            return false;
        }
    }
    return true;
}
bool AccountSystem::isValidPrivilege(int privilege) {
    if (!(privilege == 7 || privilege == 1 || privilege == 3 || privilege == 0)) {
        return false;
    }
    return true;
}
int AccountSystem::getCurrentPrivilege() const {
    if (loginStack.empty()) {
        return 0;
    }
    return loginStack.back().privilege;
}
bool AccountSystem::hasPrivilege(int required) const {
    if (loginStack.empty()) {
        return 0;
    }
    return (loginStack.back().privilege >= required);
}
string AccountSystem::getCurrentUserID() const {
    if (loginStack.empty()) {
        return "";
    }
    return loginStack.back().userID;
}

void AccountSystem::setSelectedBook(const string& ISBN) {
    if (loginStack.empty()) {
        return;
    }
    string userID = getCurrentUserID();
    selectedBooks[userID] = ISBN;
}
string AccountSystem::getSelectedBook() const {
    if (loginStack.empty()) {
        return "";
    }
    string userID = getCurrentUserID();
    auto it = selectedBooks.find(userID);
    if (it == selectedBooks.end()) {
        return "";
    }
    return it->second;
}
bool AccountSystem::isUserLoggedIn(const string& userID) const {
    for (int i = 0; i < loginStack.size(); ++i) {
        if (strcmp(loginStack[i].userID, userID.c_str()) == 0) {
            return true;
        }
    }
    return false;
}



// ============ 登录 ============
void AccountSystem::su(const string& userID, const string& password) {
    if (!isValidPasswordOrID(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!password.empty() && !isValidPasswordOrID(password)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Account> nowExisting = accountStorage->Find(userID);
    if (nowExisting.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    Account account = nowExisting[0];
    int currentPrivilege = getCurrentPrivilege();
    if (currentPrivilege > account.privilege) {
        loginStack.push_back(account);
        return;
    }
    if (password.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    if (strcmp(account.password, password.c_str()) == 0) {
        loginStack.push_back(account);
        return;
    } else {
        std::cout << "Invalid\n";
        return;
    }
}


// ============ 登出 ============
void AccountSystem::logout() {
    if (loginStack.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    if (!hasPrivilege(1)) {
        std::cout << "Invalid\n";
        return;
    }
    string userID = loginStack.back().userID;
    loginStack.pop_back();

    bool stillLoggedIn = false;
    for (int i = 0; i < loginStack.size(); ++i) {
        if (strcmp(loginStack[i].userID, userID.c_str()) == 0) {
            stillLoggedIn = true;
            break;
        }
    }
    if (!stillLoggedIn) {
        selectedBooks.erase(userID);
    }
}


// ============ 注册 ============
void AccountSystem::registerAccount(const string& userID, const string& password, const string& username) {
    if (!isValidPasswordOrID(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidPasswordOrID(password)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidUsername(username)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Account> nowExisting = accountStorage->Find(userID);
    if (!nowExisting.empty()) {
        std::cout << "Invalid\n";
        return;
    }

    Account newAccount;
    strcpy(newAccount.username, username.c_str());
    strcpy(newAccount.password, password.c_str());
    strcpy(newAccount.userID, userID.c_str());
    newAccount.privilege = 1;
    accountStorage->Insert(userID, newAccount);
}


// ============ 修改密码 ============
void AccountSystem::passwd(const string& userID, const string& newPassword, const string& currentPassword) {
    if (!isValidPasswordOrID(newPassword)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!currentPassword.empty() && !isValidPasswordOrID(currentPassword)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidPasswordOrID(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!hasPrivilege(1)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Account> nowExisting = accountStorage->Find(userID);
    if (nowExisting.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    Account account = nowExisting[0];
    int currentPrivilege = getCurrentPrivilege();
    if (currentPrivilege == 7) {
        Account updatedAccount = account;
        strcpy(updatedAccount.password, newPassword.c_str());
        accountStorage->Delete(userID, account);
        accountStorage->Insert(userID, updatedAccount);

        for (int i = 0; i < loginStack.size(); ++i) {
            if (strcmp(loginStack[i].userID, userID.c_str()) == 0) {
                loginStack[i] = updatedAccount;
            }
        }
        return;
    }

    if (currentPassword.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    if (strcmp(account.password, currentPassword.c_str()) != 0) {
        std::cout << "Invalid\n";
        return;
    } else {
        Account updatedAccount = account;
        strcpy(updatedAccount.password, newPassword.c_str());
        accountStorage->Delete(userID, account);
        accountStorage->Insert(userID, updatedAccount);

        for (int i = 0; i < loginStack.size(); ++i) {
            if (strcmp(loginStack[i].userID, userID.c_str()) == 0) {
                loginStack[i] = updatedAccount;
            }
        }
        return;
    }
}


// ============ 创建账户 ============
void AccountSystem::useradd(const string& userID, const string& password, int privilege, const string& username) {
    if (!isValidUsername(username)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidPasswordOrID(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidPasswordOrID(password)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidPrivilege(privilege)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!hasPrivilege(3)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Account> nowExisting = accountStorage->Find(userID);
    if (!nowExisting.empty()) {
        std::cout << "Invalid\n";
        return;
    }

    int currentPrivilege = getCurrentPrivilege();
    if (privilege >= currentPrivilege) {
        std::cout << "Invalid\n";
        return;
    }

    Account newAccount;
    strcpy(newAccount.username, username.c_str());
    strcpy(newAccount.password, password.c_str());
    newAccount.privilege = privilege;
    strcpy(newAccount.userID, userID.c_str());
    accountStorage->Insert(userID, newAccount);
}


// ============ 删除账户 ============
void AccountSystem::deleteAccount(const string& userID) {
    if (!isValidPasswordOrID(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!hasPrivilege(7)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Account> nowExisting = accountStorage->Find(userID);
    if (nowExisting.empty()) {
        std::cout << "Invalid\n";
        return;
    }

    if (isUserLoggedIn(userID)) {
        std::cout << "Invalid\n";
        return;
    }
    accountStorage->Delete(userID, nowExisting[0]);
}


void AccountSystem::modifySelectedBooks(const string& oldISBN, const string& newISBN) {
    for (auto it = selectedBooks.begin(); it != selectedBooks.end(); ++it) {
        if (it->second == oldISBN) {
            it->second = newISBN;
        }
    }
}