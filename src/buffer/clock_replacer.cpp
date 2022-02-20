//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) : num_pages_(num_pages) {}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) { 
  std::lock_guard<std::mutex> guard(latch_);
  if (lru_.empty()) {
    return false;
  } 
  *frame_id = lru_.back();
  table_.erase(lru_.back());
  lru_.pop_back();
  return true; 
}

void ClockReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> guard(latch_);
  auto it = table_.find(frame_id);
  if (it != table_.end()) {
    lru_.erase(it->second);
    table_.erase(frame_id);
  } 
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
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

size_t ClockReplacer::Size()  { 
  std::lock_guard<std::mutex> guard(latch_);
  return lru_.size(); 
}

}  // namespace bustub
