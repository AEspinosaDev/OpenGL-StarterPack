#include "application.h"

int main(int, char **)
{
    try
    {
        Window window{};
        window.title = "PGATR Practica 3";
        window.extent = {1280, 720};
        Application app(window);
        
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}
