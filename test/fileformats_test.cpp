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

TEST(FileExtTest, ExtGetsKnownExtension) {
  std::filesystem::path archive_path{"/some/path/to/file.rar"};

  ASSERT_EQ(xwim::ext(archive_path), std::filesystem::path{".rar"});
}

TEST(FileExtTest, CombinedExtensionGetsAll) {
  std::filesystem::path archive_path{"/some/path/to/file.tar.gz"};

  ASSERT_EQ(xwim::ext(archive_path), std::filesystem::path{".tar.gz"});
}

TEST(FileExtTest, ExtEmptyForUnknownExtension) {
  std::filesystem::path archive_path{"/some/path/to/file.ukn"};

  ASSERT_TRUE(xwim::ext(archive_path).empty());
}

TEST(FileExtTest, CombinedExtensionGetsKnown) {
  std::filesystem::path archive_path{"/some/path/to/file.ukn.tar.gz"};

  ASSERT_EQ(xwim::ext(archive_path), std::filesystem::path{".tar.gz"});
}

TEST(FileExtTest, CombinedExtensionLastUnknownEmpty) {
  std::filesystem::path archive_path{"/some/path/to/file.tar.gz.ukn"};

  ASSERT_TRUE(xwim::ext(archive_path).empty());
}
