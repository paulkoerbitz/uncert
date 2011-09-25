#ifndef SMALL_UTILS_HPP__
#define SMALL_UTILS_HPP__

void not_implemented_error(std::string &mode)
{
    std::cerr << "Mode " << mode << " not implemented, exiting..." 
        << std::endl;
    throw ("Mode " + mode + " not implemented.");
}

#endif
