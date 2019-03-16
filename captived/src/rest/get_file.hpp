#ifndef GET_FILE_H
#define GET_FILE_H

#include <string>

#include "rest/resource.hpp"

namespace captiverc {
namespace rest {

class get_file : public resource {
  public:
    get_file(std::string path, 
             std::string filename);

    ~get_file() override = default;
    
    resource::resp_type get(resource::req_type body) override;


  protected:
    std::string get_file_contents();


  protected:
    std::string filename_;

};

} // namespace rest
} // namespace captiverc


#endif
