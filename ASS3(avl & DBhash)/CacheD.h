#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class ReplacementPolicy {
protected:
  int count;
  Elem **arr;

public:
  virtual ~ReplacementPolicy() {}
  virtual int insert(Elem *e,
                     int idx) = 0; // insert e into arr[idx] if idx != -1 else
                                   // into the position by replacement policy
  virtual void
  access(int idx) = 0; // idx is index in the cache of the accessed element
  virtual int remove() = 0;
  virtual void print() = 0;

  bool isFull() { return count == MAXSIZE; }
  bool isEmpty() { return count == 0; }
  Elem *getValue(int idx) {
    return (idx >= 0 && idx < MAXSIZE) ? arr[idx] : nullptr;
  }
  // void replace(int idx, Elem *e) {
  //   delete arr[idx];
  //   access(idx);
  //   arr[idx] = e;
  // }
};

class SearchEngine {
public:
  virtual ~SearchEngine() {}
  virtual int search(int key) = 0; // -1 if not found
  virtual void insert(Elem *e, int idx) = 0;
  virtual void deleteNode(Elem *e) = 0;
  virtual void print(ReplacementPolicy *r) = 0;
};

class FIFO : public ReplacementPolicy {
private:
  int cnt;

public:
  FIFO() {
    arr = new Elem *[MAXSIZE]();
    count = cnt = 0;
    for (int i = 0; i < MAXSIZE; i++) {
      arr[i] = nullptr;
    }
  }
  ~FIFO() {
    for (int i = 0; i < MAXSIZE; i++) {
      delete arr[(cnt + i) % count];
    }
    delete[] arr;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    arr[idx] = e;
    count++;
    return idx;
  }
  void access(int idx) {
    if (idx < 0 || idx >= MAXSIZE) {
      return;
    }
    if (!arr[idx]) {
      return;
    }
  }
  int remove() {
    if (!isFull()) {
      return -1;
    }
    int idx = cnt++;
    cnt = cnt % MAXSIZE;
    count--;
    return idx;
  }
  void print() {
    for (int i = 0; i < count; i++) {
      arr[(cnt + i) % count]->print();
    }
  }
};

class MRU : public ReplacementPolicy {
private:
  struct Node {
    int index;
    Node *next;
    Node *prev;

    Node(int idx, Node *next = nullptr, Node *prev = nullptr)
        : index(idx), next(next), prev(prev) {}
  } * head, *tail;

public:
  MRU() {
    arr = new Elem *[MAXSIZE]();
    head = tail = nullptr;
    count = 0;
  }
  ~MRU() {
    for (int i = 0; i < count; i++) {
      Node *temp = head->next;
      delete arr[head->index];
      delete head;
      head = temp;
    }
    delete[] arr;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    head = new Node(idx, head, nullptr);
    if (count == 0) {
      tail = head;
    } else {
      head->next->prev = head;
    }
    count++;
    arr[idx] = e;
    return idx;
  }
  void access(int idx) {
    if (idx < 0 || idx >= MAXSIZE) {
      return;
    }
    Node *temp = head;
    for (int i = 0; i < count; i++, temp = temp->next) {
      if (temp->index == idx) {
        if (temp != head) {
          temp->prev->next = temp->next;
          if (temp != tail) {
            temp->next->prev = temp->prev;
          } else {
            tail = temp->prev;
          }
          temp->prev = nullptr;
          temp->next = head;
          head->prev = temp;
          head = temp;
        }
        break;
      }
    }
  }
  int remove() {
    if (!isFull()) {
      return -1;
    }
    Node *temp = head;
    head = temp->next;
    if (temp == tail) {
      tail = nullptr;
    } else {
      temp->next->prev = nullptr;
    }
    int idx = temp->index;
    delete temp;
    count--;
    return idx;
  }
  void print() {
    Node *temp = head;
    for (int i = 0; i < count; i++, temp = temp->next) {
      arr[temp->index]->print();
    }
  }
};

class LRU : public ReplacementPolicy {
private:
  struct Node {
    int index;
    Node *next;
    Node *prev;

    Node(int idx, Node *next = nullptr, Node *prev = nullptr)
        : index(idx), next(next), prev(prev) {}
  } * head, *tail;

public:
  LRU() {
    arr = new Elem *[MAXSIZE]();
    head = tail = nullptr;
    count = 0;
  }
  ~LRU() {
    for (int i = 0; i < count; i++) {
      Node *temp = head->next;
      delete arr[head->index];
      delete head;
      head = temp;
    }
    delete[] arr;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    head = new Node(idx, head, nullptr);
    if (count == 0) {
      tail = head;
    } else {
      head->next->prev = head;
    }
    count++;
    arr[idx] = e;
    return idx;
  }
  void access(int idx) {
    if (idx < 0 || idx >= MAXSIZE) {
      return;
    }
    Node *temp = head;
    for (int i = 0; i < count; i++, temp = temp->next) {
      if (temp->index == idx) {
        if (temp != head) {
          temp->prev->next = temp->next;
          if (temp != tail) {
            temp->next->prev = temp->prev;
          } else {
            tail = temp->prev;
          }
          temp->prev = nullptr;
          temp->next = head;
          head->prev = temp;
          head = temp;
        }
        break;
      }
    }
  }
  int remove() {
    if (!isFull()) {
      return -1;
    }
    Node *temp = tail;
    tail = temp->prev;
    if (temp == head) {
      head = nullptr;
    } else {
      temp->prev->next = nullptr;
    }
    int idx = temp->index;
    delete temp;
    count--;
    return idx;
  }
  void print() {
    Node *temp = head;
    for (int i = 0; i < count; i++, temp = temp->next) {
      arr[temp->index]->print();
    }
  }
};

