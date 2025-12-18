//
// Created by Administrator on 2025/12/9.
//
#include <iostream>
#include <cstring>
#include <vector>
#include "BookSystem.h"
#include <iomanip>
#include <algorithm>

using std::vector;

// ========== 构造 & 析构 ==========
BookSystem::BookSystem(AccountSystem* as, FinanceSystem* fs) : accountSystem(as), financeSystem(fs) {
    bookStorage = new BlockLinkedList<20, Book>("book");
    nameISBN = new BlockLinkedList<60, ISBNValue>("name_isbn");
    authorISBN = new BlockLinkedList<60, ISBNValue>("author_isbn");
    keywordISBN = new BlockLinkedList<60, ISBNValue>("keyword_isbn");
}
BookSystem::~BookSystem() {
    delete bookStorage;
    delete nameISBN;
    delete authorISBN;
    delete keywordISBN;
}


// ========== 辅助 ==========
bool compareByISBN(const Book& a, const Book& b) {
    return strcmp(a.ISBN, b.ISBN) < 0;
}


// =========== 检查各类名称是否合法 ============
bool BookSystem::isValidISBN(const string& s) {
    if (s.empty() || s.length() > 20) {
        return false;
    }
    for (char c : s) {
        if (c < 32 || c > 126) {
            return false;
        }
    }
    return true;
}
bool BookSystem::isValidAuthorOrName(const string& s) {
    if (s.empty() || s.length() > 60) {
        return false;
    }
    for (char c : s) {
        if (c < 32 || c > 126 || c == '"') {
            return false;
        }
    }
    return true;
}
bool BookSystem::isValidKeyword(const string& s) {
    if (s.empty() || s.length() > 60) {
        return false;
    }
    for (char c : s) {
        if (c < 32 || c > 126 || c == '"') {
            return false;
        }
    }

    // 检查有没有重复
    vector<string> keywords;
    int start = 0;
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '|') {
            if (i > start) {
                string temp = s.substr(start, i - start);
                for (int j = 0; j < keywords.size(); j++) {
                    if (keywords[j] == temp) {
                        return false;
                    }
                }
                keywords.push_back(temp);
            }
            start = i + 1;
        }
    }
    // 最后一个关键词
    if (start < s.size()) {
        string temp = s.substr(start);
        for (int j = 0; j < keywords.size(); j++) {
            if (temp == keywords[j]) {
                return false;
            }
        }
    }
    return true;
}
bool BookSystem::isValidPrice(double price) {
    return price >= 0.0;
}


// =========== 更新辅助块状链表 ============
void BookSystem::deleteFromIndexList(const Book& book) {
    ISBNValue isbnValue;
    strcpy(isbnValue.isbn, book.ISBN);

    if (strlen(book.name) > 0) {
        nameISBN->Delete(book.name, isbnValue);
    }

    if (strlen(book.author) > 0) {
        authorISBN->Delete(book.author, isbnValue);
    }

    if (strlen(book.keyword) > 0) {
        string keyword = book.keyword;
        int start = 0;
        for (int i = 0; i < keyword.size(); i++) {
            if (keyword[i] == '|') {
                if (i > start) {
                    string temp = keyword.substr(start, i - start);
                    keywordISBN->Delete(temp, isbnValue);
                }
                start = i + 1;
            }
        }
        if (start < keyword.size()) {
            string temp = keyword.substr(start);
            keywordISBN->Delete(temp, isbnValue);
        }
    }
}

void BookSystem::insertToIndexList(const Book& book) {
    ISBNValue isbnValue;
    strcpy(isbnValue.isbn, book.ISBN);

    if (strlen(book.name) > 0) {
        nameISBN->Insert(book.name, isbnValue);
    }

    if (strlen(book.author) > 0) {
        authorISBN->Insert(book.author, isbnValue);
    }

    if (strlen(book.keyword) > 0) {
        string keyword = book.keyword;
        int start = 0;
        for (int i = 0; i < keyword.size(); i++) {
            if (keyword[i] == '|') {
                if (i > start) {
                    string temp = keyword.substr(start, i - start);
                    keywordISBN->Insert(temp, isbnValue);
                }
                start = i + 1;
            }
        }
        if (start < keyword.size()) {
            string temp = keyword.substr(start);
            keywordISBN->Insert(temp, isbnValue);
        }
    }
}

