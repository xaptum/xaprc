#pragma once

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <experimental/optional>
#include <sys/stat.h>

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
    system(std::string chroot) : chroot_(chroot){};

    system(const system&) = delete;
    system(system&&) = default;

    /**
     * Executes the provided command using the "system" call.
     */
    int execute(std::string command) {
        auto full_command = chroot_ + command;
        return ::system(full_command.c_str());
    }

    /**
     * Runs a command and puts the output of that command in result.
     * result contains only the stdout, not the stderr.
     *
     * @returns 0 for success, errno for an error.
     */
    int execute(std::string command, std::string& result) {
        std::string fq_command = chroot_ + command;
        const int buffer_size = 128;
        std::array<char, buffer_size> buffer;
        result.clear();

        FILE* pipe = popen(fq_command.c_str(), "r");
        if (NULL == pipe) {
            return errno;
        }

        while (fgets(buffer.data(), buffer_size, pipe) != nullptr) {
            result += buffer.data();
        }

        int returnCode = pclose(pipe);
        if (-1 == returnCode) {
            return errno;
        }

        return returnCode;
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
     * Use a temporary file & rename to create an atomic write.
     */
    bool write(std::string filename, std::string contents) {
        std::string temp_file = chroot_ + filename + "_XXXXXX";
        std::ofstream out(temp_file, std::ofstream::out | std::ofstream::trunc);

        out << contents;
        out.close();

        if (out.fail())
            return false;

        std::string new_file = chroot_ + filename;
        int res = rename(temp_file.c_str(), new_file.c_str());

        return (res == 0);
    }

    /**
     * Replaces the contents of the specified file with the specified line.
     *
     * Note that a newline is automatically appended to line contents.
     */
    bool writeline(std::string filename, std::string line) {
        return write(filename, line + '\n');
    }

    /**
     * Return the target of a symbolic link.
     *
     * Note: Will only follow a single link and assumes argument IS a link
     */
    std::experimental::optional<std::string>
    symlink_target(std::string link_path) {
        char buf[ROUTER_CARD_PATH_MAX];
        std::string full_link_path = chroot_ + link_path;

        ssize_t nbytes =
            readlink(full_link_path.c_str(), buf, ROUTER_CARD_PATH_MAX);
        if ((nbytes < 0) || (nbytes >= ROUTER_CARD_PATH_MAX)) {    // error case
            return {};
        }

        std::string target{buf, static_cast<size_t>(nbytes)};
        return {target};
    }

    /**
     * Add or change the symlink to a target.
     *
     * If a file already exists and is a symlink, it is deleted before a
     * new link is created. If the file is not a link, error out.
     */
    bool symlink_target(std::string target, std::string link) {
        std::string fq_target = chroot_ + target;
        std::string fq_link = chroot_ + link;

        // check if file exists
        struct stat buffer;
        int stat_result = lstat(fq_link.c_str(), &buffer);
        if (stat_result == 0) {
            if (S_ISLNK(buffer.st_mode)) {    // delete if a link
                remove(fq_link.c_str());
            } else {    // error if other type of file
                return false;
            }
        }

        // create new symbolic link
        int result = symlink(fq_target.c_str(), fq_link.c_str());
        return (result == 0);
    }

  private:
    std::string chroot_;
};

}    // namespace captived
