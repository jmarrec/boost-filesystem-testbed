#include <gtest/gtest.h>
#include "FilesystemHelpers.hpp"
#include "TestFixture.hpp"

#include "Filesystem.hpp"
#include <fmt/format.h>

#include <vector>
#include <iostream>

TEST_F(TestFixture, BoostCopy) {

  boost::filesystem::path full_path(boost::filesystem::current_path());
  std::cout << "Current path is : " << full_path << std::endl;

  openstudio::path srcDir("../test/ori_measure_dir/");
  fmt::print("srcDir={}", srcDir.string());
  for (const auto& file : openstudio::filesystem::directory_files(srcDir)) {
    std::cout << file.string() << '\n';
  }
}