class LFU : public ReplacementPolicy {
private:
  struct Node {
    int index, count;
    Node(int idx, int cnt = 1) : index(idx), count(cnt) {}
  } * *head;

  void reHeapDown(int parent) {
    if (parent < count) {
      int child = parent * 2 + 1;
      if (child < count) {
        child += (int)(child + 1 < count &&
                       head[child + 1]->count < head[child]->count);
        if (head[parent]->count >= head[child]->count) {
          Node *parentNode = head[parent];
          Node *childNode = head[child];
          head[parent] = childNode;
          head[child] = parentNode;
          reHeapDown(child);
        }
      }
    }
  }

  void reHeapUp(int child) {
    if (child > 0) {
      int parent = (child - 1) / 2;
      if (parent >= 0 && head[parent]->count > head[child]->count) {
        Node *parentNode = head[parent];
        Node *childNode = head[child];
        head[parent] = childNode;
        head[child] = parentNode;
        reHeapDown(parent);
      }
    }
  }

public:
  LFU() {
    arr = new Elem *[MAXSIZE]();
    count = 0;
    head = new Node *[MAXSIZE]();
  }
  ~LFU() {
    for (int i = 0; i < count; i++) {
      delete arr[head[i]->index];
      delete head[i];
    }
    delete[] arr;
    delete[] head;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    head[count++] = new Node(idx);
    reHeapUp(count - 1);
    arr[idx] = e;
    return idx;
  }
  void access(int idx) {
    if (idx < 0 || idx >= MAXSIZE) {
      return;
    }
    for (int i = 0; i < count; i++) {
      if (head[i]->index == idx) {
        head[i]->count++;
        reHeapDown(i);
        break;
      }
    }
  }
  int remove() {
    if (!isFull()) {
      return -1;
    }
    int idx = head[0]->index;
    delete head[0];
    if (--count > 0) {
      head[0] = head[count];
      reHeapDown(0);
    }
    head[count] = nullptr;
    return idx;
  }
  void print() {
    for (int i = 0; i < count; i++) {
      arr[head[i]->index]->print();
    }
  }
};

class DBHashing : public SearchEngine {
private:
  struct NodeHash {
    int address, index;
    NodeHash(int addr, int idx) : address(addr), index(idx) {}
  } * *table;

  int (*hashFunc1)(int);
  int (*hashFunc2)(int);
  int size;

  int DBHashFunc(int addr, int i) {
    int hashVal1 = hashFunc1(addr);
    int hashVal2 = hashFunc2(addr);
    return ((hashVal1 + i * hashVal2) % size + size) % size;
  }

public:
  DBHashing(int (*h1)(int), int (*h2)(int), int s)
      : hashFunc1(h1), hashFunc2(h2) {
    table = new NodeHash *[(this->size = s)];
    for (int i = 0; i < size; i++) {
      table[i] = nullptr;
    }
  }
  ~DBHashing() {
    for (int i = 0; i < size; i++) {
      if (table[i] != nullptr) {
        delete table[i];
      }
    }
    delete[] table;
  }
  void insert(Elem *e, int idx) {
    for (int i = 0; i < size; i++) {
      int tem = DBHashFunc(e->addr, i);
      if (table[tem] == nullptr) {
        table[tem] = new NodeHash(e->addr, idx);
        break;
      }
    }
  }

  void deleteNode(Elem *e) {
    if (e == nullptr) {
      return;
    }
    for (int i = 0; i < size; i++) {
      int tem = DBHashFunc(e->addr, i);
      if (table[tem] != nullptr && table[tem]->address == e->addr) {
        delete table[tem];
        table[tem] = nullptr;
        break;
      }
    }
  }

  void print(ReplacementPolicy *q) {
    cout << "Prime memory:" << endl;
    for (int i = 0; i < size; i++) {
      if (table[i] != nullptr) {
        q->getValue(table[i]->index)->print();
      }
    }
  }

  int search(int addr) {
    int idx = -1;
    for (int i = 0; i < size; i++) {
      int tem = DBHashFunc(addr, i);
      if (table[tem] != nullptr && table[tem]->address == addr) {
        idx = table[tem]->index;
        break;
      }
    }
    return idx;
  }
};

class AVL : public SearchEngine {
private:
  enum BFactor { LH = -1, EH = 0, RH = 1 };

  struct ElemNode {
    int address, index;
    BFactor balance;
    ElemNode *left, *right;

