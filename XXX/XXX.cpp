#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#define _DEBUG

using namespace std;
using namespace sf;

const int Width = 1920;
const int Height = 1080;
const float EnemySpeed = 0.25f;
const float BulletSpeed = 0.5f;
bool GameOver;

Texture textureLoad(const string& filePath)
{
    Texture texture;
    if (!texture.loadFromFile(filePath))
    {
        cout << "Failed to load texture: " << filePath << endl;
    }
    return texture;
}

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

bool checkCollision(const Sprite& sprite1, const Sprite& sprite2)
{
    FloatRect bounds1 = sprite1.getGlobalBounds();
    FloatRect bounds2 = sprite2.getGlobalBounds();
    return bounds1.intersects(bounds2);
}

void characterLogic(Sprite& characterSprite, const Texture& texture, float& x, float& y, bool& isCharacterRight)
{
    if (Keyboard::isKeyPressed(Keyboard::D))
    {
        characterSprite.setTexture(texture);
        characterSprite.setScale(1, 1);
        isCharacterRight = true;
        characterSprite.move(1, 0);
        x += 1;
    }
    else if (Keyboard::isKeyPressed(Keyboard::A))
    {
        characterSprite.setTexture(texture);
        characterSprite.setScale(-1, 1);
        isCharacterRight = false;
        characterSprite.move(-1, 0);
        x -= 1;
    }
    if (x > Width - 100) {
        x = 0;
        characterSprite.setPosition(x, y);
    }
    else if (x < 0) {
        x = Width - 100;
        characterSprite.setPosition(x, y);
    }
    else {
        characterSprite.setPosition(x, y);
    }
}

void bulletLogic(vector<Sprite>& bulletSprites, Texture& bulletUpTexture, Texture& bulletLeftTexture, Texture& bulletRightTexture, float& x, float& y, vector<Sprite>& enemySprites, Sprite& characterSprite, bool& isCharacterRight)
{
    static bool isUpKeyPressed = false;
    static bool isLeftKeyPressed = false;
    static bool isRightKeyPressed = false;

    if (Keyboard::isKeyPressed(Keyboard::Left) && !isLeftKeyPressed)
    {
        isLeftKeyPressed = true;
        Sprite bulletSprite(bulletLeftTexture);
        if (isCharacterRight) { bulletSprite.setPosition(x, y + 50); }
        else { bulletSprite.setPosition(x - 50, y + 50); }
        bulletSprites.push_back(bulletSprite);
    }
    else if (!Keyboard::isKeyPressed(Keyboard::Left))
    {
        isLeftKeyPressed = false;
    }

    if (Keyboard::isKeyPressed(Keyboard::Right) && !isRightKeyPressed)
    {
        isRightKeyPressed = true;
        Sprite bulletSprite(bulletRightTexture);
        if (isCharacterRight) { bulletSprite.setPosition(x + 50, y + 50); }
        else { bulletSprite.setPosition(x - 50, y + 50); }
        bulletSprites.push_back(bulletSprite);
    }
    else if (!Keyboard::isKeyPressed(Keyboard::Right))
    {
        isRightKeyPressed = false;
    }

    if (Keyboard::isKeyPressed(Keyboard::Up) && !isUpKeyPressed)
    {
        isUpKeyPressed = true;
        Sprite bulletSprite(bulletUpTexture);
        if (isCharacterRight) { bulletSprite.setPosition(x + 26, y); }
        else { bulletSprite.setPosition(x - 47, y); }
        bulletSprites.push_back(bulletSprite);
    }
    else if (!Keyboard::isKeyPressed(Keyboard::Up))
    {
        isUpKeyPressed = false;
    }

    for (auto& bulletSprite : bulletSprites)
    {
        if (bulletSprite.getTexture() == &bulletUpTexture)
        {
            bulletSprite.move(0, -BulletSpeed);
        }
        else if (bulletSprite.getTexture() == &bulletLeftTexture)
        {
            bulletSprite.move(-BulletSpeed, 0);
        }
        else if (bulletSprite.getTexture() == &bulletRightTexture)
        {
            bulletSprite.move(BulletSpeed, 0);
        }

        if (bulletSprite.getPosition().y < 0 || bulletSprite.getPosition().x > Width || bulletSprite.getPosition().x < 0)
        {
            bulletSprite.setPosition(-100, -100);
        }
        else
        {
            for (auto& enemySprite : enemySprites)
            {
                if (checkCollision(bulletSprite, enemySprite))
                {
                    enemySprite.setPosition(static_cast<float>(rand() % (Width - 100)), 50);
                    bulletSprite.setPosition(-100, -100);
                    break;
                }
            }
        }
    }

    bulletSprites.erase(remove_if(bulletSprites.begin(), bulletSprites.end(), [](const Sprite& bulletSprite) {
        return bulletSprite.getPosition().x < 0 || bulletSprite.getPosition().x > Width || bulletSprite.getPosition().y < 0;
        }), bulletSprites.end());
}

