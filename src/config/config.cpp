#include "config.hpp"
#include <cstring>

Config& Config::get_instance() {
    static Config instance;
    return instance;
}

bool Config::load_from_command_line(int argc, char* argv[]) {
    // Default values
    node_role = "dual";
    port = 8001;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (std::strcmp(argv[i], "--role") == 0 && i + 1 < argc) {
            node_role = argv[++i];
        } else {
            // Unknown argument
            return false;
        }
    }

    return true;
}
