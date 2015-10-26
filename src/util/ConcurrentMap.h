#pragma once

#include <memory>
#include <exception>
#include <folly/ConcurrentSkipList.h>
#include <folly/Optional.h>
#include "util/MultiVersionPtr.h"
#include "util/util.h"

/*
  This ConcurrentMap is an associative layer on top
  of folly's ConcurrentSkipList.

  ConcurrentSkipList already allows multiple readers
  and multiple writers without a structure-wide
  lock (i.e. nodes can be inserted, moved, deleted
  without making another reader segfault)

  On top of that, ConcurrentMap provides 2 things:

  1) key-based access to the underlying `value_type`
     (note that this is O(log(n)), not O(1)).

  2) A pointer value type over the templated `TVal`
     which allows the ConcurrentMap node's value
     to be modified while leaving the old value
     accessible to readers that have already obtained
     it.

  In benchmarks, using this structure instead of an
  std::map or std::unordered_map with a structure-wide
  lock has improved the SimilarityScoreWorker's throughput
  by about 4x (when run with multiple threads).

  This structure probably isn't ideal for a huge map due
  to the O(log(n)) lookup time.  The SimilarityScoreWorker's
  centroid map generally contains less than 100 items,
  and almost certainly less than 1000, so that isn't
  a big concern there.

*/
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
  ConcurrentMapItem(const TKey &key, util::UniquePointer<TVal>&& val): key_(key) {
    hasValue_ = true;
    util::UniquePointer<TVal> tempV = std::move(val);
    val_.reset(tempV.release());
  }
  TReadPtr getValuePtr() {
    DCHECK(hasValue_);
    return val_;
  }
  const TKey& getKey() {
    return key_;
  }
  void setValue(util::UniquePointer<TVal> &&val) {
    util::UniquePointer<TVal> tempV = std::move(val);
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
  typedef typename MultiVersionPtr<TVal>::Proxy ReadPtr;
  ConcurrentMap(size_t headSize): headSize_(headSize) {
    skipList_ = TSkipList::createInstance(headSize_);
  }
  void insertOrUpdate(const TKey &key, util::UniquePointer<TVal> &&val) {
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
