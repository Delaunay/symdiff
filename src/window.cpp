#include <Thor/Shapes/ConcaveShape.hpp>
#include <Thor/Shapes/Shapes.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class System{

public:
    sf::Font _font;
};

constexpr float length1{25};
constexpr float offset_depth{5};
constexpr float offset_length{5};
constexpr float length2{12.5};
constexpr float margin{10};
constexpr float height{10};

// limits
constexpr float min_width{length1 + offset_length * 2 + length2 + margin};
constexpr float min_height{height + margin};

class Block: public thor::ConcaveShape
{
public:
    static const int point_number{12};

    /*
     *     length1   + length2 + length3
     *
     *  .------------\_______/--------------------. Offset
     *  |       This is a block                   |
     *  '------------\_______/--------------------'
     */

    Block()
    {
        setPointCount(point_number);
        setFillColor(sf::Color(200, 100, 100));
        setOutlineColor(sf::Color(255, 100, 100));
        setOutlineThickness(2.f);

        setPoint(0, sf::Vector2f(0, 0));
        setPoint(1, sf::Vector2f(length1, 0));
        setPoint(2, sf::Vector2f(length1 + offset_length, offset_depth));
        setPoint(3, sf::Vector2f(length1 + offset_length + length2, offset_depth));
        setPoint(4, sf::Vector2f(length1 + offset_length * 2 + length2, 0));
        setPoint(5, sf::Vector2f(min_width, 0));

        for(std::size_t i = 0; i < 6; ++i)
            setPoint(11 - i, getPoint(i) + sf::Vector2f(0, min_height));
    }

    void set_width(float f){
        f = std::max(f, min_width);
        setPoint(5, sf::Vector2f(f, 0));
        setPoint(6, sf::Vector2f(f, min_height));
    }
};

class Statement: public thor::ConcaveShape
{
public:
    static constexpr float stmt_margin{10};

    Statement()
    {
        // Top Block
        // -----------------
        setPointCount(24);
        setFillColor(sf::Color(200, 100, 100));
        setOutlineColor(sf::Color(255, 100, 100));
        setOutlineThickness(2.f);

        setPoint(0, sf::Vector2f(0, 0));
        setPoint(1, sf::Vector2f(length1, 0));
        setPoint(2, sf::Vector2f(length1 + offset_length, offset_depth));
        setPoint(3, sf::Vector2f(length1 + offset_length + length2, offset_depth));
        setPoint(4, sf::Vector2f(length1 + offset_length * 2 + length2, 0));
        setPoint(5, sf::Vector2f(min_width + length1, 0));

        // samething but shifted by length1
        for(std::size_t i = 0; i < 5; ++i)
            setPoint(11 - i, getPoint(i) + sf::Vector2f(stmt_margin, min_height));

        // except point 6 which is not shifted
        setPoint(6, getPoint(5) + sf::Vector2f(0, min_height));

        // Bot Block
        // ----------------
        float block_height = 50;

        for(std::size_t i = 0; i < 6; ++i)
            setPoint(i + 12, getPoint(11 - i) + sf::Vector2f(0, block_height));

        // samething but shifted by length1
        for(std::size_t i = 0; i < 5; ++i)
            setPoint(11 - i + 12, getPoint(i + 12) + sf::Vector2f(-stmt_margin, min_height));

        setPoint(6 + 12, getPoint(5 + 12) + sf::Vector2f(0, min_height));
    }

};

struct Cursor{
    sf::Shape* selected{nullptr};
};


int main()
{
    // Create render window
    sf::RenderWindow window(sf::VideoMode(600, 500), "Thor Shapes", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Create a concave shape by directly inserting the polygon points
    Statement b;
    //b.set_width(100);
    b.setPosition(20, 20);

    sf::RectangleShape r1; r1.setSize(sf::Vector2f(1, 1000));
    sf::RectangleShape r2; r2.setSize(sf::Vector2f(1000, 1));
    sf::CircleShape c(5);

    sf::RectangleShape b1; b1.setFillColor(sf::Color(0, 0, 0, 0));
    b1.setOutlineColor(sf::Color(0, 255, 0)); b1.setOutlineThickness(2.f);

    sf::RectangleShape b2; b2.setFillColor(sf::Color(0, 0, 0, 0));
    b2.setOutlineColor(sf::Color(0, 255, 0));  b2.setOutlineThickness(2.f);

    Cursor cursor;

    // Main loop
    while(window.isOpen())
    {
        // Mouse Locator
        sf::Vector2i p = sf::Mouse::getPosition(window);

        r2.setPosition(0.f, p.y);
        r1.setPosition(p.x, 0.f);
        c.setPosition(p.x - 2.5f,p. y - 2.5f);

        // Bound box
        sf::Rect<float> cu = c.getGlobalBounds();
        sf::Rect<float> bu = b.getGlobalBounds();

        b1.setSize(sf::Vector2f(cu.width, cu.height));
        b1.setPosition(cu.left, cu.top);
        b2.setSize(sf::Vector2f(bu.width, bu.height));
        b2.setPosition(bu.left, bu.top);

        // Event handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::MouseButtonPressed:{
                    if (sf::Mouse::Button::Left == event.mouseButton.button){
                        if (bu.intersects(cu)){
                            cursor.selected = reinterpret_cast<sf::Shape*>(&b);
                            b.setFillColor(sf::Color(0, 0, 255));
                        }
                    }
                    break;
                }

                case sf::Event::MouseButtonReleased:{
                    cursor.selected = nullptr;
                    b.setFillColor(sf::Color(255, 0, 0));
                    break;
                }

                case sf::Event::MouseMoved:{
                    if (cursor.selected){
                        cursor.selected->setPosition(event.mouseMove.x, event.mouseMove.y);
                    }

                    break;
                }

                case sf::Event::Closed:
                    return 0;
            }
        }



        // Draw everything
        window.clear();
        window.draw(b1);
        window.draw(b2);

        window.draw(b);
        window.draw(r1);
        window.draw(r2);
        window.draw(c);

        window.display();
    }
}
