#pragma once

#include <memory>
#include <exception>
#include <folly/ConcurrentSkipList.h>
#include <folly/Optional.h>
#include "util/MultiVersionPtr.h"
namespace relevanced {
namespace util {

template<typename TKey, typename TVal>
class ConcurrentMapItem {
  typedef ConcurrentMapItem<TKey, TVal> TItem;
  typedef typename MultiVersionPtr<TVal>::Proxy TReadPtr;
  typedef TKey key_type;
  typedef TVal value_type;
protected:
  TKey key_;
  TReadPtr val_;
  bool hasValue_ {false};
public:
  ConcurrentMapItem() {
    TKey defaultKey;
    key_ = defaultKey;
  }
  ConcurrentMapItem(const TKey &key): key_(key){}
  ConcurrentMapItem(const TKey &key, std::unique_ptr<TVal>&& val): key_(key) {
    hasValue_ = true;
    std::unique_ptr<TVal> tempV = std::move(val);
    val_.reset(tempV.release());
  }
  TReadPtr getValuePtr() {
    DCHECK(hasValue_);
    return val_;
  }
  const TKey& getKey() {
    return key_;
  }
  void setValue(std::unique_ptr<TVal> &&val) {
    std::unique_ptr<TVal> tempV = std::move(val);
    val_.reset(tempV.release());
  }
  friend struct std::less<TItem>;
};

} // util
} // relevanced

namespace std {
  template<typename TKey, typename TVal>
  struct less<relevanced::util::ConcurrentMapItem<TKey, TVal>> {
    typedef relevanced::util::ConcurrentMapItem<TKey, TVal> TItem;
    bool operator()(const TItem &item1, const TItem &item2) {
      return item1.key_ < item2.key_;
    }
  };
} // std

namespace relevanced {
namespace util {

template<typename TKey, typename TVal>
class ConcurrentMap {
  typedef ConcurrentMapItem<TKey, TVal> TItem;
  typedef folly::ConcurrentSkipList<TItem> TSkipList;
  typedef typename MultiVersionPtr<TVal>::Proxy TValReadPtr;
  typedef TKey key_type;
  typedef TVal value_type;
protected:
  std::shared_ptr<TSkipList> skipList_;
  size_t headSize_;
public:
  ConcurrentMap(size_t headSize): headSize_(headSize) {
    skipList_ = TSkipList::createInstance(headSize_);
  }
  void insertOrUpdate(const TKey &key, std::unique_ptr<TVal> &&val) {
    TItem toFind(key);
    typename TSkipList::Accessor accessor(skipList_);
    auto it = accessor.find(toFind);
    if (it.good()) {
      it->setValue(std::move(val));
    } else {
      TItem item(key, std::move(val));
      accessor.addOrGetData(item);
    }
  }
  bool erase(const TKey &key) {
    TItem toErase(key);
    typename TSkipList::Accessor accessor(skipList_);
    auto nErased = accessor.erase(toErase);
    return nErased > 0;
  }
  folly::Optional<TValReadPtr> getOption(const TKey &key) {
    TItem toFind(key);
    typename TSkipList::Accessor accessor(skipList_);
    auto it = accessor.find(key);
    folly::Optional<TValReadPtr> result;
    if (it.good()) {
      result.assign(it->getValuePtr());
    }
    return result;
  }
};


} // util
} // relevanced
