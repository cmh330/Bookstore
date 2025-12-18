//
// Created by Administrator on 2025/12/8.
//
#ifndef BOOKSTORE_STORAGE_CPP
#define BOOKSTORE_STORAGE_CPP

#include "storage.h"
#include <iostream>

template<int KeyLength, typename ValueType>
KeyValue<KeyLength, ValueType>::KeyValue() : value() {
    for (int i = 0; i < sizeof(index) / sizeof(index[0]); i++) {
        index[i] = 0;
    }
}
template<int KeyLength, typename ValueType>
KeyValue<KeyLength, ValueType>::KeyValue(const string &name, const ValueType& val) : value(val) {
    for (int i = 0; i < sizeof(index) / sizeof(index[0]); i++) {
        index[i] = 0;
    }
    for (int i = 0; i < KeyLength && i < name.length(); i++) {
        index[i] = name[i];
    }
}

template<int KeyLength, typename ValueType>
bool KeyValue<KeyLength, ValueType>::operator<(const KeyValue& other) const {
    // 优先按index字典序升序
    int comparison = strcmp(index, other.index);
    if (comparison != 0) {
        return comparison < 0;
    }
    return value < other.value;
}
template<int KeyLength, typename ValueType>
bool KeyValue<KeyLength, ValueType>::operator==(const KeyValue& other) const {
    return (strcmp(index, other.index) == 0 && value == other.value);
}
template<int KeyLength, typename ValueType>
bool KeyValue<KeyLength, ValueType>::operator<=(const KeyValue& other) const {
    return (*this < other) || (*this == other);
}



// Block
template<int KeyLength, typename ValueType>
Block<KeyLength, ValueType>::Block() : size(0), next(-1) {};


// BlockedLinkList
template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::readBlock(int pos, Block<KeyLength, ValueType> &block) {
        // pos: 0based
        file.seekg(3 * sizeof(int) + pos * sizeof(Block<KeyLength, ValueType>));
        file.read(reinterpret_cast<char *>(&block), sizeof(Block<KeyLength, ValueType>));
    }

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::writeBlock(int pos, Block<KeyLength, ValueType> &block) {
    file.seekp(3 * sizeof(int) + pos * sizeof(Block<KeyLength, ValueType>));
    file.write(reinterpret_cast<char *>(&block), sizeof(Block<KeyLength, ValueType>));
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::updateHead(int New) {
    head = New;
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&head), sizeof(int));
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::updateBlockCount(int New) {
    blockCount = New;
    file.seekp(sizeof(int));
    file.write(reinterpret_cast<char *>(&blockCount), sizeof(int));
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::updateFreeList(int New) {
    freeList = New;
    file.seekp(2 * sizeof(int));
    file.write(reinterpret_cast<char *>(&freeList), sizeof(int));
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::updateAll() {
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&head), sizeof(int));
    file.write(reinterpret_cast<char *>(&blockCount), sizeof(int));
    file.write(reinterpret_cast<char *>(&freeList), sizeof(int));
}


