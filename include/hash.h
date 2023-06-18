//
// Created by creeper on 23-5-30.
//

#ifndef LSPI_HASH_H
#define LSPI_HASH_H

#include <lspi-terminal.h>
#include <lspi-types.h>

#include <utility>

using ull = unsigned long long;

// Mapping a string to ValType
template <uint OuterSize, uint MagicPrime, typename ValType>
class GeneralHashMap {
 public:
  GeneralHashMap() {
    for (int i = 0; i < OuterSize; i++)
      bucket_[i] = nullptr;
  }
  // TODO: maybe we can initialize this at compile time
  void insert(const char* str, ValType val) {
    ull hashval = hash(str);
    uint idx = hashval % OuterSize;
    Node* hd = bucket_[idx];
    bucket_[idx] = new Node(val, hashval, hd);
    sz_++;
  }
  void insert(const char* str, uint n, ValType val) {
    ull hashval = hash(str, n);
    uint idx = hashval % OuterSize;
    Node* hd = bucket_[idx];
    bucket_[idx] = new Node(val, hashval, hd);
    sz_++;
  }
  ull hash(const char* str) const {
    const char* p = str;
    ull sum = 0ull, pw = 1ull;
    for (; *p != '\0'; p++) {
      sum += pw * (*p);
      pw *= MagicPrime;
    }
    return sum;
  }
  bool find(const char* str) const {
    ull hashval = hash(str);
    return FindByHash(hashval);
  }
  ull hash(const char* str, uint n) const {
    ull sum = 0ull, pw = 1ull;
    for (uint i = 0; i < n; i++) {
      sum += pw * str[i];
      pw *= MagicPrime;
    }
    return sum;
  }
  bool find(const char* str, uint n) const {
    ull hashval = hash(str, n);
    return FindByHash(hashval);
  }
  bool FindByHash(ull hashval) const {
    uint idx = hashval % OuterSize;
    Node* p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    return p != nullptr;
  }
  ValType Query(const char* str) const {
    ull hashval = hash(str);
    return QueryByHash(hashval);
  }
  ValType Query(const char* str, uint n) const {
    ull hashval = hash(str, n);
    return QueryByHash(hashval);
  }
  ValType QueryByHash(ull hashval) const {
    uint idx = hashval % OuterSize;
    Node* p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    return p->val;
  }
  ~GeneralHashMap() {
    for (int i = 0; i < OuterSize; i++) {
      Node* ptr = bucket_[i];
      while (ptr != nullptr) {
        Node* prv = ptr;
        ptr = ptr->nxt;
        delete prv;
      }
    }
  }
  uint size() const { return sz_; }
  bool empty() const { return sz_ == 0u; }

 private:
  struct Node {
    ValType val;
    ull hashval;
    Node* nxt = nullptr;
    Node() = default;
    Node(ValType val_, ull hashval_, Node* nd)
        : val(val_), hashval(hashval_), nxt(nd) {}
  };

  uint sz_ = 0;
  Node* bucket_[OuterSize];
};

template <uint N, typename Valtype>
using HashMap = GeneralHashMap<N, 1331, Valtype>;

template <char... alphabet>
struct Alphabet;

// Trie is also one way of mapping
// here I can map a pointer to something
template <typename Valtype, uint Bitlen = (SIZEOF_PTR << 3)>
class BitTrie {
 public:
  BitTrie() { rt_ = new Node; }
  using ull = unsigned long long;
  void insert(ull str, Valtype val) {
    Node* ptr = rt_;
    for (uint i = 0; i < Bitlen; i++) {
      if (ptr->ch[(1 << i) & str] == nullptr)
        ptr->ch[(1 << i) & str] = new Node;
      ptr = ptr->ch[(1 << i) & str];
    }
    ptr->val = val;
    cnt_++;
  }
  bool lookup(ull str, Valtype* res) {
    Node* ptr = rt_;
    if (ptr == nullptr) return false;
    for (uint i = 0; i < Bitlen; i++) ptr = ptr->ch[(1 << i) & str];
    if (ptr == nullptr) return false;
    *res = ptr->val;
    return true;
  }
  bool lookup(ull str) {
    Node* ptr = rt_;
    if (ptr == nullptr) return false;
    for (uint i = 0; i < Bitlen; i++) ptr = ptr->ch[(1 << i) & str];
    return ptr != nullptr;
  }
  uint StrCnt() const { return cnt_; }

 private:
  struct Node {
    Valtype val;
    Node* ch[2];
    Node() { ch[0] = ch[1] = nullptr; }
  };
  uint cnt_ = 0;
  Node* rt_ = nullptr;
};

template <uint Bitlen>
class BitTrie<bool, Bitlen> {
 public:
  BitTrie() { rt_ = new Node; }
  void insert(void* str) {
    Node* ptr = rt_;
    auto x = reinterpret_cast<std::size_t>(str);
    for (uint i = 0; i < Bitlen; i++) {
      if (ptr->ch[x & 1] == nullptr)
        ptr->ch[x & 1] = new Node;
      ptr = ptr->ch[x & 1];
      x >>= 1;
    }
    cnt_++;
  }
  bool lookup(void* str) {
    Node* ptr = rt_;
    auto x = reinterpret_cast<std::size_t>(str);
    for (uint i = 0; i < Bitlen; i++) {
      ptr = ptr->ch[x & 1];
      if (ptr == nullptr) return false;
      x >>= 1;
    }
    return ptr != nullptr;
  }
  uint StrCnt() const { return cnt_; }

 private:
  struct Node {
    Node* ch[2];
    Node() { ch[0] = ch[1] = nullptr; }
  };
  uint cnt_ = 0;
  Node* rt_ = nullptr;
};

#endif  // LSPI_HASH_H