void BookSystem::UpdateIndexList(const Book& oldBook, const Book& newBook) {
    deleteFromIndexList(oldBook);
    insertToIndexList(newBook);
}


// =========== 辅助 ============
void BookSystem::printBook(const Book& book) {
    std::cout << book.ISBN << '\t' << book.name << '\t' << book.author << '\t'
              << book.keyword << '\t' << std::fixed << std::setprecision(2) << book.price << '\t' << book.quantity << '\n';
}

bool BookSystem::parseShow(const string& line, string& field, string& value) {
    field = "";
    value = "";

    if (line.empty()) {
        // 无附加参数的情况
        return true;
    }

    // 跳过开头空格
    int start = 0;
    while (start < line.length() && (line[start] == ' ' || line[start] == '\t')) {
        ++start;
    }
    if (start >= line.length()) {
        // show 无参数
        return true;
    }

    if (line[start] != '-') {
        return false;
    }

    int equal_sign = -1;
    for (int i = 1; i < line.length(); ++i) {
        if (line[i] == '=') {
            equal_sign = i;
            break;
        }
    }
    if (equal_sign == -1) {
        return false;
    }
    field = line.substr(start + 1, equal_sign - 1 - start);

    int valuesStart = equal_sign + 1;
    if (valuesStart < line.length() && line[valuesStart] == '"') {
        // 带引号
        ++valuesStart;
        int quote_sign = -1;
        for (int i = valuesStart; i < line.length(); ++i) {
            if (line[i] == '"') {
                quote_sign = i;
                break;
            }
        }
        if (quote_sign == -1) {
            return false;
        }
        value = line.substr(valuesStart, quote_sign - valuesStart);
        if (value.empty()) {
            return false;
        }
    } else {
        // 不带引号
        value = line.substr(valuesStart);
        if (value.empty()) {
            return false;
        }
    }

    return true;
}

bool BookSystem::parseModify(const string& line, std::map<string, string>& result) {
    result.clear();

    // 跳过开头空格
    int start = 0;
    while (start < line.length() && (line[start] == ' ' || line[start] == '\t')) {
        ++start;
    }
    if (start >= line.length()) {
        return false;
    }

    while (start < line.length()) {
        while (start < line.length() && (line[start] == ' ' || line[start] == '\t')) {
            ++start;
        }
        if (start >= line.length()) {
            break;
        }
        string field = "";
        string value = "";
        if (line[start] != '-') {
            return false;
        }

        int equal_sign = -1;
        for (int i = start; i < line.length(); ++i) {
            if (line[i] == '=') {
                equal_sign = i;
                break;
            }
        }
        if (equal_sign == -1) {
            return false;
        }
        field = line.substr(start + 1, equal_sign - 1 - start);
        if (result.count(field) == 1) {
            return false;
        }

        int valuesStart = equal_sign + 1;
        if (valuesStart < line.length() && line[valuesStart] == '"') {
            // 带引号
            ++valuesStart;
            int quote_sign = -1;
            for (int i = valuesStart; i < line.length(); ++i) {
                if (line[i] == '"') {
                    quote_sign = i;
                    break;
                }
            }
            if (quote_sign == -1) {
                return false;
            }
            value = line.substr(valuesStart, quote_sign - valuesStart);
            if (value.empty()) {
                return false;
            }
            start = quote_sign + 1;
        } else {
            // 不带引号
            int valueEnd = start;
            while (valueEnd < line.length() && line[valueEnd] != ' ' && line[valueEnd] != '\t') {
                ++valueEnd;
            }
            value = line.substr(valuesStart, valueEnd - valuesStart);
            if (value.empty()) {
                return false;
            }
            start = valueEnd;
        }
        result[field] = value;
    }
    return (!result.empty());
}

