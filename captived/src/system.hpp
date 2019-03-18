#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <experimental/optional>

namespace captived {

/**
 * An abstraction of the system for common operations like accessing
 * files or running commands.
 */
class system {

  public:
    /**
     * Create a new system instance.
     *
     * @param chroot A path to prepend to all file names. Used to
     * implement a poor man's chroot for integration testing.
     */
    system(std::string chroot) :
      chroot_(chroot)
    {};

    /**
     * Executes the provided command using the "system" call.
     */
    int execute(std::string command) {
      auto full_command = chroot_ + command;
      return ::system(full_command.c_str());
    }

    /**
     * Returns the entire contents of the specified file or None if an
     * error occurs.
     */
    std::experimental::optional<std::string> read(std::string filename) {
      std::ifstream in(chroot_ + filename);
      std::stringstream buf;

      buf << in.rdbuf();
      if (buf.fail())
        return {};

      return {buf.str()};
    }

    /**
     * Returns the first line of the specified file or None if an error occurs.
     *
     * Note that the newline is stripped from the returned result.
     */
    std::experimental::optional<std::string> readline(std::string filename) {
      std::ifstream in(chroot_ + filename);

      std::string line;
      std::getline(in, line);
      if (in.fail())
        return {};

      return {line};
    }

    /**
     * Replaces the contents of the specified file with the specified contents.
     */
    bool write(std::string filename, std::string contents) {
      std::ofstream out(chroot_ + filename,
                        std::ofstream::out | std::ofstream::trunc);

      out << contents;
      out.close();

      if (out.fail())
        return false;

      return true;
    }

    /**
     * Replaces the contents of the specified file with the specified line.
     *
     * Note that a newline is automatically appended to line contents.
     */
    bool writeline(std::string filename, std::string line) {
      return write(filename, line + '\n');
    }

  private:
    std::string chroot_;
  };

} // namespace captived
