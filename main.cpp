#include <gtest/gtest.h>

#include "Filesystem.hpp"
#include "FilesystemHelpers.hpp"
#include "TestFixture.hpp"

#include <fmt/format.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

namespace fs = openstudio::filesystem;

// @param ignoredPaths: a list of subfolders to ignore
bool copyDir(fs::path const& source, fs::path const& destination, [[maybe_unused]] const std::vector<fs::path>& ignoredPaths = std::vector<fs::path>()) {
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



struct TestPath {
  fs::path path;
  bool allowed;
};

std::vector<TestPath> generateTestMeasurePaths() {

  std::vector<TestPath> testPaths;

  std::vector<std::string> approvedRootFiles {
      "measure.rb", "README.md","README.md.erb",
      "LICENSE.md", "measure.xml", ".gitkeep",
  };

  std::vector<std::string> approved_folders {
    "docs", "resources", "tests"
  };

  std::vector<std::string> ignoredPaths {
    "root_file.txt", "tests/output/output.txt", "subfolder/subfolder.txt",
    ".git/index", ".hidden_folder/file.txt"
  };

  testPaths.reserve(approvedRootFiles.size() + 2*approved_folders.size() + ignoredPaths.size());
  for (const auto& s: approvedRootFiles) {
    testPaths.emplace_back(fs::path{s}, true);
  }

  for (const auto& s: approved_folders) {
    fs::path file(s);
    file.replace_extension(".rb");
    testPaths.emplace_back(fs::path{s} / file, true);
    testPaths.emplace_back(fs::path(s) / "subfolder" / "subfolder_file.txt", true);
  }

  for (const auto& s: ignoredPaths) {
    testPaths.emplace_back(fs::path{s}, false);
  }

  return testPaths;
}

void createTestMeasureDirectory(const std::vector<TestPath>& testPaths) {

  openstudio::path srcDir("../test/my_measure/");
  auto absolutePath = fs::weakly_canonical(srcDir);
  fmt::print("srcDir={}\n", srcDir);
  fmt::print("absolutePath={}\n", absolutePath);
  fs::remove_all(srcDir);
  fs::create_directories(absolutePath);

  for (const auto& testPath: testPaths) {
    const fs::path& p = fs::weakly_canonical(absolutePath / testPath.path);
    fmt::print("{}\n", p);
    fs::create_directories(p.parent_path());
    std::ofstream(p.string()) << p.string();
  }
}


TEST_F(TestFixture, TestFull) {

  std::vector<TestPath> testPaths = generateTestMeasurePaths();
  EXPECT_GT(testPaths.size(), 5);
  createTestMeasureDirectory(testPaths);

}

TEST_F(TestFixture, TestExclusion) {

  auto abspath = fs::canonical(".");
  openstudio::path parentPath = abspath;

  while (!parentPath.empty()) {
    fmt::print("{}\n", parentPath);
    parentPath = parentPath.parent_path();
  }
}
