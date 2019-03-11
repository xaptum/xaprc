#ifndef REST_GET_FILE_H
#define REST_GET_FILE_H

#include <string>

#include "http_server.h"
#include "resource.h"

namespace captiverc {

class rest_get_file : public resource {
  public:
    rest_get_file(std::string path, 
                  std::string filename);

    ~rest_get_file() override = default;
    
    resource::resp_type get(resource::req_type body) override;


  protected:
    std::string get_file_contents();


  protected:
    std::string filename_;

};

}   // namespace captiverc


#endif
