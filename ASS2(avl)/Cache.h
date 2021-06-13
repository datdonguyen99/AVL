#ifndef CACHE_H
#define CACHE_H

#include "main.h"

enum BFactor { LH = -1, EH = 0, RH = 1 };

struct ElemNode {
  Elem *e;
  int balance;
  ElemNode *left;
  ElemNode *right;

  ElemNode() : e(NULL), balance(EH), left(NULL), right(NULL) {}
  ElemNode(Elem *e) : e(e), balance(EH), left(NULL), right(NULL) {}

  int getAddress() { return e->addr; }
};

class ElemTree {
private:
  ElemNode *root;
  int size;

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
      } else
        node->balance = node->right->balance = node->left->balance = EH;
      return true;
    }
    rotateRight(node);
    node->balance = RH;
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

  bool insert(ElemNode *&node, Elem *e) {
    if (!node) {
      node = new ElemNode(e);
      return true;
    }
    if (e->addr < node->getAddress()) {
      if (insert(node->left, e))
        return !balanceLeft(node);
      return false;
    } else {
      if (insert(node->right, e))
        return !balanceRight(node);
      return false;
    }
  }

  bool remove(ElemNode *&node, Elem *e) {
    if (!node) {
      size++;
      return false;
    }
    if (node->getAddress() == e->addr) {
      if (!node->left && !node->right) {
        delete node;
        node = NULL;
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
      while (temp->left)
        temp = temp->left;
      node->e = temp->e;
      temp->e = e;
      if (remove(node->right, e))
        return balanceLeft(node);
      else
        return false;
    }
    if (e->addr < node->getAddress()) {
      if (remove(node->left, e))
        return balanceRight(node);
      else
        return false;
    } else {
      if (remove(node->right, e))
        return balanceLeft(node);
      else
        return false;
    }
  }

  Elem *search(ElemNode *node, int address) {
    if (!node)
      return NULL;
    if (node->getAddress() == address)
      return node->e;
    else if (address < node->getAddress())
      return search(node->left, address);
    else
      return search(node->right, address);
  }

  void clear(ElemNode *&node) {
    if (!node)
      return;
    if (node->left)
      clear(node->left);
    if (node->right)
      clear(node->right);
    delete node;
    node = NULL;
  }

  void preOrder(ElemNode *node) {
    if (!node)
      return;
    node->e->print();
    preOrder(node->left);
    preOrder(node->right);
  }

  void inOrder(ElemNode *node) {
    if (!node)
      return;
    inOrder(node->left);
    node->e->print();
    inOrder(node->right);
  }

  void postOrder(ElemNode *node) {
    if (!node)
      return;
    postOrder(node->left);
    postOrder(node->right);
    node->e->print();
  }

public:
  ElemTree() {
    root = NULL;
    size = 0;
  }

  ~ElemTree() {
    clear(root);
    size = 0;
  }

  void insert(Elem *e) {
    insert(root, e);
    size++;
  }

  void remove(Elem *e) {
    remove(root, e);
    size--;
  }

  Elem *search(int address) { return search(root, address); }

  void preOrder() { preOrder(root); }

  void inOrder() { inOrder(root); }

  void postOrder() { postOrder(root); }
};

class Cache {
  Elem **arr;
  int p;
  int maxSize;
  ElemTree searchTree;

public:
  Cache(int s) {
    arr = new Elem *[s];
    for (int i = 0; i < s; i++)
      arr[i] = NULL;
    p = 0;
    maxSize = s;
  }
  ~Cache() {
    for (int i = 0; i < maxSize; i++)
      if (arr[i])
        delete arr[i];
    delete[] arr;
  }
  Data *read(int addr);
  Elem *put(int addr, Data *cont, bool sync);
  Elem *write(int addr, Data *cont);
  void print();
  void preOrder();
  void inOrder();
  void postOrder();
};

#endif