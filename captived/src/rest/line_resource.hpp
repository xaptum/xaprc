#ifndef LINE_RESOURCE_H
#define LINE_RESOURCE_H

#include <string>

#include "system.hpp"
#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

class line_resource : public resource {
public:
  line_resource(std::string path, system system,
                std::string filename, bool writable);
  ~line_resource() override = default;

  resp_type get(req_type) override;
  resp_type put(req_type body) override;

  /**
   * Fetch the line.
   *
   * @returns The line or None on an error.
   */
  virtual
  std::experimental::optional<std::string>
  line();

  /**
   * Updates the line.
   *
   * @returns true on success and false on an error
   */
  virtual
  bool
  line(std::string new_line);

protected:
  system system_;
  std::string filename_;
  bool writable_;
};

} // namespace rest
} // namespace captiverc


#endif
