//
// Created by Adnan Vatric on 18.02.23.
//

#ifndef FOREST_FIRE_PROGRAM_OPTIONS_H
#define FOREST_FIRE_PROGRAM_OPTIONS_H

#include <string_view>
#include <vector>

namespace program_options {
    bool has(const std::vector<std::string_view> &args, const std::string_view &option_name);

    std::string_view get(const std::vector<std::string_view> &args, const std::string_view &option_name);

    void description();
}

#endif //FOREST_FIRE_PROGRAM_OPTIONS_H
