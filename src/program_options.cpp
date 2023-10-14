#include "program_options.h"

#include <vector>
#include <iostream>

/**
 * Get options passed to the program
 *
 * @param args
 * @param option_name
 * @return
 */
std::string_view program_options::get(
        const std::vector<std::string_view> &args,
        const std::string_view &option_name) {
    for (auto it = args.begin(), end = args.end(); it != end; ++it) {
        if (*it == option_name)
            if (it + 1 != end)
                return *(it + 1);
    }

    return "";
}

/**
 * Check if option exists
 *
 * @param args
 * @param option_name
 * @return
 */
bool program_options::has(
        const std::vector<std::string_view> &args,
        const std::string_view &option_name) {
    for (auto arg: args) {
        if (arg == option_name)
            return true;
    }

    return false;
}

/**
 * Output description
 */
void program_options::description() {
    std::cout.setf(std::ios::right, std::ios::adjustfield);
    std::cout.width(40);
    std::cout << "Usage of the forest fire simulator" << std::endl << std::endl;
    std::cout.setf(std::ios::right, std::ios::adjustfield);
    std::cout.width(26);
    std::cout << "-m: Measurement" << std::endl;
    std::cout.width(19);
    std::cout << "-h: Help" << std::endl << std::endl;
}
