#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "http_server.h"
#include "jansson.h"
#include "resource.h"
#include "rest_get_file.h"
#include "rest_reboot.h"

namespace captiverc {

////////////////////////////////////////////////////////////////////////////////
/// constructor
////////////////////////////////////////////////////////////////////////////////
rest_reboot::rest_reboot (std::string path, std::string reboot_exe):
            resource(path),
            reboot_exe_(reboot_exe)
    {}

////////////////////////////////////////////////////////////////////////////////
/// put
/// Implement the 'put' functionality
/// Changes the file and then returns its conents as a JSON value.
////////////////////////////////////////////////////////////////////////////////
resource::resp_type
rest_reboot::put(resource::req_type body){
    //parse the json
    json_t* root;
    json_error_t error;

    std::cout << "The JSON payload is: " << body << std::endl;
    root = json_loads(body.c_str(), JSON_DECODE_ANY, &error);

    pid_t PID = fork();
    if (PID < 0){
        std::cerr << "Unable to fork process for rebooting." << std::endl;
        return std::make_tuple(HTTP_INTERNAL, "\"ERROR - Unable to reboot\"");
    } else if (PID == 0) {
        std::cout << "-------- Rebooting in 5 seconds ----------" << std::endl;
        usleep (5000000);   // sleep for 5 sec
        system(reboot_exe_.c_str());
        exit(0);
    } else {
        return std::make_tuple(HTTP_OK, "\"Reboot Scheduled\"");
    }

    return std::make_tuple(HTTP_INTERNAL, "\"Internal error - reached dead code.\"");
}

}   // namespace captiverc
