#include <gtest/gtest.h>

#include <fileformats.hpp>
#include <string>

TEST(FileformatsTest, StemStripsSingleKnownExtension) {
  std::filesystem::path archive_path {"/some/path/to/file.rar"};

  ASSERT_EQ(xwim::stem(archive_path), std::filesystem::path{"file"});
}

TEST(FileformatsTest, StemStripsMultipleKnownExtensions) {
  std::filesystem::path archive_path{"/some/path/to/file.tar.rar.gz.7z.rar"};

  ASSERT_EQ(xwim::stem(archive_path), std::filesystem::path{"file"});
}

TEST(FileformatsTest, StemStripsOnlyKnownExtension) {
  std::filesystem::path archive_path{"/some/path/to/file.ukn.rar"};

  ASSERT_EQ(xwim::stem(archive_path), std::filesystem::path{"file.ukn"});
}

TEST(FileformatsTest, StemStripsNothingWithoutKnownExtension) {
  std::filesystem::path archive_path{"/some/path/to/file.ukn"};

  ASSERT_EQ(xwim::stem(archive_path), std::filesystem::path{"file.ukn"});
}

TEST(FileformatsTest, StemStripsNothingWithoutExtension) {
  std::filesystem::path archive_path{"/some/path/to/filerar"};

  ASSERT_EQ(xwim::stem(archive_path), std::filesystem::path{"filerar"});
}
