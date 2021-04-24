#include "Cache.h"

Data *Cache::read(int addr) {
  Elem *found = searchTree.search(addr);
  if (found)
    return found->data;
  else
    return NULL;
}
Elem *Cache::put(int addr, Data *cont, bool sync = true) {
  Elem *removed = arr[p];
  if (removed)
    searchTree.remove(removed);
  arr[p] = new Elem(addr, cont, sync);
  searchTree.insert(arr[p]);
  p = (p + 1) % maxSize;
  return removed;
}
Elem *Cache::write(int addr, Data *cont) {
  Elem *found = searchTree.search(addr);
  if (found) {
    delete found->data;
    found->data = cont;
    found->sync = false;
    return NULL;
  } else
    return put(addr, cont, false);
}
void Cache::print() {
  if (!arr[p]) {
    for (int i = 0; i < p; i++)
      arr[i]->print();
  } else {
    for (int i = 0; i < maxSize; i++) {
      arr[(p + i) % maxSize]->print();
    }
  }
}
void Cache::preOrder() { searchTree.preOrder(); }
void Cache::inOrder() { searchTree.inOrder(); }
void Cache::postOrder() { searchTree.postOrder(); }