    ElemNode(int addr, int idx)
        : address(addr), index(idx), balance(EH), left(nullptr),
          right(nullptr) {}

    // int getAddress() { return this->address; }
  } * root;

  void rotateLeft(ElemNode *&node) {
    ElemNode *temp = node;
    node = node->right;
    temp->right = node->left;
    node->left = temp;
  }

  void rotateRight(ElemNode *&node) {
    ElemNode *temp = node;
    node = node->left;
    temp->left = node->right;
    node->right = temp;
  }

  bool balanceLeft(ElemNode *&node) {
    if (node->balance == RH) {
      node->balance = EH;
      return true;
    }
    if (node->balance == EH) {
      node->balance = LH;
      return false;
    }
    if (node->left->balance == LH) {
      rotateRight(node);
      node->balance = node->right->balance = EH;
      return true;
    }
    if (node->left->balance == RH) {
      rotateLeft(node->left);
      rotateRight(node);
      if (node->balance == LH) {
        node->balance = node->left->balance = EH;
        node->right->balance = RH;
      } else if (node->balance == RH) {
        node->balance = node->right->balance = EH;
        node->left->balance = LH;
      } else {
        node->balance = node->left->balance = node->right->balance = EH;
      }
      return true;
    }
    rotateRight(node);
    node->balance = EH;
    node->right->balance = LH;
    return false;
  }

  bool balanceRight(ElemNode *&node) {
    if (node->balance == LH) {
      node->balance = EH;
      return true;
    }
    if (node->balance == EH) {
      node->balance = RH;
      return false;
    }
    if (node->right->balance == RH) {
      rotateLeft(node);
      node->balance = node->left->balance = EH;
      return true;
    }
    if (node->right->balance == LH) {
      rotateLeft(node->right);
      rotateRight(node);
      if (node->balance == RH) {
        node->balance = node->right->balance = EH;
        node->left->balance = LH;
      } else if (node->balance == LH) {
        node->balance = node->left->balance = EH;
        node->right->balance = RH;
      } else {
        node->balance = node->left->balance = node->right->balance = EH;
      }
      return true;
    }
    rotateLeft(node);
    node->balance = LH;
    node->left->balance = RH;
    return false;
  }

  bool insert(ElemNode *&node, int addr, int idx) {
    if (!node) {
      node = new ElemNode(addr, idx);
      return true;
    }
    if (addr < node->address) {
      if (insert(node->left, addr, idx)) {
        return !balanceLeft(node);
      }
      return false;
    } else {
      if (insert(node->right, addr, idx)) {
        return !balanceRight(node);
      }
      return false;
    }
  }

  bool remove(ElemNode *&node, int addr) {
    if (!node) {
      return false;
    }
    if (addr == node->address) {
      if (!node->left && !node->right) {
        delete node;
        node = nullptr;
        return true;
      } else if (!node->left) {
        ElemNode *temp = node;
        node = node->right;
        delete temp;
        return true;
      } else if (!node->right) {
        ElemNode *temp = node;
        node = node->left;
        delete temp;
        return true;
      }
      ElemNode *temp = node->right;
      while (temp->left) {
        temp = temp->left;
      }
      node->address = temp->address;
      node->index = temp->index;
      temp->address = addr;

      if (remove(node->right, addr)) {
        return balanceLeft(node);
      } else {
        return false;
      }
    }
    if (addr < node->address) {
      if (remove(node->left, addr)) {
        return balanceRight(node);
      } else {
        return false;
      }
    } else {
      if (remove(node->right, addr)) {
        return balanceLeft(node);
      } else {
        return false;
      }
    }
  }

  bool search(ElemNode *node, int addr) {
    if (!node) {
      return false;
    }
    if (node->address == addr) {
      return true;
    } else if (node->address < addr) {
      return search(node->left, addr);
    } else {
      return search(node->right, addr);
    }
  }

  void clear(ElemNode *&node) {
    if (!node) {
      return;
    }
    if (node->left) {
      clear(node->left);
    } else {
      clear(node->right);
    }
    delete node;
    node = nullptr;
  }

  void preOrder(ReplacementPolicy *r, ElemNode *node) {
    if (!node) {
      return;
    }
    r->getValue(node->index)->print();
    preOrder(r, node->left);
    preOrder(r, node->right);
  }

  void inOrder(ReplacementPolicy *r, ElemNode *node) {
    if (!node) {
      return;
    }
    inOrder(r, node->left);
    r->getValue(node->index)->print();
    inOrder(r, node->right);
  }

public:
  AVL() { root = nullptr; }
  ~AVL() { clear(root); }
  void insert(Elem *e, int i) { insert(root, e->addr, i); }

  void deleteNode(Elem *e) {
    if (e != nullptr) {
      remove(root, e->addr);
    }
  }

  void print(ReplacementPolicy *q) {
    cout << "Print AVL in inorder:" << endl;
    inOrder(q, root);
    cout << "Print AVL in preorder:" << endl;
    preOrder(q, root);
  }

  int search(int addr) {
    int idx = search(root, addr);
    return idx;
  }
};

#endif