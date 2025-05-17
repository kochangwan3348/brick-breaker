#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// 설정 상수
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BRICK_ROWS = 3;
const int BRICK_COLUMNS = 10;
const int BRICK_WIDTH = 60;
const int BRICK_HEIGHT = 20;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const float BALL_RADIUS = 10.f;

// 속도 설정 (픽셀/초)
const float BALL_SPEED = 250.f;
const float PADDLE_SPEED = 400.f;

// 벽돌 생성 함수
std::vector<sf::RectangleShape> createBricks() {
    std::vector<sf::RectangleShape> bricks;
    for (int i = 0; i < BRICK_ROWS; ++i) {
        for (int j = 0; j < BRICK_COLUMNS; ++j) {
            sf::RectangleShape brick(sf::Vector2f(BRICK_WIDTH, BRICK_HEIGHT));
            brick.setFillColor(sf::Color::Green);
            brick.setPosition(j * (BRICK_WIDTH + 10) + 35, i * (BRICK_HEIGHT + 10) + 30);
            bricks.push_back(brick);
        }
    }
    return bricks;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Breakout Game");
    window.setFramerateLimit(60);

    // 패들 및 공 선언
    sf::RectangleShape paddle(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
    sf::CircleShape ball(BALL_RADIUS);
    sf::Vector2f ballVelocity(BALL_SPEED, -BALL_SPEED);

    // 벽돌 벡터
    std::vector<sf::RectangleShape> bricks;

    // 점수 변수
    int score = 0;
    const int maxScore = BRICK_ROWS * BRICK_COLUMNS * 10;

    // 폰트 및 점수 텍스트
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        return -1; // 폰트 로드 실패 시 종료
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, WINDOW_HEIGHT - 35.f);

    // 초기화 함수
    auto resetGame = [&]() {
        ball.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
        ballVelocity = sf::Vector2f(BALL_SPEED, -BALL_SPEED);

        paddle.setFillColor(sf::Color::Blue);
        paddle.setPosition(WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2, WINDOW_HEIGHT - 50);

        bricks = createBricks();
        score = 0;
        };

    resetGame(); // 첫 시작 초기화

    sf::Clock clock;

    // 게임 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        // 패들 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0)
            paddle.move(-PADDLE_SPEED * deltaTime, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x + PADDLE_WIDTH < WINDOW_WIDTH)
            paddle.move(PADDLE_SPEED * deltaTime, 0.f);

        // 공 이동
        ball.move(ballVelocity * deltaTime);

        // 벽 충돌
        if (ball.getPosition().x <= 0 || ball.getPosition().x + 2 * BALL_RADIUS >= WINDOW_WIDTH)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y <= 0)
            ballVelocity.y = -ballVelocity.y;

        // 패들 충돌
        if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
            ballVelocity.y = -ballVelocity.y;

        // 벽돌 충돌
        for (auto it = bricks.begin(); it != bricks.end();) {
            if (ball.getGlobalBounds().intersects(it->getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                it = bricks.erase(it);
                score += 10;
            }
            else {
                ++it;
            }
        }

        // 바닥 충돌 → 게임 전체 초기화
        if (ball.getPosition().y > WINDOW_HEIGHT) {
            resetGame();
        }

        // 점수 텍스트 업데이트
        int percent = static_cast<int>((static_cast<float>(score) / maxScore) * 100);
        scoreText.setString("Progress: " + std::to_string(percent) + "%");

        // 화면 그리기
        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (const auto& brick : bricks)
            window.draw(brick);
        window.draw(scoreText);
        window.display();

        // 벽돌이 모두 없어졌을 경우 → 메시지 표시 후 종료
        if (bricks.empty()) {
            sf::Text winText;
            winText.setFont(font);
            winText.setString("You Win!");
            winText.setCharacterSize(40);
            winText.setFillColor(sf::Color::Yellow);
            winText.setPosition(WINDOW_WIDTH / 2.f - 100.f, WINDOW_HEIGHT / 2.f - 30.f);

            window.clear(sf::Color::Black);
            window.draw(winText);
            window.display();

            sf::sleep(sf::seconds(2));
            window.close();
        }
    }

    return 0;
}
