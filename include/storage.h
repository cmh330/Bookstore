//
// Created by Administrator on 2025/12/8.
//

#ifndef BOOKSTORE_STORAGE_H
#define BOOKSTORE_STORAGE_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;

const int BLOCK_SIZE = 600;

template<int KeyLength, typename ValueType>
struct KeyValue {
    char index[KeyLength + 1];
    ValueType value;

    KeyValue();
    KeyValue(const string &name, const ValueType& val);

    bool operator<(const KeyValue& other) const;
    bool operator==(const KeyValue& other) const;
    bool operator<=(const KeyValue& other) const;
};

template<int KeyLength, typename ValueType>
struct Block {
    KeyValue<KeyLength, ValueType> data[BLOCK_SIZE];
    int size; // 目前该块中元素个数
    int next; // 下一个在文件中的位置，-1：没有

    Block();
};

template<int KeyLength, typename ValueType>
class BlockLinkedList {
private:
    fstream file;
    string fileName;
    int head;
    int blockCount;
    int freeList; // 空链表表头

    void readBlock(int pos, Block<KeyLength, ValueType> &block);
    void writeBlock(int pos, Block<KeyLength, ValueType> &block);
    void updateHead(int New);
    void updateBlockCount(int New);
    void updateFreeList(int New);
    void updateAll();

    int AllocateBlock();
    void FreeBlock(int pos);
    void MergeBlock(int pos, Block<KeyLength, ValueType> &block);
    void SplitBlock(int pos, Block<KeyLength, ValueType> &block);
    int BinarySearch(Block<KeyLength, ValueType> &block, KeyValue<KeyLength, ValueType> &goal);

public:
    BlockLinkedList(const string &name);
    ~BlockLinkedList();


    void Insert(const string &index, const ValueType& value);
    void Delete(const string &index, const ValueType& value);
    vector<ValueType> Find(const string &index);
    void FindPrint(const string &index);
};

#include "storage.cpp"
#endif //BOOKSTORE_STORAGE_H