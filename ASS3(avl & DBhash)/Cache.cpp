#include "Cache.h"

Cache::Cache(SearchEngine *s, ReplacementPolicy *r) : rp(r), s_engine(s) {}
Cache::~Cache() {
  delete rp;
  delete s_engine;
}

Data *Cache::read(int addr) {
  int idx = s_engine->search(addr);
  Elem *searched = rp->getValue(idx);
  rp->access(idx);
  return (searched != nullptr) ? searched->data : nullptr;
}

Elem *Cache::put(int addr, Data *cont) {
  int idx = rp->remove();
  Elem *deleted = rp->getValue(idx);
  s_engine->deleteNode(deleted);
  Elem *inserted = new Elem(addr, cont, true);
  idx = rp->insert(inserted, idx);
  s_engine->insert(inserted, idx);
  return deleted;
}

Elem *Cache::write(int addr, Data *cont) {
  int idx = s_engine->search(addr);
  Elem *searched = rp->getValue(idx);
  Elem *deleted = nullptr;
  if (searched != nullptr) {
    rp->access(idx);
    Data *data = searched->data;
    searched->data = cont;
    searched->sync = false;
    delete data;
  } else {
    idx = rp->remove();
    deleted = rp->getValue(idx);
    s_engine->deleteNode(deleted);
    Elem *inserted = new Elem(addr, cont, false);
    idx = rp->insert(inserted, idx);
    s_engine->insert(inserted, idx);
  }
  return deleted;
}

void Cache::printRP() { rp->print(); }

void Cache::printSE() { s_engine->print(rp); }
