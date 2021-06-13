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
  int head;

public:
  FIFO() {
    arr = new Elem *[MAXSIZE]();
    count = head = 0;
  }
  ~FIFO() {
    for (int i = 0; i < count; i++) {
      delete arr[(head + i) % count];
    }
    delete[] arr;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    count++;
    arr[idx] = e;
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
    int idx = head++;
    head = head % MAXSIZE;
    count--;
    return idx;
  }
  void print() {
    for (int i = 0; i < count; i++) {
      arr[(head + i) % count]->print();
    }
  }
};

class MRU : public ReplacementPolicy {
private:
  struct Node {
    int idx;
    Node *next;
    Node *prev;
    Node(int idx, Node *next = nullptr, Node *prev = nullptr)
        : idx(idx), next(next), prev(prev) {}
  } * head, *tail;

public:
  MRU() {
    arr = new Elem *[MAXSIZE]();
    count = 0;
    head = tail = nullptr;
  }
  ~MRU() {
    for (int i = 0; i < count; i++) {
      Node *temp = head->next;
      delete arr[head->idx];
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
    for (int i = 0; i < count; temp = temp->next, i++) {
      if (temp->idx == idx) {
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
    int idx = temp->idx;
    delete temp;
    count--;
    return idx;
  }
  void print() {
    Node *temp = head;
    for (int i = 0; i < count; temp = temp->next, i++) {
      arr[temp->idx]->print();
    }
  }
};

class LRU : public ReplacementPolicy {
private:
  struct Node {
    int idx;
    Node *next;
    Node *prev;
    Node(int idx, Node *next = nullptr, Node *prev = nullptr)
        : idx(idx), next(next), prev(prev) {}
  } * head, *tail;

public:
  LRU() {
    arr = new Elem *[MAXSIZE]();
    count = 0;
    head = tail = nullptr;
  }
  ~LRU() {
    for (int i = 0; i < count; i++) {
      Node *temp = head->next;
      delete arr[head->idx];
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
    for (int i = 0; i < count; temp = temp->next, i++) {
      if (temp->idx == idx) {
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
    int idx = temp->idx;
    delete temp;
    count--;
    return idx;
  }
  void print() {
    Node *temp = head;
    for (int i = 0; i < count; temp = temp->next, i++) {
      arr[temp->idx]->print();
    }
  }
};

class LFU : public ReplacementPolicy {
private:
  struct Node {
    int idx, count;
    Node(int idx, int count = 1) : idx(idx), count(count) {}
  } * *head;

  void heapDown(int parent) {
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
          heapDown(child);
        }
      }
    }
  }

  void heapUp(int child) {
    if (child > 0) {
      int parent = (child - 1) / 2;
      if (parent >= 0 && head[parent]->count > head[child]->count) {
        Node *parentNode = head[parent];
        Node *childNode = head[child];
        head[parent] = childNode;
        head[child] = parentNode;
        heapUp(parent);
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
      delete arr[head[i]->idx];
      delete head[i];
    }
    delete[] arr;
    delete[] head;
  }
  int insert(Elem *e, int idx) {
    idx = (idx == -1) ? count : idx;
    head[count++] = new Node(idx);
    heapUp(count - 1);
    arr[idx] = e;
    return idx;
  }
  void access(int idx) {
    if (idx < 0 || idx >= MAXSIZE) {
      return;
    }
    for (int i = 0; i < count; i++) {
      if (head[i]->idx == idx) {
        head[i]->count++;
        heapDown(i);
        break;
      }
    }
  }
  int remove() {
    if (!isFull()) {
      return -1;
    }
    int idx = head[0]->idx;
    delete head[0];
    if (--count > 0) {
      head[0] = head[count];
      heapDown(0);
    }
    head[count] = nullptr;
    return idx;
  }
  void print() {
    for (int i = 0; i < count; i++)
      arr[head[i]->idx]->print();
  }
};

class DBHashing : public SearchEngine {
private:
  struct Node {
    int address, idx;
    Node(int address, int idx) : address(address), idx(idx) {}
  } * *head;

  int (*hash1)(int);
  int (*hash2)(int);
  int size;

  int hash(int address, int i) {
    int k1 = hash1(address);
    int k2 = hash2(address);
    return ((k1 + i * k2) % size + size) % size;
  }

public:
  DBHashing(int (*hash1)(int), int (*hash2)(int), int size)
      : hash1(hash1), hash2(hash2) {
    head = new Node *[(this->size = size)]();
  }
  ~DBHashing() {
    for (int i = 0; i < size; i++)
      if (head[i] != nullptr) {
        delete head[i];
      }
    delete[] head;
  }
  void insert(Elem *e, int idx) {
    for (int i = 0; i < size; i++) {
      int temp = hash(e->addr, i);
      if (head[temp] == nullptr) {
        head[temp] = new Node(e->addr, idx);
        break;
      }
    }
  }
  void deleteNode(Elem *e) {
    if (e == nullptr) {
      return;
    }
    for (int i = 0; i < size; i++) {
      int temp = hash(e->addr, i);
      if (head[temp] != nullptr && head[temp]->address == e->addr) {
        delete head[temp];
        head[temp] = nullptr;
        break;
      }
    }
  }
  void print(ReplacementPolicy *q) {
    cout << "Prime memory:" << endl;
    for (int i = 0; i < size; i++) {
      if (head[i] != nullptr)
        q->getValue(head[i]->idx)->print();
    }
  }
  int search(int address) {
    int idx = -1;
    for (int i = 0; i < size; i++) {
      int temp = hash(address, i);
      if (head[temp] != nullptr && head[temp]->address == address) {
        idx = head[temp]->idx;
        break;
      }
    }
    return idx;
  }
};
class AVL : public SearchEngine {
private:
  enum BFactor { LH = -1, EH = 0, RH = 1 };
  struct Node {
    int address, idx;
    BFactor balance;
    Node *left, *right;

    Node(int address, int idx)
        : address(address), idx(idx), balance(EH), left(nullptr),
          right(nullptr) {}
    int getAddress() { return address; }
  } * root;

  void rotateRight(Node *&node) {
    Node *temp = node;
    node = node->left;
    temp->left = node->right;
    node->right = temp;
  }

  void rotateLeft(Node *&node) {
    Node *temp = node;
    node = node->right;
    temp->right = node->left;
    node->left = temp;
  }

  bool balanceRight(Node *&node) {
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
      rotateRight(node->right);
      rotateLeft(node);
      if (node->balance == RH) {
        node->balance = node->right->balance = EH;
        node->left->balance = LH;
      } else if (node->balance == LH) {
        node->balance = node->left->balance = EH;
        node->right->balance = RH;
      } else
        node->balance = node->left->balance = node->right->balance = EH;
      return true;
    }
    rotateLeft(node);
    node->balance = LH;
    node->left->balance = RH;
    return false;
  }

  bool balanceLeft(Node *&node) {
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
      } else
        node->balance = node->right->balance = node->left->balance = EH;
      return true;
    }
    rotateRight(node);
    node->balance = RH;
    node->right->balance = LH;
    return false;
  }

  int search(Node *node, int address) {
    if (!node) {
      return -1;
    }
    if (node->getAddress() == address) {
      return node->idx;
    } else if (address < node->getAddress()) {
      return search(node->left, address);
    } else {
      return search(node->right, address);
    }
  }

  bool insert(Node *&node, int address, int idx) {
    if (!node) {
      node = new Node(address, idx);
      return true;
    }
    if (address < node->getAddress()) {
      if (insert(node->left, address, idx)) {
        return !balanceLeft(node);
      }
      return false;
    } else {
      if (insert(node->right, address, idx)) {
        return !balanceRight(node);
      }
      return false;
    }
  }

  bool remove(Node *&node, int address) {
    if (!node) {
      return false;
    }
    if (node->getAddress() == address) {
      if (!node->left && !node->right) {
        delete node;
        node = nullptr;
        return true;
      } else if (!node->left) {
        Node *temp = node;
        node = node->right;
        delete temp;
        return true;
      } else if (!node->right) {
        Node *temp = node;
        node = node->left;
        delete temp;
        return true;
      }
      Node *temp = node->right;
      while (temp->left) {
        temp = temp->left;
      }
      node->address = temp->address;
      node->idx = temp->idx;
      temp->address = address;
      if (remove(node->right, address)) {
        return balanceLeft(node);
      } else {
        return false;
      }
    }
    if (address < node->getAddress()) {
      if (remove(node->left, address)) {
        return balanceRight(node);
      } else {
        return false;
      }
    } else {
      if (remove(node->right, address)) {
        return balanceLeft(node);
      } else {
        return false;
      }
    }
  }

  void preOrder(ReplacementPolicy *q, Node *node) {
    if (!node) {
      return;
    }
    q->getValue(node->idx)->print();
    preOrder(q, node->left);
    preOrder(q, node->right);
  }

  void inOrder(ReplacementPolicy *q, Node *node) {
    if (!node) {
      return;
    }
    inOrder(q, node->left);
    q->getValue(node->idx)->print();
    inOrder(q, node->right);
  }

  void clear(Node *&node) {
    if (!node) {
      return;
    }
    if (node->left) {
      clear(node->left);
    }
    if (node->right) {
      clear(node->right);
    }
    delete node;
    node = nullptr;
  }

public:
  AVL() { root = nullptr; }
  ~AVL() { clear(root); }
  void insert(Elem *e, int idx) { insert(root, e->addr, idx); }
  void deleteNode(Elem *e) {
    if (e != nullptr)
      remove(root, e->addr);
  }
  void print(ReplacementPolicy *q) {
    cout << "Print AVL in inorder:" << endl;
    this->inOrder(q, root);
    cout << "Print AVL in preorder:" << endl;
    this->preOrder(q, root);
  }
  int search(int address) {
    int idx = search(root, address);
    return idx;
  }
};

#endif