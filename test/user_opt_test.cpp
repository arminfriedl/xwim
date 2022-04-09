#include <gtest/gtest-death-test.h>
#include "gtest/gtest.h"
#include <filesystem>
#include <string>

#include "UserOpt.hpp"

TEST(UserOpt, compress) {
  using namespace xwim;

  // clang-format off
  char* args[] = {
    const_cast<char*>("xwim"),
    const_cast<char*>("-c"),
    const_cast<char*>("mandator_paths"),
    nullptr};
  // clang-format on

  UserOpt uo = UserOpt{3, args};
  ASSERT_TRUE(uo.compress);
  ASSERT_FALSE(uo.extract);
}

TEST(UserOpt, exclusive_actions) {
  using namespace xwim;

  // clang-format off
  char* args[] = {
    const_cast<char*>("xwim"),
    const_cast<char*>("-c"),
    const_cast<char*>("-x"),
    const_cast<char*>("mandatory_paths"),
    nullptr};
  // clang-format on

  UserOpt uo = UserOpt{4, args};
  ASSERT_TRUE(uo.compress);
  ASSERT_TRUE(uo.extract);
}

TEST(UserOpt, whitespace_in_path) {
  using namespace xwim;

  // clang-format off
  char* args[] = {
    const_cast<char*>("xwim"),
    const_cast<char*>("-c"),
    const_cast<char*>("/foo/bar baz/a file"),
    nullptr};
  // clang-format on

  UserOpt uo = UserOpt{3, args};
  ASSERT_TRUE(uo.paths.find(std::filesystem::path("/foo/bar baz/a file")) !=
              uo.paths.end());
}

TEST(UserOpt, mixed_output_and_paths) {
  using namespace xwim;

  // clang-format off
  char* args[] = {
    const_cast<char*>("xwim"),
    const_cast<char*>("-o"),
    const_cast<char*>("/foo/bar baz/output"),
    const_cast<char*>("/foo/bar baz/a path"),
    const_cast<char*>("/foo/bar baz/another path"),
    nullptr};
  // clang-format on

  UserOpt uo = UserOpt{5, args};
  ASSERT_TRUE(uo.paths.find(std::filesystem::path("/foo/bar baz/a path")) !=
              uo.paths.end());
  ASSERT_TRUE(uo.paths.find(std::filesystem::path("/foo/bar baz/another path")) !=
              uo.paths.end());
  ASSERT_TRUE(uo.out == std::filesystem::path("/foo/bar baz/output"));
}

TEST(UserOpt, output_defaults_to_nullopt) {
  using namespace xwim;

  // clang-format off
  char* args[] = {
    const_cast<char*>("xwim"),
    const_cast<char*>("/foo/bar"),
    nullptr};
  // clang-format on

  UserOpt uo = UserOpt{2, args};
  ASSERT_FALSE(uo.out);
}