template<int KeyLength, typename ValueType>
int BlockLinkedList<KeyLength, ValueType>::AllocateBlock() {
    // 有free，用
    if (freeList != -1) {
        int pos = freeList;
        Block<KeyLength, ValueType> block;
        readBlock(pos, block);
        freeList = block.next;
        updateFreeList(freeList);
        return pos;
    }
    // 没有free，尾部加一个，这里不建立连接，调用的地方建立
    int pos = blockCount;
    ++blockCount;
    updateBlockCount(blockCount);
    return pos;
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::FreeBlock(int pos) {
    Block<KeyLength, ValueType> block;
    block.next = freeList;
    freeList = pos;
    writeBlock(pos, block);
    updateFreeList(freeList);
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::MergeBlock(int pos, Block<KeyLength, ValueType> &block) {
    if (block.next == -1 || block.size > BLOCK_SIZE * 2 / 3) return;
    Block<KeyLength, ValueType> next;
    readBlock(block.next, next);
    if (block.size + next.size < BLOCK_SIZE) {
        for (int i = 0; i < next.size; ++i) {
            block.data[block.size] = next.data[i];
            ++block.size;
        }

        int temp = block.next;
        block.next = next.next;
        writeBlock(pos, block);
        FreeBlock(temp);
    }
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::SplitBlock(int pos, Block<KeyLength, ValueType> &block) {
    // if (block.size < BLOCK_SIZE) return;
    int newPos = AllocateBlock();
    Block<KeyLength, ValueType> newBlock;
    int mid = block.size / 2;
    newBlock.size = block.size - mid;
    for (int i = 0; i < newBlock.size; ++i) {
        newBlock.data[i] = block.data[mid + i];
    }

    block.size = mid;
    newBlock.next = block.next;
    block.next = newPos;
    writeBlock(newPos, newBlock);
    writeBlock(pos, block);
}

template<int KeyLength, typename ValueType>
int BlockLinkedList<KeyLength, ValueType>::BinarySearch(Block<KeyLength, ValueType> &block, KeyValue<KeyLength, ValueType> &goal) {
    int l = 0, r = block.size;
    while (l < r) {
        // [l, r)
        int mid = (l + r) / 2;
        if (block.data[mid] < goal) {
            l = mid + 1;
        } else {
            r = mid;
        }
    }
    return l;
}


template<int KeyLength, typename ValueType>
BlockLinkedList<KeyLength, ValueType>::BlockLinkedList(const string &name) : fileName(name), head(-1), freeList(-1), blockCount(0) {
    file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        file.open(fileName, std::ios::out);
        file.close();
        file.open(fileName, std::ios::out | std::ios::in | std::ios::binary);
        file.seekp(0);
        file.write(reinterpret_cast<const char *>(&head), sizeof(head));
        file.write(reinterpret_cast<const char *>(&blockCount), sizeof(blockCount));
        file.write(reinterpret_cast<const char *>(&freeList), sizeof(freeList));
        //file.flush();
    } else {
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&head), sizeof(head));
        file.read(reinterpret_cast<char *>(&blockCount), sizeof(blockCount));
        file.read(reinterpret_cast<char *>(&freeList), sizeof(freeList));
    }
}

