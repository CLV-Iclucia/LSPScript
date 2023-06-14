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
    for (int i = 0; i < OuterSize; i++) bucket_[i] = nullptr;
  }
  // TODO: maybe we can initialize this at compile time
  void insert(const char* str, ValType val) {
    ull hashval = hash(str);
    uint idx = hashval % OuterSize;
    Node* hd = bucket_[idx];
    bucket_[idx] = new Node(val, hashval, hd);
    sz_++;
  }
  ull hash(const char* str) {
    const char* p = str;
    ull sum = 0ull, pw = 1ull;
    for (; *p != '\0'; p++) {
      sum += pw * OuterSize + (*p);
      pw *= MagicPrime;
    }
    return sum;
  }
  bool find(const char* str) const {
    ull hashval = hash(str);
    uint idx = hashval % OuterSize;
    Node *p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    if (p != nullptr) return true;
  }
  uint hash(const char* str, uint n) const {
    ull sum = 0ull, pw = 1ull;
    for (uint i = 0; i < n; i++) {
      sum += pw * OuterSize + str[i];
      pw *= MagicPrime;
    }
    return sum;
  }
  bool find(const char* str, uint n) const {
    ull hashval = hash(str, n);
    uint idx = hashval % OuterSize;
    Node* p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    return p != nullptr;
  }
  bool FindByHash(ull hashval) const {
    uint idx = hashval % OuterSize;
    Node* p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    return p != nullptr;
  }
  ValType Query(const char* str) const {
    ull hashval = hash(str);
    uint idx = hashval % OuterSize;
    Node* p = bucket_[idx];
    while (p && p->hashval != hashval) p = p->nxt;
    if (p) return p->val;
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

template <char... alphabet> struct Alphabet;

template <char a, char... alphabet>
struct Alphabet<a, alphabet...> : public Alphabet<alphabet...> {
  using Tail = Alphabet<alphabet...>;
  static constexpr uint size = Tail::map(a) >= 0 ? Tail::size : Tail::size + 1;
  static constexpr int map(char c) {
    if constexpr (size == 1) return a == c ? 0 : -1;
    if constexpr (Tail::map(c) > 0)
      return Tail::map(c);
    else
      return size;
  }
};

template <> struct Alphabet<> {
  static constexpr uint size = 0;
  static constexpr int map(char c) {
    return -1;
  }
};

// Trie is also one way of mapping strings to constants
template <typename Valtype, char... charSet>
class Trie {
 public:
  void insert(const char* str, Valtype val) {
    const char* p = str;
    Node* ptr = rt_;
    if(rt_ == nullptr) rt_ = new Node;
    while (*p != '\0') {
      uint idx = ptr->ch[AlphabetMap::map(*p)];
      if(ptr->ch[idx])
        ptr->ch[idx] = new Node;
      ptr = ptr->ch[idx];
      p++;
    }
    ptr->val = val;
    cnt_++;
  }
  bool lookup(const char* str, Valtype* res) {
    Node* ptr = rt_;
    if(ptr == nullptr) return false;
    const char *p = str;
    while(*p != '\0' && ptr->ch[AlphabetMap::map(*p)]) {
      ptr = ptr->ch[AlphabetMap::map(*p)];
      p++;
    }
    if(*p != '\0') return false;
    else {
      *res = ptr->val;
      return true;
    }
  }
  uint StrCnt() const { return cnt_; }
 private:
  using AlphabetMap = Alphabet<charSet...>;
  static constexpr uint kAlphabetSize = AlphabetMap::size;
  struct Node {
    Valtype val;
    Node* ch[kAlphabetSize];
    Node() {
      memset(ch, 0, sizeof(ch));
    }
  };
  uint cnt_ = 0;
  Node* rt_ = nullptr;
};

#endif  // LSPI_HASH_H
