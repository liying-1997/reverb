
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

#include "reverb/cc/selectors/customize.h"

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "reverb/cc/schema.pb.h"
#include "reverb/cc/platform/status_matchers.h"
#include "reverb/cc/testing/proto_test_util.h"

namespace deepmind {
namespace reverb {
namespace {

TEST(CustomizeSelectorTest, ReturnValueSantiyChecks) {
  CustomizeSelector customize;

  // Non existent keys cannot be deleted or updated.
  EXPECT_EQ(customize.Delete(123).code(), absl::StatusCode::kInvalidArgument);
  EXPECT_EQ(customize.Update(123, 4).code(), absl::StatusCode::kInvalidArgument);

  // Keys cannot be inserted twice.
  REVERB_EXPECT_OK(customize.Insert(123, 4));
  EXPECT_EQ(customize.Insert(123, 4).code(), absl::StatusCode::kInvalidArgument);

  // Keys with different priorities can be inserted correctly.
  REVERB_EXPECT_OK(customize.Insert(1234, 4));
  EXPECT_EQ(customize.Insert(123, 4).code(), absl::StatusCode::kInvalidArgument);

  REVERB_EXPECT_OK(customize.Insert(12, 4));
  EXPECT_EQ(customize.Insert(12, 4).code(), absl::StatusCode::kInvalidArgument);

  REVERB_EXPECT_OK(customize.Insert(15, 4));
  EXPECT_EQ(customize.Insert(15, 4).code(), absl::StatusCode::kInvalidArgument);

  REVERB_EXPECT_OK(customize.Insert(23, 2));
  EXPECT_EQ(customize.Insert(23, 2).code(), absl::StatusCode::kInvalidArgument);

  REVERB_EXPECT_OK(customize.Insert(122, 3));
  EXPECT_EQ(customize.Insert(122, 3).code(), absl::StatusCode::kInvalidArgument);

  // Existing keys can be updated and sampled.
  REVERB_EXPECT_OK(customize.Update(123, 5));
  EXPECT_EQ(customize.Sample(3).key, 122);

  // Existing keys cannot be deleted twice.
  REVERB_EXPECT_OK(customize.Delete(123));
  EXPECT_EQ(customize.Delete(123).code(), absl::StatusCode::kInvalidArgument);
}

TEST(CustomizeSelectorTest, MatchesCustomizeSelector) {
  const int64_t kItems = 100;
  const int64_t kSamples = 1000000;
  // double expected_probability = 1. / static_cast<double>(kItems);

  CustomizeSelector customize;
  for (int i = 0; i < kItems; i++) {
    REVERB_EXPECT_OK(customize.Insert(i, i%10));
  }
  std::vector<int64_t> counts(kItems);
  for (int i = 0; i < kSamples; i++) {
    int64_t priority = i % 10;
    ItemSelector::KeyWithProbability sample = customize.Sample(priority);
    EXPECT_EQ(sample.probability, priority);
  }
}

// TEST(CustomizeSelectorTest, Options) {
//   CustomizeSelector customize;
//   EXPECT_THAT(customize.options(),
//               testing::EqualsProto("customize: true is_deterministic: false"));
// }

TEST(CustomizeDeathTest, ClearThenSample) {
  CustomizeSelector customize;
  for (int i = 0; i < 100; i++) {
    REVERB_EXPECT_OK(customize.Insert(i, i));
  }
  customize.Sample(4);
  customize.Clear();
  EXPECT_DEATH(customize.Sample(4), "");
}

}  // namespace
}  // namespace reverb
}  // namespace deepmind
