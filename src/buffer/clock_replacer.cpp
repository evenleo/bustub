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

// Victim(受害者)，可理解为牺牲
bool ClockReplacer::Victim(frame_id_t *frame_id) { 
  std::lock_guard<std::mutex> guard(latch_);
  if (lru_.empty()) {  // 没有可以牺牲的页直接返回false
    return false;
  } 
  *frame_id = lru_.back();
  table_.erase(lru_.back());
  lru_.pop_back();
  return true; 
}

// pin函数表示这个frame被某个进程引用了
// 被引用的frame不能成为LRU算法的牺牲目标，所以这里把它从我们的数据结构中删除
void ClockReplacer::Pin(frame_id_t frame_id) {
  std::lock_guard<std::mutex> guard(latch_);
  auto it = table_.find(frame_id);
  if (it != table_.end()) {
    lru_.erase(it->second);
    table_.erase(frame_id);
  } 
}

// 先看一下这个页是否在可替换链表中
// 如果它不存在的话。则需要看一下当前链表是否还有空闲位置。如果有的话则直接加入
// 如果没有则需要移除链表尾部的节点知道有空余位置
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
