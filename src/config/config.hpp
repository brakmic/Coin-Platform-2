#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

class Config {
public:
    static Config& get_instance();

    bool load_from_command_line(int argc, char* argv[]);

    std::string node_role;
    int port;

private:
    Config() = default;
};

#endif // CONFIG_HPP
