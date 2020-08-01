#include "argparse.hpp"

namespace xwim {
namespace argparse {
  XwimPath parse(int argc, char** argv) {
    return XwimPath{argc, argv};
  }

  // contructs XwimPath{} first so that destructurs may running
  // http://www.vishalchovatiya.com/7-best-practices-for-exception-handling-in-cpp-with-example/
  XwimPath::XwimPath(int argc, char** argv) : XwimPath{} {
    if (argc < 2) throw ArgParseException{"No argument provided"};
    if (argc > 2) throw ArgParseException{"Too many arguments provided"};

    this->_path = std::filesystem::path{argv[1]};
  }

  bool XwimPath::is_archive() {
    return !xwim::ext(_path).empty();
  }

  std::filesystem::path XwimPath::path() const {
    return std::filesystem::path{_path};
  }
}
}