bool BookSystem::isValidModifications(const std::map<string, string>& modifications, const string& currentISBN) {
    for (auto it = modifications.begin(); it != modifications.end(); ++it) {
        string field = it->first;
        string value = it->second;
        if (value.empty()) {
            return false;
        }
        if (!(field == "ISBN" || field == "name" || field == "author" || field == "keyword" || field == "price")) {
            return false;
        }
        if (field == "ISBN") {
            if (!isValidISBN(value) || value == currentISBN) {
                return false;
            }
            vector<Book> existingBooks = bookStorage->Find(value);
            if (!existingBooks.empty()) {
                return false;
            }
        } else if (field == "name" || field == "author") {
            if (!isValidAuthorOrName(value)) {
                return false;
            }
        } else if (field == "keyword") {
            if (!isValidKeyword(value)) {
                return false;
            }
        } else if (field == "price") {
            char* end;
            double price = strtod(value.c_str(), &end);
            if (!isValidPrice(price)) {
                return false;
            }
            if (*end != '\0') {
                return false;
            }
        }
    }
    return true;
}


// ============ 检索 ============
void BookSystem::show(const string& line) {
    if (!accountSystem->hasPrivilege(1)) {
        std::cout << "Invalid\n";
        return;
    }
    string field, value;
    if (!parseShow(line, field, value)) {
        std::cout << "Invalid\n";
        return;
    }
    vector<Book> result;
    if (field.empty()) {
        result = bookStorage->GetAll();
    }

    else if (field == "ISBN") {
        if (!isValidISBN(value)) {
            std::cout << "Invalid\n";
            return;
        }
        result = bookStorage->Find(value);
    }

    else if (field == "name") {
        if (!isValidAuthorOrName(value)) {
            std::cout << "Invalid\n";
            return;
        }
        vector<ISBNValue> isbnValues;
        isbnValues = nameISBN->Find(value);
        for (int i = 0; i < isbnValues.size(); ++i) {
            vector<Book> book = bookStorage->Find(isbnValues[i].isbn);
            if (!book.empty()) {
                result.push_back(book[0]);
            }
        }
    }

    else if (field == "author") {
        if (!isValidAuthorOrName(value)) {
            std::cout << "Invalid\n";
            return;
        }
        vector<ISBNValue> isbnValues;
        isbnValues = authorISBN->Find(value);
        for (int i = 0; i < isbnValues.size(); ++i) {
            vector<Book> book = bookStorage->Find(isbnValues[i].isbn);
            if (!book.empty()) {
                result.push_back(book[0]);
            }
        }
    }

    else if (field == "keyword") {
        if (!isValidAuthorOrName(value)) {
            std::cout << "Invalid\n";
            return;
        }
        // 检查有没有多个关键词
        bool valid = true;
        for (int i = 0; i < value.size(); ++i) {
            if (value[i] == '|') {
                valid = false;
                break;
            }
        }
        if (!valid) {
            std::cout << "Invalid\n";
            return;
        }

        vector<ISBNValue> isbnValues;
        isbnValues = keywordISBN->Find(value);
        for (int i = 0; i < isbnValues.size(); ++i) {
            vector<Book> book = bookStorage->Find(isbnValues[i].isbn);
            if (!book.empty()) {
                result.push_back(book[0]);
            }
        }
    }

    else {
        std::cout << "Invalid\n";
        return;
    }

    if (result.empty()) {
        std::cout << "\n";
    } else {
        std::sort(result.begin(), result.end(), compareByISBN);
        for (int i = 0; i < result.size(); ++i) {
            printBook(result[i]);
        }
    }
}


