#include <gtest/gtest.h>

#include <archive.hpp>
#include <filesystem>
#include <spec.hpp>

TEST(ArchiveTest, ArchiveSpecDetectsSingleRoot) {
  xwim::Archive archive("test/archives/root.tar.gz");

  xwim::ArchiveSpec spec = archive.check();
  ASSERT_TRUE(spec.has_single_root);
}
