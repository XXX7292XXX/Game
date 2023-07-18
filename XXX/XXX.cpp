#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <Windows.h>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

const int Width = 1920;
const int Height = 1080;

const int enemyCount = 5;
const int maxBulletCount = 3;

const float EnemySpeed = 200.0f;
const float CharacterSpeed = 1000.0f;
const float BulletSpeed = 1000.0f;

class Game
{
private:
    RenderWindow window;
    vector<Sprite> bulletSprites;
    bool isCharacterRight;
    int bulletCount;
    int killCount = 0;
    bool gameOver;
    bool gameStarted;
    Clock clock;

    Sprite backgroundSprite;
    Sprite gameoverfonSprite;
    Sprite characterSprite;

    Texture backgroundTexture;
    Texture bulletTexture;
    Texture characterTexture;
    Texture enemyTexture;
    Texture gameoverfonTexture;

    Music music;
    Font font;
    time_t startTime;
    float elapsedTime;
    float elapsedTimeSinceEnemy;
    float enemySpeed;
    float characterSpeed;
    float bulletSpeed;

    struct Enemy
    {
        Sprite sprite;
        float x;
        float y;
        float speedX;
        float speedY;
        bool isEnemyRight;
    };

    vector<Enemy> enemies;

    Text startText; // Text for the start button
    bool startButtonPressed; // Flag to indicate if the start button has been pressed

public:
    Game()
        : window(VideoMode(Width, Height), "Game", Style::Fullscreen),
        isCharacterRight(true),
        bulletCount(maxBulletCount),
        gameOver(false),
        gameStarted(false),
        elapsedTime(0.0f),
        elapsedTimeSinceEnemy(0.0f),
        enemySpeed(EnemySpeed),
        characterSpeed(CharacterSpeed),
        bulletSpeed(BulletSpeed),
        startButtonPressed(false)
    {
    }

    void run()
    {
        initialize();
        while (window.isOpen())
        {
            processEvents();
            update();
            render();
        }
    }

private:
    void initialize()
    {
        srand(static_cast<unsigned int>(time(NULL)));
        window.setFramerateLimit(240);
        window.setVerticalSyncEnabled(true);

        if (!music.openFromFile("Gamefiles/Sound/soundtrack.mp3"))
        {
            cout << "Failed to load music" << endl;
        }

        if (!font.loadFromFile("Gamefiles/Font/font.ttf"))
        {
            cout << "Failed to load font" << endl;
        }

        music.setVolume(10);
        music.setLoop(true);
        music.play();

        backgroundTexture = textureLoad("GameFiles/Images/fon.png");
        bulletTexture = textureLoad("GameFiles/Images/bullet.png");
        characterTexture = textureLoad("GameFiles/Images/character.png");
        enemyTexture = textureLoad("GameFiles/Images/enemy.png");
        gameoverfonTexture = textureLoad("GameFiles/Images/gameoverfon.png");

        backgroundSprite.setTexture(backgroundTexture);
        gameoverfonSprite.setTexture(gameoverfonTexture);

        characterSprite.setTexture(characterTexture);
        characterSprite.setPosition(static_cast<float>(Width / 2), static_cast<float>(Height - 230));

        float scaleRatioX = static_cast<float>(Width) / gameoverfonTexture.getSize().x;
        float scaleRatioY = static_cast<float>(Height) / gameoverfonTexture.getSize().y;
        float scaleRatio = std::max(scaleRatioX, scaleRatioY);
        gameoverfonSprite.setScale(scaleRatio, scaleRatio);

        for (int i = 0; i < enemyCount; i++)
        {
            Enemy enemy;
            enemy.sprite.setTexture(enemyTexture);
            enemy.x = static_cast<float>(rand() % (Width - 100));
            enemy.y = -enemy.sprite.getGlobalBounds().height;
            enemy.speedX = static_cast<float>(rand() % 2) == 0 ? -enemySpeed : enemySpeed;
            enemy.speedY = enemySpeed;
            enemy.isEnemyRight = enemy.speedX > 0;
            enemies.push_back(enemy);
        }
        startTime = time(0);
    }

