#include <lib/lib.hpp>

#include <boost/program_options.hpp>

#include <chrono>
#include <iostream>

namespace po = boost::program_options;

using stop_watch = xzr::chrono::stop_watch<std::chrono::milliseconds>;

namespace
{
void run()
{
    stop_watch w{};
    while (true)
    {
        char c{};
        std::cin >> c;
        if (c == 'q')
            return;
        if (c == 'a')
            w.start();
        else if (c == 'b')
            w.stop();
        else if (c == 'c')
            w.reset();
        else if (c == 'd')
            std::cout << w.peek().count() << '\n';
    }
}
} // namespace
int main(int ac, char* av[])
{
    std::cout << "Hello stop_watch\n";

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()("help", "produce help message");

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }
        run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception of unknown type!\n";
    }

    return 0;
}
