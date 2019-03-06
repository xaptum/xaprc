#ifndef rest_mode_get_put_H
#define rest_mode_get_put_H

#include <string>

#include "http_server.h"
#include "resource.h"
#include "rest_get_file.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// rest_mode_get_put
/// This inherits from rest_get_file.
/// It is almost identical to the rest_put_file, except that it validates the 
/// mode values being passed in.
////////////////////////////////////////////////////////////////////////////////
class rest_mode_get_put : public rest_get_file {
  public:
    rest_mode_get_put(std::string path, 
                  std::string filename);

    resource::resp_type put(resource::req_type body) override;

};

}   // namespace captiverc


#endif
