#ifndef PUT_FILE_H
#define PUT_FILE_H

#include <string>

#include "rest/resource.hpp"
#include "rest/get_file.hpp"

namespace captiverc {
namespace rest {

class put_file : public get_file {
  public:
    put_file(std::string path, 
             std::string filename);

    resource::resp_type put(resource::req_type body) override;

};

} // namespace rest
} // namespace captiverc


#endif
