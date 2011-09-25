#include <iostream>
#include <boost/timer.hpp>
#include <string>

namespace QuantLibExt {

class fancytimer 
{
    public:
        fancytimer(std::string s = "Time elapsed: ") : t_(), message_(s) {}
        ~fancytimer();
        double elapsed();
        void print_elapsed();
        static void print_time(int hours, int minutes, double seconds);

    private:
        void update_fields();
        boost::timer t_;
        std::string message_;
        double seconds_;
        int hours_;
        int minutes_;
};

fancytimer::~fancytimer()
{
    std::cerr << message_ ;
    print_elapsed();
}

void fancytimer::update_fields()
{
    seconds_ = elapsed();
    hours_ = int(seconds_/3600);
    seconds_ -= hours_ * 3600;
    minutes_ = int(seconds_/60);
    seconds_ -= minutes_ * 60;
}

double fancytimer::elapsed()
{
    return t_.elapsed();
}

void fancytimer::print_time(int hours, int minutes, double seconds)
{
    if (hours > 0)
        std::cerr << hours << " h ";
    if (hours > 0 || minutes > 0)
        std::cerr << minutes << " m ";
    std::cerr << std::fixed << std::setprecision(0)
              << seconds << " s\n" << std::endl;
}

void fancytimer::print_elapsed()
{
    update_fields();
    print_time(hours_, minutes_, seconds_);
}

}  // namespace QuantLibExt
