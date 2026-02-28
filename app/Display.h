#ifndef DISPLAY
#define DISPLAY

#include "ConvexHull.h"
#include <SFML/Graphics.hpp>
#include <string>

class Display : private JarvisAlgorithm, private GrahamAlgorithm {
private:
    sf::RenderWindow window;
    sf::Font font;

    std::vector<Point> points;
    std::vector<Point> hull;

    sf::RectangleShape loadButton;
    sf::Text loadText;

    sf::RectangleShape findButtonJarvis;
    sf::Text findTextJarvis;

    sf::RectangleShape findButtonGraham;
    sf::Text findTextGraham;

    sf::RectangleShape exitButton;
    sf::Text exitText;

    sf::Text infoText;
    bool showInfo = false;

    sf::Color algorithmColor = sf::Color::Red;

    sf::RectangleShape coordinatePlane;

    void drawGrid();
    void drawPoints();
    void drawHull();
    void drawButtons();
    void drawInfo();
    void convertToScreen(Point& p);

public:
    Display();
    void run();
    void loadPointsFromFile(const std::string& filename);
};

#endif