void enemyLogic(Sprite& enemySprite, Sprite& characterSprite, Sprite* bulletSprite)
{
    enemySprite.move(EnemySpeed,EnemySpeed);
    if (enemySprite.getGlobalBounds().top > Height + 10 || enemySprite.getGlobalBounds().left < -25 || enemySprite.getGlobalBounds().left > Width + 20) {
        enemySprite.setPosition(std::rand() % static_cast<int>(Width - enemySprite.getGlobalBounds().width), std::rand() % (-100 - (-40) + 1) + (-40));
    }
    Vector2f enemyPosition = enemySprite.getPosition();
    Vector2f characterPosition = characterSprite.getPosition();
    Vector2f direction = characterPosition - enemyPosition;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (distance > 0) { direction /= distance; }

    float speed = EnemySpeed + static_cast<float>(rand() % 5) / 500.0f;
    enemySprite.move(direction * speed);

    if (bulletSprite && checkCollision(enemySprite, *bulletSprite))
    {
        bulletSprite->setPosition(-100, -100);
    }

    if (checkCollision(enemySprite, characterSprite)) { GameOver = true; }
}

void render(RenderWindow& window, Sprite& backgroundSprite, Sprite& characterSprite, vector<Sprite>& enemySprites, vector<Sprite>& bulletSprites)
{
    window.clear(Color::Black);
    window.draw(backgroundSprite);
    window.draw(characterSprite);
    for (auto& bulletSprite : bulletSprites)
    {
        window.draw(bulletSprite);
    }
    for (auto& enemySprite : enemySprites)
    {
        window.draw(enemySprite);
    }

    window.display();
}

int main()
{
    srand(static_cast<unsigned int>(time(NULL)));
    RenderWindow window(VideoMode(Width, Height), "Game");

    const int enemyCount = 5;
    float characterX = static_cast<float>(Width / 2);
    float characterY = static_cast<float>(850);
    bool isCharacterRight = true;

#ifdef _RELEASE
    Texture backgroundTexture = textureLoad("GameFiles/Images/fon.png");
    Texture bulletUpTexture = textureLoad("GameFiles/Images/bullet_up.png");
    Texture bulletLeftTexture = textureLoad("GameFiles/Images/bullet_left.png");
    Texture bulletRightTexture = textureLoad("GameFiles/Images/bullet_right.png");
    Texture characterTexture = textureLoad("GameFiles/Images/character_right.png");
    Texture enemyTexture = textureLoad("GameFiles/Images/enemy.png");
#else
    Texture backgroundTexture = textureLoad("D:\\c++\\Images\\fon.png");
    Texture bulletUpTexture = textureLoad("D:\\c++\\Images\\bullet_up.png");
    Texture bulletLeftTexture = textureLoad("D:\\c++\\Images\\bullet_left.png");
    Texture bulletRightTexture = textureLoad("D:\\c++\\Images\\bullet_right.png");
    Texture characterTexture = textureLoad("D:\\c++\\Images\\character.png");
    Texture enemyTexture = textureLoad("D:\\c++\\Images\\enemy.png");
#endif

    Sprite backgroundSprite(backgroundTexture);
    Sprite characterSprite(characterTexture);
    vector<Sprite> enemySprites(enemyCount);
    vector<Sprite> bulletSprites;

    characterSprite.setPosition(characterX, characterY);

    for (auto& enemySprite : enemySprites)
    {
        enemySprite.setTexture(enemyTexture);
        enemySprite.setPosition(static_cast<float>(rand() % (Width - 100)), 50);
    }

    while (window.isOpen())
    {
        processEvents(window);

        if (!GameOver)
        {
            characterLogic(characterSprite, characterTexture, characterX, characterY, isCharacterRight);
            bulletLogic(bulletSprites, bulletUpTexture, bulletLeftTexture, bulletRightTexture, characterX, characterY, enemySprites, characterSprite, isCharacterRight);
            for (auto& enemySprite : enemySprites)
            {
                enemyLogic(enemySprite, characterSprite, !bulletSprites.empty() ? &bulletSprites[0] : nullptr);
            }
        }

        render(window, backgroundSprite, characterSprite, enemySprites, bulletSprites);
    }

    return 0;
}
