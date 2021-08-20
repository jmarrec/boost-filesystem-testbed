#include <gtest/gtest.h>

#include "FilesystemHelpers.hpp"
#include "TestFixture.hpp"

#include "Filesystem.hpp"
#include <fmt/format.h>

#include <vector>
#include <iostream>

#include <boost/filesystem.hpp>

namespace fs = openstudio::filesystem;

bool copyDir(fs::path const& source, fs::path const& destination) {
  try {
    // Check whether the function call is valid
    if (!fs::exists(source) || !fs::is_directory(source)) {
      std::cerr << "Source directory " << source.string() << " does not exist or is not a directory." << '\n';
      return false;
    }
    if (fs::exists(destination)) {
      std::cerr << "Destination directory " << destination.string() << " already exists." << '\n';
      return false;
    }
    // Create the destination directory
    if (!fs::create_directory(destination)) {
      std::cerr << "Unable to create destination directory" << destination.string() << '\n';
      return false;
    }
  } catch (fs::filesystem_error const& e) {
    std::cerr << e.what() << '\n';
    return false;
  }
  // Iterate through the source directory
  for (auto dir_itr = openstudio::filesystem::directory_iterator(source); dir_itr != openstudio::filesystem::directory_iterator(); ++dir_itr) {
    try {
      fs::path current(dir_itr->path());
      if (fs::is_directory(current)) {
        // Found directory: Recursion
        if (!copyDir(current, destination / current.filename())) {
          return false;
        }
      } else {
        // Found file: Copy
        fs::copy_file(current, destination / current.filename());
      }
    } catch (fs::filesystem_error const& e) {
      std::cerr << e.what() << '\n';
    }
  }
  return true;
}

TEST_F(TestFixture, BoostCopy) {

  boost::filesystem::path full_path(boost::filesystem::current_path());
  fmt::print("Current path is : {}\n", full_path);

  std::vector<fs::path> oriEntries;

  openstudio::path srcDir("../test/ori_measure_dir/");
  openstudio::path destDir("../test/output/");
  fmt::print("srcDir={}\n", srcDir);

  // recursive_directory_files returns path relative to t_path passed as argument
  for (const auto& file : openstudio::filesystem::recursive_directory_files(srcDir)) {
    fmt::print("* {}\n", file);
    oriEntries.push_back(file);
  }

  EXPECT_EQ(14, oriEntries.size());

  // Cleanup the output dir if it exists
  fs::remove_all("../test/output/");
  ASSERT_FALSE(fs::exists(destDir));

  // recursively copy a directory
  EXPECT_TRUE(copyDir(srcDir, destDir));

  // Now check destination
  std::vector<fs::path> copiedEntries;
  fmt::print("\n\ndestDir={}\n", destDir);
  for (const auto& file : openstudio::filesystem::recursive_directory_files(destDir)) {
    fmt::print("* {}\n", file);
    copiedEntries.push_back(file);
  }

  EXPECT_EQ(copiedEntries.size(), oriEntries.size());
  for (const auto& p: oriEntries) {
    auto s = srcDir / p;
    auto d = destDir / p;
    ASSERT_TRUE(fs::exists(s) && fs::is_regular_file(s));
    ASSERT_TRUE(fs::exists(d) && fs::is_regular_file(d));
    // Check that size is OK (will throw if either doesn't exist or regular file, hence the ASSERT above)
    EXPECT_EQ(fs::file_size(s), fs::file_size(d));
  }
}
