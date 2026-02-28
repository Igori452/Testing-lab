#include "Display.h"
#include <fstream>

Display::Display()
    : window(sf::VideoMode({ 900u, 700u }), "Convex Hull Visualizer"),
    font(),
    loadText(font), findTextJarvis(font), findTextGraham(font),
    exitText(font), infoText(font) {
    window.setFramerateLimit(60);

    if (!font.openFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"))
    {
        font.openFromFile("C:/Windows/Fonts/arial.ttf");
    }

    float buttonX = 700.f;
    float startY = 50.f + 50.f;
    float spacing = 80.f;

    sf::Vector2f buttonSize = { 170.f, 40.f };

    // ---------- Find Jarvis Button ----------
    findButtonJarvis.setSize(buttonSize);
    findButtonJarvis.setPosition({ buttonX, startY });
    findButtonJarvis.setFillColor(sf::Color(100, 150, 250));
    findButtonJarvis.setOutlineThickness(2.f);
    findButtonJarvis.setOutlineColor(sf::Color::Red);

    findTextJarvis.setString("Find Jarvis Hull");
    findTextJarvis.setCharacterSize(18);
    findTextJarvis.setFillColor(sf::Color::White);
    findTextJarvis.setPosition({ buttonX + 20.f, startY + 10.f });

    // ---------- Find Graham Button ----------
    findButtonGraham.setSize(buttonSize);
    findButtonGraham.setPosition({ buttonX, startY + spacing });
    findButtonGraham.setFillColor(sf::Color(100, 150, 250));
    findButtonGraham.setOutlineThickness(2.f);
    findButtonGraham.setOutlineColor(sf::Color::Green);

    findTextGraham.setString("Find Graham Hull");
    findTextGraham.setCharacterSize(18);
    findTextGraham.setFillColor(sf::Color::White);
    findTextGraham.setPosition({ buttonX + 20.f, startY + spacing + 10.f });

    // ---------- Load Button ----------
    loadButton.setSize(buttonSize);
    loadButton.setPosition({ buttonX, startY + spacing * 2 });
    loadButton.setFillColor(sf::Color(100, 150, 250));
    loadButton.setOutlineThickness(2.f);
    loadButton.setOutlineColor(sf::Color::White);

    loadText.setString("Load File");
    loadText.setCharacterSize(18);
    loadText.setFillColor(sf::Color::White);
    loadText.setPosition({ buttonX + 20.f, startY + spacing * 2 + 10.f });

    // ---------- Exit Button ----------
    exitButton.setSize(buttonSize);
    exitButton.setPosition({ buttonX, startY + spacing * 3 });
    exitButton.setFillColor(sf::Color(200, 100, 100));
    exitButton.setOutlineThickness(2);
    exitButton.setOutlineColor(sf::Color::White);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(18);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition({ buttonX + 20.f, startY + spacing * 3 + 10.f });

    // ---------- Info Text ----------
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setPosition({ buttonX, startY - 50.f });
    infoText.setString("Cannot build Convex Hull!");

    // ---------- Coordinate Plane ----------
    coordinatePlane.setSize({ 600.f, 600.f });
    coordinatePlane.setPosition({ 50.f, 50.f });
    coordinatePlane.setFillColor(sf::Color(240, 240, 240));
    coordinatePlane.setOutlineThickness(2.f);
    coordinatePlane.setOutlineColor(sf::Color::Black);
}

void Display::convertToScreen(Point& p)
{
    float startX = coordinatePlane.getPosition().x;
    float startY = coordinatePlane.getPosition().y;

    p.x = startX + p.x;
    p.y = startY + p.y;
}

void Display::drawGrid()
{
    sf::VertexArray lines(sf::PrimitiveType::Lines);

    float startX = coordinatePlane.getPosition().x;
    float startY = coordinatePlane.getPosition().y;
    float width = coordinatePlane.getSize().x;
    float height = coordinatePlane.getSize().y;

    float step = 50.f;

    for (float x = startX + step; x < startX + width; x += step)
    {
        lines.append(sf::Vertex{ {x, startY}, sf::Color(200,200,200) });
        lines.append(sf::Vertex{ {x, startY + height}, sf::Color(200,200,200) });
    }

    for (float y = startY + step; y < startY + height; y += step)
    {
        lines.append(sf::Vertex{ {startX, y}, sf::Color(200,200,200) });
        lines.append(sf::Vertex{ {startX + width, y}, sf::Color(200,200,200) });
    }

    for (float x = startX; x <= startX + width; x += step)
    {
        if (x <= startX || x >= startX + width) continue;

        lines.append(sf::Vertex{ {x, startY + height / 2 - 5}, sf::Color::Black });
        lines.append(sf::Vertex{ {x, startY + height / 2 + 5}, sf::Color::Black });

        float centerX = startX + width / 2;
        if (fabs(x - centerX) > 0.1f) {
            sf::Text label(font);
            label.setString(std::to_string((int)(x - startX)));
            label.setCharacterSize(12);
            label.setFillColor(sf::Color::Black);

            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
            label.setPosition(sf::Vector2f(x, startY + height / 2 + 15));
            window.draw(label);
        }
    }

    for (float y = startY; y <= startY + height; y += step)
    {
        if (y <= startY || y >= startY + height) continue;

        lines.append(sf::Vertex{ {startX + width / 2 - 5, y}, sf::Color::Black });
        lines.append(sf::Vertex{ {startX + width / 2 + 5, y}, sf::Color::Black });

        float centerY = startY + height / 2;
        if (fabs(y - centerY) > 0.1f) {
            sf::Text label(font);
            label.setString(std::to_string((int)(y - startY)));
            label.setCharacterSize(12);
            label.setFillColor(sf::Color::Black);

            sf::FloatRect bounds = label.getLocalBounds();
            label.setOrigin(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
            label.setPosition(sf::Vector2f(startX + width / 2 + 18, y - 3));
            window.draw(label);
        }
    }

    window.draw(lines);
}

void Display::drawPoints()
{
    for (const auto& p : points)
    {
        Point screen = p;
        convertToScreen(screen);

        sf::CircleShape circle(3.f);
        circle.setFillColor(sf::Color::Blue);
        circle.setPosition(sf::Vector2f(
            static_cast<float>(screen.x - 4),
            static_cast<float>(screen.y - 4)
        ));

        window.draw(circle);
    }
}

void Display::drawHull()
{
    if (hull.size() < 3) return;

    for (size_t i = 0; i < hull.size(); ++i)
    {
        size_t next = (i + 1) % hull.size();

        Point p1 = hull[i];
        Point p2 = hull[next];

        convertToScreen(p1);
        convertToScreen(p2);

        sf::Vertex line[2];

        line[0].position = {
            static_cast<float>(p1.x),
            static_cast<float>(p1.y)
        };
        line[0].color = algorithmColor;

        line[1].position = {
            static_cast<float>(p2.x),
            static_cast<float>(p2.y)
        };
        line[1].color = algorithmColor;

        window.draw(line, 4, sf::PrimitiveType::Lines);
    }
}

void Display::drawButtons()
{
    window.draw(loadButton);
    window.draw(loadText);

    window.draw(findButtonJarvis);
    window.draw(findTextJarvis);

    window.draw(findButtonGraham);
    window.draw(findTextGraham);

    window.draw(exitButton);
    window.draw(exitText);
}

void Display::drawInfo()
{
    if (showInfo)
        window.draw(infoText);
}

void Display::loadPointsFromFile(const std::string& filename)
{
    points.clear();
    hull.clear();

    std::ifstream file(filename);
    if (!file.is_open()) return;

    double x, y;
    while (file >> x >> y)
        points.emplace_back(x, y);
}

void Display::run()
{
    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    auto mousePos = sf::Mouse::getPosition(window);

                    sf::Vector2f mouseF = {
                        static_cast<float>(mousePos.x),
                        static_cast<float>(mousePos.y)
                    };

                    if (loadButton.getGlobalBounds().contains(mouseF))
                        loadPointsFromFile("points.txt");

                    if (findButtonJarvis.getGlobalBounds().contains(mouseF)) {
                        hull = JarvisAlgorithm::findConvexHull(points);
                        algorithmColor = sf::Color::Red;
                        infoText.setFillColor(algorithmColor);
                        if (hull.empty()) showInfo = true;
                        else showInfo = false;
                    }

                    if (findButtonGraham.getGlobalBounds().contains(mouseF)) {
                        hull = GrahamAlgorithm::findConvexHull(points);
                        algorithmColor = sf::Color::Green;
                        infoText.setFillColor(algorithmColor);
                        if (hull.empty()) showInfo = true;
                        else showInfo = false;
                    }

                    if (exitButton.getGlobalBounds().contains(mouseF))
                        window.close();
                }
            }
        }

        window.clear(sf::Color(50, 50, 50));
        window.draw(coordinatePlane);
        drawGrid();
        drawPoints();
        drawHull();
        drawButtons();
        drawInfo();
        window.display();
    }
}