#include <SFML/Graphics.hpp>

// Function to set a pixel to a specific color in an sf::Image
void SetPixel(sf::Image &image, unsigned int x, unsigned int y,
              sf::Color color) {
    if (x < image.getSize().x && y < image.getSize().y) {
        image.setPixel(x, y, color);
    }
}

int main() {
    static constexpr uint16_t kW = 800;
    static constexpr uint16_t kH = 600;

    // Create a window
    sf::RenderWindow window(sf::VideoMode(kW, kH), "Set Pixel Example");

    // Create an image
    sf::Image image;
    image.create(kW, kH, sf::Color::Black);

    // Set a pixel to red
    SetPixel(image, kW / 2, kH / 2, sf::Color::Red);

    // Load the image into a texture
    sf::Texture texture;
    texture.loadFromImage(image);

    // Create a sprite that we can draw onto the screen
    sf::Sprite sprite;
    sprite.setTexture(texture);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(sprite); // Draw the sprite with the modified image
        window.display();
    }

    return 0;
}
