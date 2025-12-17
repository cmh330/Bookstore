//
// Created by Administrator on 2025/12/8.
//

#ifndef BOOKSTORE_ACCOUNTSYSTEM_H
#define BOOKSTORE_ACCOUNTSYSTEM_H
#include "storage.h"
#include <vector>
#include <map>

struct Account {
    char userID[31];
    char password[31];
    char username[31];
    int privilege;

    Account() : privilege(0) {
        for (int i = 0; i < 31; ++i) {
            userID[i] = 0;
            password[i] = 0;
            username[i] = 0;
        }
    }
};

class AccountSystem {
private:
    BlockLinkedList<30, Account>* accountStorage; // userID -> 账户信息
    std::vector<Account> loginStack; // 记录都有哪些用户登录
    std::map<string, string> selectedBooks; // userID -> ISBN

    bool isValidPasswordOrID(const string& s);
    bool isValidUsername(const string& s);
    bool isValidPrivilege(int privilege);

public:
    AccountSystem(); // 申请空间
    ~AccountSystem(); // 释放空间

    // 登录：若成功则修改登录栈
    // {0}
    void su(const string& userID, const string& password = "");

    // 登出登录栈中最后一个账户
    // 若无已登录帐户则操作失败
    // {1}
    void logout();

    // 注册权限等级为 {1} 的新账户
    // 若userID与已注册用户重复则失败
    // {0}
    void registerAccount(const string& userID, const string& password, const string& username);

    // 修改密码，权限为{7}账户可以不输入旧密码
    // 若userID不存在则失败，若旧密码不正确则不存在
    // {1}
    void passwd(const string& userID, const string& newPassword, const string& currentPassword = "");

    // 创建账户
    // 若userID与已注册账户重复则失败，若待创建帐户的权限等级 >= 当前帐户权限等级则失败
    // {3}
    void useradd(const string& userID, const string& password, int privilege, const string& username);

    // 删除账户
    // 若该账户不存在或已登录则失败
    // {7}
    void deleteAccount(const string& userID);

    // 设置选中图书
    void setSelectedBook(const string& ISBN);
    // 获取选中图书
    string getSelectedBook() const;

    int getCurrentPrivilege() const;
    bool hasPrivilege(int required) const;
    string getCurrentUserID() const;

    bool isUserLoggedIn(const string& userID) const;
    void initializeRoot();
};

#endif //BOOKSTORE_ACCOUNTSYSTEM_H