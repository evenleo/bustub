//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) : num_pages_(num_pages) {}

LRUReplacer::~LRUReplacer() = default;

// 删除与Replacer追踪的所有其他元素相比最近被访问的对象，将其内容存储在输出参数中
// 并返回True。如果Replacer是空的，则返回False。
bool LRUReplacer::Victim(frame_id_t *frame_id) { 
  std::lock_guard<std::mutex> guard(latch_);
  if (lru_.empty()) {
    return false;
  } 
  *frame_id = lru_.back();
  table_.erase(lru_.back());
  lru_.pop_back();
  return true; 
}

// 这个方法应该在一个页面被钉在BufferPoolManager的一个框架上之后被调用。它应该从
// LRUReplacer中删除包含钉住的页面的框架。
void LRUReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> guard(latch_);
  auto it = table_.find(frame_id);
  if (it != table_.end()) {
    lru_.erase(it->second);
    table_.erase(frame_id);
  } 
}

// 当一个页面的pin_count变成0的时候，这个方法应该被调用，这个方法应该把包含取消
// pin的页面的框架添加到LRUReplacer中。
void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> guard(latch_);
  auto it = table_.find(frame_id);
  if (it == table_.end()) {
    if (lru_.size() == num_pages_) {
      table_.erase(lru_.back());
      lru_.pop_back();
    }
    lru_.push_front(frame_id);
    table_[frame_id] = lru_.begin();
  }
}

// 该方法返回当前在LRUReplacer中的帧数。
size_t LRUReplacer::Size() { 
  std::lock_guard<std::mutex> guard(latch_);
  return lru_.size(); 
}

}  // namespace bustub
