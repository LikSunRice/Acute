#include "Application.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    std::cout << "Acute Drawing Software" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  - Left Mouse Button: Draw" << std::endl;
    std::cout << "  - Ctrl+C: Clear canvas" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
    std::cout << std::endl;
    
    Acute::Application app;
    
    if (!app.initialize("Acute - Drawing Software", 1280, 720)) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    app.run();
    app.shutdown();
    
    return 0;
}


