#ifndef REST_PUT_FILE_H
#define REST_PUT_FILE_H

#include <string>

#include "http_server.h"
#include "resource.h"
#include "rest_get_file.h"

namespace captiverc {

class rest_put_file : public rest_get_file {
  public:
    rest_put_file(std::string path, 
                  std::string filename);

    resource::resp_type put(resource::req_type body) override;

};

}   // namespace captiverc


#endif
