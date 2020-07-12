#include "controller.hxx"

#include <stdexcept>
#include <string>

using namespace std;

int main(int argc, char* argv[])

try {
    ///
    /// Don't change this command-line processing, as the grader
    /// will expect to be able to start your program this way.
    ///
    int width, height;
    switch (argc) {
        case 1:
            width  = height = 4;
            break;
        case 3:
            width  = stoi(argv[1]);
            height = stoi(argv[2]);
            break;
        default:
            cerr << "Usage: " << argv[0] << " [WIDTH HEIGHT]\n";
            return 1;
    }

    // You can change how you start your game class if you want (but
    // you probably don't need to):
    Controller(width, height).run();
}

// This prints out error messages if, say, the command-line argument
// cannot be parsed as `int`s.
catch (exception const& e) {
    cerr << argv[0] << ": " << e.what() << "\n";
    return 1;
}
