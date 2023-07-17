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

const int enemyCount = 10;
const int maxBulletCount = 10;

const float EnemySpeed = 200.0f;
const float BulletSpeed = 1000.0f;
const float CharacterSpeed = 1000.0f;

class Game {
private:
    RenderWindow window;
    vector<Sprite> bulletSprites;
    bool isCharacterRight;
    int bulletCount;
    int killCount = 0;
    bool gameOver;
    Clock clock;

    Sprite backgroundSprite;
    Sprite gameoverfonSprite;
    Sprite characterSprite;

    Texture backgroundTexture;
    Texture bulletLeftTexture;
    Texture bulletRightTexture;
    Texture bulletUpTexture;
    Texture characterTexture;
    Texture enemyTexture;
    Texture gameoverfonTexture;

    Music music;
    Font font;
    time_t startTime;
    float elapsedTime;

    struct Enemy {
        Sprite sprite;
        float x;
        float y;
        float speedX;
        float speedY;
        bool isEnemyRight;
    };

    vector<Enemy> enemies;

public:
    Game() : window(VideoMode(Width, Height), "Game", Style::Fullscreen), isCharacterRight(true), bulletCount(maxBulletCount), gameOver(false), elapsedTime(0.0f) {}

    void run() {
        initialize();
        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }

private:
    void initialize() {
        srand(static_cast<unsigned int>(time(NULL)));
        window.setFramerateLimit(240);
        window.setVerticalSyncEnabled(true);

        if (!music.openFromFile("Gamefiles/Sound/soundtrack.mp3")) {
            cout << "Failed to load music" << endl;
        }

        if (!font.loadFromFile("Gamefiles/Font/font.ttf")) {
            cout << "Failed to load font" << endl;
        }

        music.setVolume(10);
        music.setLoop(true);
        music.play();

        backgroundTexture = textureLoad("GameFiles/Images/fon.png");
        bulletLeftTexture = textureLoad("GameFiles/Images/bullet_left.png");
        bulletRightTexture = textureLoad("GameFiles/Images/bullet_right.png");
        bulletUpTexture = textureLoad("GameFiles/Images/bullet_up.png");
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

        for (int i = 0; i < enemyCount; i++) {
            Enemy enemy;
            enemy.sprite.setTexture(enemyTexture);
            enemy.x = static_cast<float>(rand() % Width);
            enemy.y = 0 + rand() % 200;
            enemy.speedX = static_cast<float>(rand() % 2) == 0 ? -EnemySpeed : EnemySpeed;
            enemy.speedY = EnemySpeed;
            enemy.isEnemyRight = enemy.speedX > 0;
            enemies.push_back(enemy);
        }

        startTime = time(0);
    }

    Texture textureLoad(const string& filePath) {
        Texture texture;
        if (!texture.loadFromFile(filePath)) {
            cout << "Failed to load texture: " << filePath << endl;
        }
        return texture;
    }