// ============ 购买 ============
void BookSystem::buy(const string& ISBN, int quantity) {
    if (!accountSystem->hasPrivilege(1)) {
        std::cout << "Invalid\n";
        return;
    }
    if (quantity <= 0 || !isValidISBN(ISBN)) {
        std::cout << "Invalid\n";
        return;
    }
    vector<Book> book = bookStorage->Find(ISBN);
    if (book.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    Book toBuy = book[0];
    if (quantity > toBuy.quantity) {
        std::cout << "Invalid\n";
        return;
    }
    double totalPrice = toBuy.price * quantity;
    std::cout << std::fixed << std::setprecision(2) << totalPrice << '\n';
    toBuy.quantity -= quantity;
    bookStorage->Delete(ISBN, book[0]);
    bookStorage->Insert(ISBN, toBuy);
    financeSystem->recordTransaction(totalPrice);
}


// ============ 选择 ============
void BookSystem::select(const string& ISBN) {
    if (!accountSystem->hasPrivilege(3)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidISBN(ISBN)) {
        std::cout << "Invalid\n";
        return;
    }
    vector<Book> book = bookStorage->Find(ISBN);
    if (book.empty()) {
        Book newBook;
        strcpy(newBook.ISBN, ISBN.c_str());
        bookStorage->Insert(ISBN, newBook);
    }
    accountSystem->setSelectedBook(ISBN);
}


// ============ 修改 ============
void BookSystem::modify(const string& line) {
    if (!accountSystem->hasPrivilege(3)) {
        std::cout << "Invalid\n";
        return;
    }
    std::map<string, string> modifications;
    string currentISBN = accountSystem->getSelectedBook();
    if (currentISBN.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    if (!parseModify(line, modifications)) {
        std::cout << "Invalid\n";
        return;
    }
    if (modifications.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    if (!isValidModifications(modifications, currentISBN)) {
        std::cout << "Invalid\n";
        return;
    }

    vector<Book> book = bookStorage->Find(currentISBN);
    if (book.empty()) {
        std::cout << "Invalid\n";
        return;
    }

    Book oldBook = book[0];
    Book newBook = oldBook;
    for (auto it = modifications.begin(); it != modifications.end(); ++it) {
        string field = it->first;
        string value = it->second;
        if (field == "ISBN") {
            strcpy(newBook.ISBN, value.c_str());
        } else if (field == "name") {
            strcpy(newBook.name, value.c_str());
        } else if (field == "author") {
            strcpy(newBook.author, value.c_str());
        } else if (field == "keyword") {
            strcpy(newBook.keyword, value.c_str());
        } else if (field == "price") {
            char* end;
            double price = strtod(value.c_str(), &end);
            newBook.price = price;
        }
    }

    if (modifications.count("ISBN")) {
        accountSystem->setSelectedBook(newBook.ISBN);
        bookStorage->Delete(currentISBN, oldBook);
        deleteFromIndexList(oldBook);
        bookStorage->Insert(newBook.ISBN, newBook);
        insertToIndexList(newBook);
    } else {
        bookStorage->Delete(currentISBN, oldBook);
        bookStorage->Insert(newBook.ISBN, newBook);
        UpdateIndexList(oldBook, newBook);
    }
}


void BookSystem::import(int quantity, double totalCost) {
    if (!(quantity > 0 && totalCost > 0.0)) {
        std::cout << "Invalid\n";
        return;
    }
    if (!accountSystem->hasPrivilege(3)) {
        std::cout << "Invalid\n";
        return;
    }
    string ISBN = accountSystem->getSelectedBook();
    if (ISBN.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    vector<Book> book = bookStorage->Find(ISBN);
    if (book.empty()) {
        std::cout << "Invalid\n";
        return;
    }
    Book bookToImport = book[0];
    totalCost = -totalCost;
    financeSystem->recordTransaction(totalCost);
    bookToImport.quantity += quantity;
    bookStorage->Delete(ISBN, book[0]);
    bookStorage->Insert(ISBN, bookToImport);
}
