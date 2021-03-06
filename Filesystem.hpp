#ifndef UTILITIES_CORE_FILESYSTEM_HPP
#define UTILITIES_CORE_FILESYSTEM_HPP

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>

// For fmt support
#include <fmt/format.h>
#include <sstream>
#include <iomanip>

namespace openstudio {
typedef boost::filesystem::path path;

namespace filesystem {

  // types
  using boost::filesystem::path;
  using boost::filesystem::wpath;

  using boost::filesystem::basic_ofstream;

  using boost::filesystem::ofstream;
  using boost::filesystem::ifstream;

  using boost::filesystem::recursive_directory_iterator;
  using boost::filesystem::filesystem_error;
  using boost::filesystem::copy_option;
  using boost::filesystem::directory_entry;

  // functions
  using boost::filesystem::basename;
  using boost::filesystem::canonical;
  using boost::filesystem::complete;
  using boost::filesystem::copy;
  using boost::filesystem::copy_file;
  using boost::filesystem::create_directories;
  using boost::filesystem::create_directory;
  using boost::filesystem::directory_iterator;
  using boost::filesystem::equivalent;
  using boost::filesystem::exists;
  using boost::filesystem::extension;
  using boost::filesystem::file_size;
  using boost::filesystem::initial_path;
  using boost::filesystem::is_directory;
  using boost::filesystem::is_empty;
  using boost::filesystem::is_regular_file;
  using boost::filesystem::is_symlink;
  using boost::filesystem::last_write_time;
  using boost::filesystem::remove;
  using boost::filesystem::remove_all;
  using boost::filesystem::file_size;
  using boost::filesystem::system_complete;
  using boost::filesystem::temp_directory_path;
  using boost::filesystem::read_symlink;
  using boost::filesystem::weakly_canonical;

}  // namespace filesystem
}  // namespace openstudio

namespace fs = openstudio::filesystem;

// Add a custom formatter for fmt
template <> struct fmt::formatter<fs::path>: fmt::formatter<string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(const fs::path& p, FormatContext& ctx) {
    std::stringstream ss;
    ss << std::quoted(p.string());

    // Note: std::quoted does this

    /*
    constexpr char delim = '"';
    constexpr char escape = '\\';
    ss << delim;
    for (auto c: p.string()) {
        if (c == delim || c == escape) {
            ss << escape;
        }
        ss << c;
    }
    ss << delim;
    */

    return fmt::formatter<string_view>::format(ss.str(), ctx);
  }
};


#endif
