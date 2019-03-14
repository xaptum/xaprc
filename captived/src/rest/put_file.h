#ifndef PUT_FILE_H
#define PUT_FILE_H

#include <string>

#include "rest/resource.h"
#include "rest/get_file.h"

namespace captiverc {

class rest_put_file : public rest_get_file {
  public:
    rest_put_file(std::string path, 
                  std::string filename);

    resource::resp_type put(resource::req_type body) override;

};

}   // namespace captiverc


#endif
