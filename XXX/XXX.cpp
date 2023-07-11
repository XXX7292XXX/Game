#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

const int Width = 1920;
const int Height = 1080;
const float EnemySpeed = 0.25f;
bool GameOver;

void processEvents(RenderWindow& window)
{
    Event event;
    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
        {
            window.close();
        }
    }
}

void characterLogic(Sprite& characterSprite, const Texture& rightTexture, const Texture& leftTexture, int& x)
{
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        characterSprite.setTexture(rightTexture);
        characterSprite.move(1, 0);
        x += 1;
    }
    else if (Keyboard::isKeyPressed(Keyboard::A))
    {
        characterSprite.setTexture(leftTexture);
        characterSprite.move(-1, 0);
        x -= 1;
    }
    x = (x > Width - 30) ? (characterSprite.setPosition(31 - 100, 850), x - Width + 59) : (x < 1 + 30) ? (characterSprite.setPosition(Width - 30, 850), x + Width - 59) : x;
}

bool checkCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2)
{
    sf::FloatRect bounds1 = sprite1.getGlobalBounds();
    sf::FloatRect bounds2 = sprite2.getGlobalBounds();
    return bounds1.intersects(bounds2);
}

void enemyLogic(Sprite& enemySprite, const Sprite& characterSprite)
{
    Vector2f enemyPosition = enemySprite.getPosition();
    Vector2f characterPosition = characterSprite.getPosition();
    Vector2f direction = characterPosition - enemyPosition;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0){direction /= distance;}

    float speed = EnemySpeed + static_cast<float>(rand() % 5) / 500.0f;
    enemySprite.move(direction * speed);

    if (checkCollision(enemySprite, characterSprite)){GameOver = true;}
}

void render(RenderWindow& window, Sprite& backgroundSprite, Sprite& characterSprite, int enemyCount, Sprite* enemySprites)
{
    window.clear(Color::Black);
    window.draw(backgroundSprite);
    window.draw(characterSprite);
    for (int i = 0; i < enemyCount; ++i)
    {
        window.draw(enemySprites[i]);
    }

    window.display();
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    RenderWindow window(VideoMode(Width, Height), "Game");

    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("D:\\c++\\Images\\fon.png"))
    {
        cout << "Failed to load background texture!" << endl;
        return -1;
    }

    Texture characterTextureRight;
    if (!characterTextureRight.loadFromFile("D:\\c++\\Images\\character_right.png"))
    {
        cout << "Failed to load character right texture!" << endl;
        return -1;
    }

    Texture characterTextureLeft;
    if (!characterTextureLeft.loadFromFile("D:\\c++\\Images\\character_left.png"))
    {
        cout << "Failed to load character left texture!" << endl;
        return -1;
    }

    Texture enemyTexture;
    if (!enemyTexture.loadFromFile("D:\\c++\\Images\\enemy.png"))
    {
        cout << "Failed to load enemy texture!" << endl;
        return -1;
    }

    Sprite backgroundSprite(backgroundTexture);

    int characterX = Width / 2;
    Sprite characterSprite(characterTextureRight);
    characterSprite.setPosition(Width / 2, 850);

    const int enemyCount = 5;
    Sprite* enemySprites = new Sprite[enemyCount];
    for (int i = 0; i < enemyCount; ++i)
    {
        enemySprites[i].setTexture(enemyTexture);
        enemySprites[i].setPosition(rand() % (Width - 100), 50);
    }

    while (window.isOpen())
    {

        processEvents(window);
        if (!GameOver) 
        {
            characterLogic(characterSprite, characterTextureRight, characterTextureLeft, characterX);
            for (int i = 0; i < enemyCount; ++i)
            {
                enemyLogic(enemySprites[i], characterSprite);
            }
        }

        render(window, backgroundSprite, characterSprite, enemyCount, enemySprites);
    }

    delete[] enemySprites;
    return 0;
}
