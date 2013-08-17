#include <SFML/Graphics.hpp>

int main()
{
    // Create the main window
    sf::RenderWindow App(sf::VideoMode(800, 600), "SFML window");

    // Load a sprite to display
    sf::Image Image;
    if (!Image.LoadFromFile("cb.bmp"))
        return EXIT_FAILURE;
    sf::Sprite Sprite(Image);

	// Start the game loop
    while (App.IsOpened())
    {
        // Process events
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();
        }

        // Clear screen
        App.Clear();

        // Draw the sprite
        App.Draw(Sprite);

        // Update the window
        App.Display();
    }

    return EXIT_SUCCESS;
}