template<int KeyLength, typename ValueType>
BlockLinkedList<KeyLength, ValueType>::~BlockLinkedList() {
    file.close();
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::Insert(const string &index, const ValueType& value) {
    KeyValue<KeyLength, ValueType> insert(index, value);
        if (head == -1) {
            head = AllocateBlock();
            updateHead(head);
            Block<KeyLength, ValueType> block;
            block.size = 1;
            block.data[0] = insert;
            writeBlock(head, block);
            return;
        }
        bool success = false;
        while (!success) {
            int pos = head;
            int previousPos = -1;
            while (pos != -1) {
                Block<KeyLength, ValueType> block;
                readBlock(pos, block);

                // 满足以下条件，表示可以放进该block
                if (block.size == 0 || insert <= block.data[block.size - 1]) {

                    // 要满了，分裂
                    if (block.size >= BLOCK_SIZE) {
                        SplitBlock(pos, block);
                        break;
                    }

                    // 在当前块寻找合适插入位置
                    int insertPos = BinarySearch(block, insert);
                    // 检查有没有重复
                    if (block.data[insertPos] == insert) {
                        return;
                    }
                    for (int i = block.size; i > insertPos; --i) {
                        block.data[i] = block.data[i - 1];
                    }
                    block.data[insertPos] = insert;
                    ++block.size;
                    writeBlock(pos, block);

                    if (block.size >= BLOCK_SIZE * 2 / 3) {
                        SplitBlock(pos, block);
                    }

                    success = true;
                    break;
                }
                previousPos = pos;
                pos = block.next;
            }
            // 遍历结束还没成功，插入最后一个块
            if (previousPos != -1 && !success) {
                Block<KeyLength, ValueType> block;
                readBlock(previousPos, block);
                for (int i = 0; i < block.size; ++i) {
                    if (block.data[i] == insert) {
                        return;
                    }
                }
                if (block.size >= BLOCK_SIZE * 2 / 3) {
                    SplitBlock(previousPos, block);
                    continue;
                }
                block.data[block.size] = insert;
                ++block.size;
                writeBlock(previousPos, block);
                if (block.size >= BLOCK_SIZE * 2 / 3) {
                    SplitBlock(previousPos, block);
                }
                success = true;
            }
        }
}

template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::Delete(const string &index, const ValueType& value) {
    KeyValue<KeyLength, ValueType> toDelete(index, value);
    int pos = head;
    int previousPos = -1;
    while (pos != -1) {
        Block<KeyLength, ValueType> block;
        readBlock(pos, block);

        int deletePos = BinarySearch(block, toDelete);
        if (deletePos < block.size && block.data[deletePos] == toDelete) {
            for (int j = deletePos; j < block.size - 1; ++j) {
                block.data[j] = block.data[j + 1];
            }
            --block.size;

            if (block.size == 0 && pos != head) {
                if (previousPos != -1) {
                    Block<KeyLength, ValueType> previousBlock;
                    readBlock(previousPos, previousBlock);
                    previousBlock.next = block.next;
                    writeBlock(previousPos, previousBlock);
                }
                FreeBlock(pos);
            } else if (block.size == 0 && pos == head) {
                head = block.next;
                updateHead(head);
                FreeBlock(pos);
            } else {
                writeBlock(pos, block);
                MergeBlock(pos, block);
            }
            return;
        }
        previousPos = pos;
        pos = block.next;
    }
}

template<int KeyLength, typename ValueType>
vector<ValueType> BlockLinkedList<KeyLength, ValueType>::Find(const string &index) {
    vector<ValueType> result;
    int pos = head;

    while (pos != -1) {
        Block<KeyLength, ValueType> block;
        readBlock(pos, block);

        if (block.size > 0 && strcmp(block.data[block.size - 1].index, index.c_str()) < 0) {
            pos = block.next;
            continue;
        }

        if (block.size > 0 && strcmp(block.data[0].index, index.c_str()) > 0) {
            break;
        }

        KeyValue<KeyLength, ValueType> goal(index, ValueType());
        int findPos = BinarySearch(block, goal);
        for (int i = findPos; i < block.size; ++i) {
            int compare = strcmp(index.c_str(), block.data[i].index);
            if (compare == 0) {
                result.push_back(block.data[i].value);
            } else if (compare < 0) {
                break;
            }
        }

        pos = block.next;
    }

    return result;
}


template<int KeyLength, typename ValueType>
void BlockLinkedList<KeyLength, ValueType>::FindPrint(const string &index) {
    bool found = false;
    int pos = head;

    while (pos != -1) {
        Block<KeyLength, ValueType> block;
        readBlock(pos, block);

        // 如果该块最后一个元素小于目标，则跳过
        if (block.size > 0 && strcmp(block.data[block.size - 1].index, index.c_str()) < 0) {
            pos = block.next;
            continue;
        }

        // 如果该块第一个元素大于目标，后续也不会有符合条件的了
        if (block.size > 0 && strcmp(block.data[0].index, index.c_str()) > 0) {
            break;
        }

        KeyValue<KeyLength, ValueType> goal(index, ValueType());
        int findPos = BinarySearch(block, goal);
        for (int i = findPos; i < block.size; ++i) {
            int compare = strcmp(index.c_str(), block.data[i].index);
            if (compare == 0) {
                if (found) std::cout << ' ';
                found = true;
                std::cout << block.data[i].value;
            } else if (compare < 0) {
                break;
            }
        }

        pos = block.next;
    }

    if (!found) {
        std::cout << "null\n";
    }
    else std::cout << '\n';
}

template<int KeyLength, typename ValueType>
vector<ValueType> BlockLinkedList<KeyLength, ValueType>::GetAll() {
    vector<ValueType> result;
    int pos = head;
    while (pos != -1) {
        Block<KeyLength, ValueType> block;
        readBlock(pos, block);
        for (int i = 0; i < block.size; ++i) {
            result.push_back(block.data[i].value);
        }
        pos = block.next;
    }
    return result;
}

#endif //BOOKSTORE_STORAGE_CPP