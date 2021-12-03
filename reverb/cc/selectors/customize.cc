// Copyright 2019 DeepMind Technologies Limited.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <list>
#include "reverb/cc/selectors/customize.h"

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "reverb/cc/checkpointing/checkpoint.pb.h"
#include "reverb/cc/platform/logging.h"
#include "reverb/cc/schema.pb.h"

namespace deepmind {
namespace reverb {

absl::Status CustomizeSelector::Delete(Key key) {
  const auto it = key_to_index_.find(key);
  if (it == key_to_index_.end())
    return absl::InvalidArgumentError(absl::StrCat("Key ", key, " not found."));
  const size_t index = it->second;
  key_to_index_.erase(it);

  const size_t last_index = keys_.size() - 1;
  const Key last_key = keys_.back();
  if (index != last_index) {
    keys_[index] = last_key;
    key_to_index_[last_key] = index;
  }

  keys_.pop_back();
  return absl::OkStatus();
}

absl::Status CustomizeSelector::Insert(Key key, double priority) {
  const size_t index = keys_.size();
  if (!key_to_index_.emplace(key, index).second)
    return absl::InvalidArgumentError(
        absl::StrCat("Key ", key, " already inserted."));
  keys_.push_back(key);

  if (priority_to_indexs_.find(priority) !=priority_to_indexs_.end()){
    // std::list<size_t> t = priority_to_indexs_.find(priority)
    // auto t = priority_to_indexs_.find(priority);
    auto t =priority_to_indexs_[priority];
    // std::cout<<"type t:"<<typeid(t).name()<<std::endl;

    // priority_to_indexs_.emplace(priority,t.emplace(t.end(),index));}  
    t.push_back(index);
    priority_to_indexs_.emplace(priority,t);
    } 
  else{ 
    std::vector<size_t> l;
    l.push_back(index);
    priority_to_indexs_.emplace(priority, l);
    }
  return absl::OkStatus();
}

absl::Status CustomizeSelector::Update(Key key, double priority) {
  if (key_to_index_.find(key) == key_to_index_.end())
    return absl::InvalidArgumentError(absl::StrCat("Key ", key, " not found."));
  return absl::OkStatus();
}

ItemSelector::KeyWithProbability CustomizeSelector::Sample(double priority) {
  REVERB_CHECK(!keys_.empty());
  const size_t index = absl::Uniform<size_t>(bit_gen_, 0, priority_to_indexs_[priority].size());
  auto index_ = priority_to_indexs_[priority][index];
  return {keys_[index_], priority};

  // if (priority_to_indexs_.find(priority))
  //   const size_t index = absl::Uniform<size_t>(bit_gen_, 0, priority_to_indexs_[priority].size())
  //   return {priority_to_indexs_[priority][index], priority}

//   const size_t index = absl::Uniform<size_t>(bit_gen_, 0, keys_.size());
//   return {keys_[index], 1.0 / static_cast<double>(keys_.size())};
}




void CustomizeSelector::Clear() {
  keys_.clear();
  key_to_index_.clear();
}

// KeyDistributionOptions CustomizeSelector::options() const {
//   KeyDistributionOptions options;
//   options.set_customize(true);
//   options.set_is_deterministic(false);
//   return options;
// }

std::string CustomizeSelector::DebugString() const { return "CustomizeSelector"; }

}  // namespace reverb
}  // namespace deepmind
