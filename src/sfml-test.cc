#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <cstddef>
#include <iostream>

int main() {
  sf::RenderWindow window(sf::VideoMode(255, 255), "I am title");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      window.clear();
      for (size_t i = 0; i < 255; ++i) {
        for (size_t j = 0; j < 255; ++j) {
          sf::RectangleShape rectangle(sf::Vector2f(1, 1));
          rectangle.setPosition(sf::Vector2f(i, j));
          rectangle.setFillColor(sf::Color(0, i, j));
          window.draw(rectangle);
        }
      }
      window.display();
    }
  }

  return 0;
}