    void processEvents() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
            }
            else if (gameOver) {
                if ((event.type == Event::KeyPressed) && !(Keyboard::isKeyPressed(Keyboard::Escape))) {
                    restartGame();
                }
                else if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    window.close();
                }
            }
        }
    }

    void restartGame() {
        gameOver = false;
        killCount = 0;
        startTime = time(0);
        for (auto& enemy : enemies) {
            enemy.sprite.setTexture(enemyTexture);
            enemy.x = static_cast<float>(rand() % Width);
            enemy.y = 0 + rand() % 200;
            enemy.speedX = static_cast<float>(rand() % 2) == 0 ? -EnemySpeed : EnemySpeed;
            enemy.speedY = EnemySpeed;
            enemy.isEnemyRight = enemy.speedX > 0;
        }
        for (auto& bulletSprite : bulletSprites) {
            bulletSprite.setPosition(-100, -100);
        }
        bulletCount = maxBulletCount;
    }

    bool checkCollision(const Sprite& sprite1, const Sprite& sprite2) {
        FloatRect bounds1 = sprite1.getGlobalBounds();
        FloatRect bounds2 = sprite2.getGlobalBounds();
        return bounds1.intersects(bounds2);
    }

    void characterLogic(float deltaTime) {
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            characterSprite.setTexture(characterTexture);
            characterSprite.setScale(1, 1);
            isCharacterRight = true;
            characterSprite.move(CharacterSpeed * deltaTime, 0);
        }
        else if (Keyboard::isKeyPressed(Keyboard::A)) {
            characterSprite.setTexture(characterTexture);
            characterSprite.setScale(-1, 1);
            isCharacterRight = false;
            characterSprite.move(-CharacterSpeed * deltaTime, 0);
        }
        Vector2f characterPosition = characterSprite.getPosition();
        if (characterPosition.x > Width - 100) {
            characterPosition.x = 0;
            characterSprite.setPosition(characterPosition);
        }
        else if (characterPosition.x < 0) {
            characterPosition.x = Width - 100;
            characterSprite.setPosition(characterPosition);
        }
        else {
            characterSprite.setPosition(characterPosition);
        }
    }

    void bulletLogic(float deltaTime) {
        static bool isUpKeyPressed = false;
        static bool isLeftKeyPressed = false;
        static bool isRightKeyPressed = false;

        if (Keyboard::isKeyPressed(Keyboard::Left) && !isLeftKeyPressed && bulletCount > 0) {
            isLeftKeyPressed = true;
            Sprite bulletSprite(bulletLeftTexture);
            if (isCharacterRight) {
                bulletSprite.setPosition(characterSprite.getPosition().x - 25, characterSprite.getPosition().y + 25);
            }
            else {
                bulletSprite.setPosition(characterSprite.getPosition().x - 100, characterSprite.getPosition().y + 25);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Left)) {
            isLeftKeyPressed = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Right) && !isRightKeyPressed && bulletCount > 0) {
            isRightKeyPressed = true;
            Sprite bulletSprite(bulletRightTexture);
            if (isCharacterRight) {
                bulletSprite.setPosition(characterSprite.getPosition().x + 35, characterSprite.getPosition().y + 25);
            }
            else {
                bulletSprite.setPosition(characterSprite.getPosition().x - 35, characterSprite.getPosition().y + 25);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Right)) {
            isRightKeyPressed = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Up) && !isUpKeyPressed && bulletCount > 0) {
            isUpKeyPressed = true;
            Sprite bulletSprite(bulletUpTexture);
            if (isCharacterRight) {
                bulletSprite.setPosition(characterSprite.getPosition().x + 10, characterSprite.getPosition().y - 40);
            }
            else {
                bulletSprite.setPosition(characterSprite.getPosition().x - 64, characterSprite.getPosition().y - 40);
            }
            bulletSprites.push_back(bulletSprite);
            bulletCount--;
        }
        else if (!Keyboard::isKeyPressed(Keyboard::Up)) {
            isUpKeyPressed = false;
        }

        for (auto& bulletSprite : bulletSprites) {
            if (bulletSprite.getTexture() == &bulletUpTexture) {
                bulletSprite.move(0, -BulletSpeed * deltaTime);
            }
            else if (bulletSprite.getTexture() == &bulletLeftTexture) {
                bulletSprite.move(-BulletSpeed * deltaTime, 0);
            }
            else if (bulletSprite.getTexture() == &bulletRightTexture) {
                bulletSprite.move(BulletSpeed * deltaTime, 0);
            }

            if (bulletSprite.getPosition().y < 0 || bulletSprite.getPosition().x > Width || bulletSprite.getPosition().x < 0) {
                bulletSprite.setPosition(-100, -100);
                bulletCount++;
            }
            else {
                for (auto& enemy : enemies) {
                    if (checkCollision(bulletSprite, enemy.sprite)) {
                        enemy.x = static_cast<float>(rand() % (Width - 100));
                        enemy.y = 50;
                        bulletSprite.setPosition(-100, -100);
                        bulletCount++;
                        killCount++;
                        break;
                    }
                }
            }
        }
    }

    Vector2f normalize(const Vector2f& vector) {
        float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        if (length != 0) {
            return Vector2f(vector.x / length, vector.y / length);
        }
        return Vector2f(0, 0);
    }

    void enemyLogic(float deltaTime) {
        for (auto& enemy : enemies) {
            Vector2f direction = characterSprite.getPosition() - enemy.sprite.getPosition();
            direction = normalize(direction);

            float deviationAngle = static_cast<float>(rand() % 181 - 90);
            float rotation = atan2(direction.y, direction.x) + deviationAngle * 3.14159265f / 180.0f;
            direction.x = cos(rotation);
            direction.y = sin(rotation);
            direction = normalize(direction);

            enemy.x += direction.x * EnemySpeed * deltaTime;
            enemy.y += direction.y * EnemySpeed * deltaTime;

            if (enemy.x < 0)
                enemy.x = 0;
            else if (enemy.x > Width - enemy.sprite.getGlobalBounds().width)
                enemy.x = Width - enemy.sprite.getGlobalBounds().width;

            if (enemy.y < 0)
                enemy.y = 0;
            else if (enemy.y > Height - enemy.sprite.getGlobalBounds().height)
                enemy.y = Height - enemy.sprite.getGlobalBounds().height;

            enemy.sprite.setPosition(enemy.x, enemy.y);

            // Дополнительная логика для отталкивания от других врагов
            for (auto& otherEnemy : enemies) {
                if (&enemy != &otherEnemy) {
                    if (checkCollision(enemy.sprite, otherEnemy.sprite)) {
                        Vector2f repulsionDirection = enemy.sprite.getPosition() - otherEnemy.sprite.getPosition();
                        repulsionDirection = normalize(repulsionDirection);

                        enemy.x += repulsionDirection.x * EnemySpeed * deltaTime;
                        enemy.y += repulsionDirection.y * EnemySpeed * deltaTime;
                    }
                }
            }

            enemy.sprite.setPosition(enemy.x, enemy.y);

            if (enemy.speedX > 0) {
                enemy.sprite.setScale(1, 1);
                enemy.isEnemyRight = true;
            }
            else {
                enemy.sprite.setScale(-1, 1);
                enemy.isEnemyRight = false;
            }

            if (checkCollision(enemy.sprite, characterSprite)) {
                gameOver = true;
            }
        }
    }



    void render() {
        window.clear(Color::Black);
        window.draw(backgroundSprite);
        if (!gameOver) {
            window.draw(characterSprite);
            for (auto& bulletSprite : bulletSprites) {
                window.draw(bulletSprite);
            }
            for (auto& enemy : enemies) {
                window.draw(enemy.sprite);
            }
            Text killCountText;
            killCountText.setFont(font);
            killCountText.setString("Kills\n\t\t" + to_string(killCount));
            killCountText.setCharacterSize(36);
            killCountText.setFillColor(Color::Black);
            killCountText.setOutlineColor(Color::White);
            killCountText.setOutlineThickness(2);
            killCountText.setPosition(Width - 115, 5);

            Text timeText;
            timeText.setFont(font);
            timeText.setString("\tTime\n" + getCurrentTime());
            timeText.setCharacterSize(36);
            timeText.setFillColor(Color::Black);
            timeText.setOutlineColor(Color::White);
            timeText.setOutlineThickness(2);
            timeText.setPosition(Width - 270, 5);

            window.draw(killCountText);
            window.draw(timeText);
        }
        else {
            Text restartText;
            restartText.setFont(font);
            restartText.setString("Press  any  key  to  restart");
            restartText.setCharacterSize(36);
            restartText.setFillColor(Color::White);
            restartText.setOutlineColor(Color::Black);
            restartText.setOutlineThickness(2);
            FloatRect textBounds = restartText.getLocalBounds();
            restartText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            restartText.setPosition(Width / 2.0f, Height / 2.0f + 125.0f);

            Text EscText;
            EscText.setFont(font);
            EscText.setString("Escape  to  exit");
            EscText.setCharacterSize(24);
            EscText.setFillColor(Color::White);
            EscText.setOutlineColor(Color::Black);
            EscText.setOutlineThickness(2);
            FloatRect textBounds1 = EscText.getLocalBounds();
            EscText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            EscText.setPosition(Width / 2.0f + 125, Height / 2.0f + 170.0f);

            Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("\tGAME\n\tOVER");
            gameOverText.setCharacterSize(180);
            gameOverText.setFillColor(Color::White);
            gameOverText.setOutlineColor(Color::Black);
            gameOverText.setOutlineThickness(2);
            FloatRect textBounds2 = gameOverText.getLocalBounds();
            gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            gameOverText.setPosition(Width / 2 - 75, Height / 2 - 500 / 2);

            window.draw(gameoverfonSprite);
            window.draw(gameOverText);
            window.draw(restartText);
            window.draw(EscText);
        }
        window.display();
    }

    void update() {
        float deltaTime = clock.restart().asSeconds();
        elapsedTime += deltaTime;
        if (!gameOver) {
            characterLogic(deltaTime);
            bulletLogic(deltaTime);
            enemyLogic(deltaTime);
        }
    }

    string getCurrentTime() {
        time_t now = time(0);
        double seconds = difftime(now, startTime);
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        int secs = static_cast<int>(seconds - hours * 3600 - minutes * 60);

        char buffer[80];
        sprintf_s(buffer, sizeof(buffer), "%02d %02d %02d", hours, minutes, secs);

        string timeString = buffer;
        return timeString;
    }
};

int main()
{
    Game game;
    game.run();
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    return 0;
}
