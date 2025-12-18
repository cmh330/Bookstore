//
// Created by Administrator on 2025/12/9.
//

#ifndef BOOKSTORE_BOOKSYSTEM_H
#define BOOKSTORE_BOOKSYSTEM_H
#include "storage.h"
#include "AccountSystem.h"
#include "FinanceSystem.h"
#include <vector>
#include <cstring>

struct Book {
    char ISBN[21];
    char name[61];
    char author[61];
    char keyword[61];
    int quantity;
    double price;

    Book() : quantity(0), price(0.0) {
        for (int i = 0; i < 21; ++i) ISBN[i] = 0;
        for (int i = 0; i < 61; ++i) {
            name[i] = 0;
            author[i] = 0;
            keyword[i] = 0;
        }
    }
    bool operator<(const Book& other) const {
        return strcmp(ISBN, other.ISBN) < 0;
    }
    bool operator==(const Book& other) const {
        return strcmp(ISBN, other.ISBN) == 0;
    }
    bool operator<=(const Book& other) const {
        return strcmp(ISBN, other.ISBN) <= 0;
    }
};

class BookSystem {
private:
    BlockLinkedList<20, Book>* bookStorage; // ISBN -> 整体图书信息

    // 为了方便后面将ISBN与图书各类信息（name等）封装成块状链表，定义一个结构体
    struct ISBNValue {
        char isbn[21];
        ISBNValue() {
            for (int i = 0; i < 21; ++i) isbn[i] = 0;
        }
        bool operator<(const ISBNValue& other) const {
            return (strcmp(isbn, other.isbn) < 0);
        }
        bool operator==(const ISBNValue& other) const {
            return (strcmp(isbn, other.isbn) == 0);
        }
        bool operator>(const ISBNValue& other) const {
            return (strcmp(isbn, other.isbn) > 0);
        }
    };

    BlockLinkedList<60, ISBNValue>* nameISBN; // name -> ISBN
    BlockLinkedList<60, ISBNValue>* authorISBN; // author -> ISBN
    BlockLinkedList<60, ISBNValue>* keywordISBN; // keyword -> ISBN

    AccountSystem* accountSystem;
    FinanceSystem* financeSystem;

    bool isValidISBN(const string& s);
    bool isValidAuthorOrName(const string& s);
    bool isValidKeyword(const string& s);  // 验证关键词（检查有没有重复的，允许出现'|'）
    bool isValidPrice(double price);

    void deleteFromIndexList(const Book& book);
    void insertToIndexList(const Book& book);
    void UpdateIndexList(const Book& oldBook, const Book& newBook);

    // 打印单本
    void printBook(const Book& book);

    bool parseShow(const string& line, string& field, string& value);

    bool parseModify(const string& line, std::map<string, string>& result);
    bool isValidModifications(const std::map<string, string>& modifications, const string& currentISBN);


public:
    BookSystem(AccountSystem* as, FinanceSystem* fs); // s申请空间并初始化
    ~BookSystem(); // 释放空间

    // // 无附加参数时，输出所有图书
    // // {1} （所有show都是）
    // void show();
    //
    // // 按ISBN查询
    // // 输出：图书信息或空行（未找到时）; "Invalid"（ISBN为空时）
    // void show(const string& ISBN);
    //
    // // field表示属于哪个板块（如name）
    // // 输出：图书信息或空行（未找到时）; "Invalid"（附加参数内容为空时，[Keyword] 中出现多个关键词时）
    // void show(const string& field, const string& value);

    // 统一的show接口
    void show(const string& line);

    // 输出: 总价（成功）; "Invalid"（失败：权限不足/图书不存在/库存不足/quantity<=0）
    // {1}
    void buy(const string& ISBN, int quantity);

    // 查找图书，如果不存在则创建空白图书; 调用accountSystem中setSelectedBook(ISBN)
    // 输出: 无输出（成功）; "Invalid"（权限不足）
    // {3}
    void select(const string& ISBN);
    // 还没写setselectedbook

    // {3}
    void modify(const string& line);

    // 失败：未选择书 / 购入数量 <= 0 / 总额 <= 0
    // {3}
    void import(int quantity, double totalCost);
};


#endif //BOOKSTORE_BOOKSYSTEM_H