    Texture textureLoad(const string& filePath)
    {
        Texture texture;
        if (!texture.loadFromFile(filePath))
        {
            cout << "Failed to load texture: " << filePath << endl;
        }
        return texture;
    }

    void processEvents()
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (Keyboard::isKeyPressed(Keyboard::Escape))
            {
                window.close();
            }
            else if (gameOver)
            {
                if (event.type == Event::KeyPressed && startButtonPressed) // Check if start button has been pressed
                {
                    restartGame();
                }
                else if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    window.close();
                }
            }
            else if (!gameStarted) // Check if game has not started
            {
                if ((event.type == Event::KeyPressed) && !startButtonPressed)
                {
                    startButtonPressed = true;
                    gameStarted = true;
                }
            }
        }
    }

    void startGame()
    {
        RectangleShape background(Vector2f(Width, Height));
        background.setFillColor(Color::Black);
        Text sartText;
        startText.setFont(font);
        startText.setString("Press any key to start"); // Text for the start button
        startText.setCharacterSize(36 * 2);
        startText.setFillColor(Color::White);
        startText.setOutlineColor(Color::Black);
        startText.setOutlineThickness(5);
        FloatRect textBounds3 = startText.getLocalBounds();
        startText.setOrigin(textBounds3.left + textBounds3.width / 2.0f, textBounds3.top + textBounds3.height / 2.0f);
        startText.setPosition(Width / 2.0f, Height / 2.0f);

        window.draw(background); window.draw(startText);
    }
    void restartGame()
    {
        gameOver = false;
        killCount = 0;
        startTime = time(0);
        elapsedTimeSinceEnemy = 0.0f;
        enemySpeed = EnemySpeed;
        characterSpeed = CharacterSpeed;
        bulletSpeed = BulletSpeed;

        enemies.clear();

        for (int i = 0; i < enemyCount; i++)
        {
            Enemy enemy;
            enemy.sprite.setTexture(enemyTexture);
            enemy.x = static_cast<float>(rand() % (Width - 100));
            enemy.y = -enemy.sprite.getGlobalBounds().height;
            enemy.speedX = static_cast<float>(rand() % 2) == 0 ? -enemySpeed : enemySpeed;
            enemy.speedY = enemySpeed;
            enemy.isEnemyRight = enemy.speedX > 0;
            enemies.push_back(enemy);
        }

        bulletSprites.clear();
        bulletCount = maxBulletCount;

        startButtonPressed = false;
    }

    bool checkCollision(const Sprite& sprite1, const Sprite& sprite2)
    {
        FloatRect bounds1 = sprite1.getGlobalBounds();
        FloatRect bounds2 = sprite2.getGlobalBounds();
        return bounds1.intersects(bounds2);
    }

    void characterLogic(float deltaTime)
    {
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            characterSprite.setTexture(characterTexture);
            characterSprite.setScale(1, 1);
            isCharacterRight = true;
            characterSprite.move(characterSpeed * deltaTime, 0);
        }
        else if (Keyboard::isKeyPressed(Keyboard::A))
        {
            characterSprite.setTexture(characterTexture);
            characterSprite.setScale(-1, 1);
            isCharacterRight = false;
            characterSprite.move(-characterSpeed * deltaTime, 0);
        }
        Vector2f characterPosition = characterSprite.getPosition();
        if (characterPosition.x > Width - 100)
        {
            characterPosition.x = 0;
            characterSprite.setPosition(characterPosition);
        }
        else if (characterPosition.x < 0)
        {
            characterPosition.x = Width - 100;
            characterSprite.setPosition(characterPosition);
        }
        else
        {
            characterSprite.setPosition(characterPosition);
        }
    }

    void bulletLogic(float deltaTime)
    {
        static bool isUpKeyPressed = false;
        static bool isLeftKeyPressed = false;
        static bool isRightKeyPressed = false;

        if (Keyboard::isKeyPressed(Keyboard::Left) && !isLeftKeyPressed && bulletCount > 0)
        {
            isLeftKeyPressed = true;
            Sprite bulletSprite(bulletTexture);
            bulletSprite.setScale(-1, 1);
            bulletSprite.setRotation(-90);
            if (isCharacterRight)
            {
                bulletSprite.setPosition(characterSprite.getPosition().x - 25, characterSprite.getPosition().y + 25);
            }
            else
            {
                bulletSprite.setPosition(characterSprite.getPosition().x - 100, characterSprite.getPosition().y + 25);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Left))
        {
            isLeftKeyPressed = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Right) && !isRightKeyPressed && bulletCount > 0)
        {
            isRightKeyPressed = true;
            Sprite bulletSprite(bulletTexture);
            bulletSprite.setScale(1, 1);
            bulletSprite.setRotation(90);
            if (isCharacterRight)
            {
                bulletSprite.setPosition(characterSprite.getPosition().x + 100, characterSprite.getPosition().y + 25);
            }
            else
            {
                bulletSprite.setPosition(characterSprite.getPosition().x + 30, characterSprite.getPosition().y + 25);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Right))
        {
            isRightKeyPressed = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Up) && !isUpKeyPressed && bulletCount > 0)
        {
            isUpKeyPressed = true;
            Sprite bulletSprite(bulletTexture);
            if (isCharacterRight)
            {
                bulletSprite.setPosition(characterSprite.getPosition().x + 10, characterSprite.getPosition().y - 40);
            }
            else
            {
                bulletSprite.setPosition(characterSprite.getPosition().x - 64, characterSprite.getPosition().y - 40);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Up))
        {
            isUpKeyPressed = false;
        }

        for (auto& bulletSprite : bulletSprites)
        {
            if (bulletSprite.getRotation() == 0)
            {
                bulletSprite.move(0, -bulletSpeed * deltaTime); // движение вверх
            }
            else if (bulletSprite.getRotation() == 90)
            {
                bulletSprite.move(bulletSpeed * deltaTime, 0); // движение вправо
            }
            else
            {
                bulletSprite.move(-bulletSpeed * deltaTime, 0); // движение влево
            }

            if (bulletSprite.getPosition().y < 0 || bulletSprite.getPosition().x > Width || bulletSprite.getPosition().x < 0)
            {
                auto it = std::find_if(bulletSprites.begin(), bulletSprites.end(), [&bulletSprite](const sf::Sprite& bullet)
                    {
                        return bullet.getPosition() == bulletSprite.getPosition();
                    });

                if (it != bulletSprites.end())
                {
                    bulletSprites.erase(it);
                }
                bulletCount++;
            }

            else
            {
                for (auto it = bulletSprites.begin(); it != bulletSprites.end();)
                {
                    bool bulletCollided = false;

                    for (auto& enemy : enemies)
                    {
                        if (!bulletCollided && checkCollision(*it, enemy.sprite))
                        {
                            enemy.x = static_cast<float>(rand() % (Width - 100));
                            enemy.y = -enemy.sprite.getGlobalBounds().height;
                            it = bulletSprites.erase(it);
                            bulletCount++;
                            killCount++;
                            bulletCollided = true;
                            break;
                        }
                    }
                    if (!bulletCollided){ ++it; }    
                }
            }
        }
    }

    Vector2f normalize(const Vector2f& vector)
    {
        float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        if (length != 0)
        {
            return Vector2f(vector.x / length, vector.y / length);
        }
        return Vector2f(0, 0);
    }

    void enemyLogic(float deltaTime)
    {
        for (auto& enemy : enemies)
        {
            Vector2f direction = characterSprite.getPosition() - enemy.sprite.getPosition();
            direction = normalize(direction);

            float deviationAngle = static_cast<float>(rand() % 181 - 90);
            float rotation = atan2(direction.y, direction.x) + deviationAngle / 180.0f;
            direction.x = cos(rotation);
            direction.y = sin(rotation);
            direction = normalize(direction);

            enemy.x += direction.x * enemySpeed * deltaTime;
            enemy.y += direction.y * enemySpeed * deltaTime;

            if (enemy.x < 0)
                enemy.x = 0;
            else if (enemy.x > Width - enemy.sprite.getGlobalBounds().width)
                enemy.x = Width - enemy.sprite.getGlobalBounds().width;

            enemy.sprite.setPosition(enemy.x, enemy.y);

            for (auto& otherEnemy : enemies)
            {
                if (&enemy != &otherEnemy)
                {
                    if (checkCollision(enemy.sprite, otherEnemy.sprite))
                    {
                        Vector2f repulsionDirection = enemy.sprite.getPosition() - otherEnemy.sprite.getPosition();
                        repulsionDirection = normalize(repulsionDirection);

                        enemy.x += repulsionDirection.x * enemySpeed * deltaTime;
                        enemy.y += repulsionDirection.y * enemySpeed * deltaTime;
                    }
                }
            }

            enemy.sprite.setPosition(enemy.x, enemy.y);
            if (checkCollision(enemy.sprite, characterSprite))
            {
                gameOver = true;
            }
        }
    }

    void addEnemy()
    {
        Enemy enemy;
        enemy.sprite.setTexture(enemyTexture);
        enemy.x = static_cast<float>(rand() % (Width - 100));
        enemy.y = -enemy.sprite.getGlobalBounds().height;
        enemy.speedX = static_cast<float>(rand() % 2) == 0 ? -enemySpeed : enemySpeed;
        enemy.speedY = enemySpeed;
        enemy.isEnemyRight = enemy.speedX > 0;
        enemies.push_back(enemy);
    }

    void render()
    {
        window.clear(Color::Black);
        window.draw(backgroundSprite);
        if (gameStarted)
        {
            if (!gameOver)
            {
                window.draw(characterSprite);
                for (auto& bulletSprite : bulletSprites)
                {
                    window.draw(bulletSprite);
                }
                for (auto& enemy : enemies)
                {
                    window.draw(enemy.sprite);
                }

                Text elapsedTimeText;
                elapsedTimeText.setFont(font);

                int hours = static_cast<int>(elapsedTime) / 3600;
                int minutes = static_cast<int>(elapsedTime) / 60 % 60;
                int seconds = static_cast<int>(elapsedTime) % 60;

                string timeString = "Time  " + to_string(hours) + "  h  " + to_string(minutes) + "  m  " + to_string(seconds) + "  s";
                elapsedTimeText.setString(timeString);

                elapsedTimeText.setCharacterSize(36);
                elapsedTimeText.setFillColor(Color::White);
                elapsedTimeText.setOutlineColor(Color::Black);
                elapsedTimeText.setOutlineThickness(5);

                FloatRect textBounds1 = elapsedTimeText.getLocalBounds();
                elapsedTimeText.setOrigin(textBounds1.left + textBounds1.width / 2.0f, textBounds1.top + textBounds1.height / 2.0f);
                elapsedTimeText.setPosition(Width - textBounds1.width / 2.0f - 20.0f, 20.0f);

                window.draw(elapsedTimeText);


                Text killCountText;
                killCountText.setFont(font);
                killCountText.setString("Kills  " + to_string(killCount));
                killCountText.setCharacterSize(36);
                killCountText.setFillColor(Color::White);
                killCountText.setOutlineColor(Color::Black);
                killCountText.setOutlineThickness(5);
                FloatRect textBounds2 = killCountText.getLocalBounds();
                killCountText.setOrigin(textBounds2.left + textBounds2.width / 2.0f, textBounds2.top + textBounds2.height / 2.0f);
                killCountText.setPosition(Width - textBounds2.width / 2.0f - 20.0f, 70.0f);
                window.draw(killCountText);

                Text bulletCountText;
                bulletCountText.setFont(font);
                bulletCountText.setString("Bullets  " + to_string(bulletCount));
                bulletCountText.setCharacterSize(36);
                bulletCountText.setFillColor(Color::White);
                bulletCountText.setOutlineColor(Color::Black);
                bulletCountText.setOutlineThickness(5);
                FloatRect textBounds3 = bulletCountText.getLocalBounds();
                bulletCountText.setOrigin(textBounds3.left + textBounds3.width / 2.0f, textBounds3.top + textBounds3.height / 2.0f);
                bulletCountText.setPosition(Width - textBounds3.width / 2.0f - 20.0f, 120.0f);

                window.draw(bulletCountText);
                window.draw(killCountText);
                window.draw(elapsedTimeText);
            }
            else
            {
                Text restartText;
                restartText.setFont(font);
                restartText.setString("Press any key to restart");
                restartText.setCharacterSize(36);
                restartText.setFillColor(Color::White);
                restartText.setOutlineColor(Color::Black);
                restartText.setOutlineThickness(2);
                FloatRect textBounds = restartText.getLocalBounds();
                restartText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                restartText.setPosition(Width / 2.0f - 27, Height / 2.0f + 175.0f);

                Text EscText;
                EscText.setFont(font);
                EscText.setString("Escape to exit");
                EscText.setCharacterSize(24);
                EscText.setFillColor(Color::White);
                EscText.setOutlineColor(Color::Black);
                EscText.setOutlineThickness(2);
                FloatRect textBounds1 = EscText.getLocalBounds();
                EscText.setOrigin(textBounds1.left + textBounds1.width / 2.0f, textBounds1.top + textBounds1.height / 2.0f);
                EscText.setPosition(Width / 2.0f - 30, Height / 2.0f + 200.0f);

                Text gameOverText;
                gameOverText.setFont(font);
                gameOverText.setString("\tGAME\n\tOVER");
                gameOverText.setCharacterSize(180);
                gameOverText.setFillColor(Color::White);
                gameOverText.setOutlineColor(Color::Black);
                gameOverText.setOutlineThickness(2);
                FloatRect textBounds2 = gameOverText.getLocalBounds();
                gameOverText.setOrigin(textBounds2.left + textBounds2.width / 2.0f, textBounds2.top + textBounds2.height / 2.0f);
                gameOverText.setPosition(Width / 2 - 75, Height / 2 );

                window.draw(gameoverfonSprite);
                window.draw(gameOverText);
                window.draw(restartText);
                window.draw(EscText);
            }
        }
        else
        {
            startGame();
        }
        window.display();
    }

    void update()
    {
        float deltaTime = clock.restart().asSeconds();
        elapsedTime += deltaTime;
        elapsedTimeSinceEnemy += deltaTime;

        if (gameStarted)
        {
            if (!gameOver)
            {
                characterLogic(deltaTime);
                bulletLogic(deltaTime);
                enemyLogic(deltaTime);
            }

            if (elapsedTimeSinceEnemy >= 30.0f)
            {
                addEnemy();
                elapsedTimeSinceEnemy = 0.0f;
            }

            enemySpeed += 1.0f * deltaTime;
            characterSpeed += 1.0f * deltaTime;
            bulletSpeed += 1.0f * deltaTime;
        }
    }

    string getCurrentTime()
    {
        time_t now = time(0);
        double seconds = difftime(now, startTime);

        int hours = static_cast<int>(seconds / 3600);
        seconds -= hours * 3600;
        int minutes = static_cast<int>(seconds / 60);
        seconds -= minutes * 60;
        int secs = static_cast<int>(seconds);

        // Добавляем ведущие нули при необходимости
        string hoursStr = (hours < 10) ? "0" + to_string(hours) : to_string(hours);
        string minutesStr = (minutes < 10) ? "0" + to_string(minutes) : to_string(minutes);
        string secsStr = (secs < 10) ? "0" + to_string(secs) : to_string(secs);

        return hoursStr + " " + minutesStr + " " + secsStr;
    }
};

int main()
{
    Game game;
    game.run();
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    return 0;
}
