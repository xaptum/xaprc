#ifndef REST_RESOURCE_ROOT_H
#define REST_RESOURCE_ROOT_H

#include <string>
#include <event2/buffer.h>
#include <event2/http.h>

#include "resource.h"

namespace captiverc {

class rest_resource_root : public resource {
  public:
    rest_resource_root();

    resource::resp_type get(resource::req_type body) override;

  private:
    std::string get_file_contents(std::string filename);
    std::string get_status();


};

}   // namespace captiverc


#endif
