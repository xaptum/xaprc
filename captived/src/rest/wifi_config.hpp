#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include <string>

#include "rest/resource.hpp"

namespace captiverc {

class rest_wifi_config : public resource {
  public:
    rest_wifi_config(std::string path, 
                  std::string filename);

    ~rest_wifi_config() override = default;

    resource::resp_type get(resource::req_type body) override;
    resource::resp_type put(resource::req_type body) override;

  protected:
    std::string get_entire_file();

  protected:
    std::string filename_;

};

}   // namespace captiverc


#endif
