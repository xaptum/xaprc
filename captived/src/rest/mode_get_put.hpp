#ifndef MODE_GET_PUT_H
#define MODE_GET_PUT_H

#include <string>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"

namespace captiverc {
namespace rest {

////////////////////////////////////////////////////////////////////////////////
/// mode_get_put
/// This inherits from get_file.
/// It is almost identical to the put_file, except that it validates the 
/// mode values being passed in.
////////////////////////////////////////////////////////////////////////////////
class mode_get_put : public get_file {
  public:
    mode_get_put(std::string path, 
                 std::string filename);

    resource::resp_type put(resource::req_type body) override;

};

} // namespace rest
} // namespace captiverc


#endif
