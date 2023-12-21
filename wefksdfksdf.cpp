#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>

const int MAP_LEFT_BORDER = 100;
const int MAP_TOP_BORDER = 100;
const int MAP_RIGHT_BORDER = 1000;
const int MAP_BOTTOM_BORDER = 700;
const int MAX_CARS = 8;
class GameObject {
protected:
    sf::RectangleShape shape;
public:
    GameObject(float startX, float startY, float objWidth, float objHeight)
        : shape(sf::Vector2f(objWidth, objHeight)) {
        shape.setPosition(startX, startY);
    }

    float GetX() const { return shape.getPosition().x; }
    float GetY() const { return shape.getPosition().y; }
    float GetWidth() const { return shape.getSize().x; }
    float GetHeight() const { return shape.getSize().y; }

    void SetX(float value) { shape.setPosition(value, GetY()); }
    void SetY(float value) { shape.setPosition(GetX(), value); }
    void SetWidth(float value) { shape.setSize(sf::Vector2f(value, GetHeight())); }
    void SetHeight(float value) { shape.setSize(sf::Vector2f(GetWidth(), value)); }

    virtual void Draw(sf::RenderWindow& window) const = 0;
};

class Car : public GameObject {
private:
    float angle;
    sf::Color color;
    bool isParking = false;

public:
    Car() :GameObject(0, 0, 100, 50), angle(0), color(sf::Color::Green) {
        shape.setFillColor(color);
    };
    Car(float startX, float startY)
        :GameObject(startX, startY, 100, 50), angle(0), color(sf::Color::Green) {
        shape.setFillColor(color);
    }

    void SetisParking(bool Parking) { isParking = Parking; }
    bool GetisParking() { return isParking; }

    bool IsCollision(const GameObject& other) const {
        return shape.getGlobalBounds().intersects(other.shape.getGlobalBounds());
    }

    void Move(float newX, float newY, float newAngle) {
        shape.setPosition(newX, newY);
        angle = newAngle;
    }

    void SetColor(sf::Color newColor) { color = newColor; shape.setFillColor(color); }

    void Draw(sf::RenderWindow& window) const override {
        window.draw(shape);
    }
};

class ParkingArea : public GameObject {
private:
    const float PARKING_AREA_X = 400.f;
    const float PARKING_AREA_Y = 0.f;
    const float PARKING_AREA_WIDTH = 800.f;
    const float PARKING_AREA_HEIGHT = 800.f;

    const int MaxParkingSpace = 9;

    float ParkingLineStartXLeft = 400.f;
    float ParkingLineEndXLeft = 500.f;

    float ParkingLineStartXRight = 700.f;
    float ParkingLineEndXRight = 800.f;

    mutable float ParkingLineStartY[11] = { 1.f };
    mutable float ParkingLineEndY[11] = { 6.f };

    void ParkingLine() const {
        for (int i = 1; i < MaxParkingSpace; ++i) {
            ParkingLineStartY[i] =
                PARKING_LINE_INTERVAL + ParkingLineStartY[i - 1];
            ParkingLineEndY[i] = PARKING_LINE_INTERVAL + ParkingLineEndY[i - 1];
        }
    }

protected:
    const float PARKING_LINE_INTERVAL = 75.f;

    void MidParking(float xParLeft[9], float xParRight[9],
        float yPar[11]) const {
        for (int i = 0; i < 8; i++) {
            xParLeft[i] = (ParkingLineStartXLeft + ParkingLineEndXLeft) * 0.5f;
            yPar[i] = (ParkingLineStartY[i] + ParkingLineEndY[i + 1]) * 0.5f;
            xParRight[i] = (ParkingLineStartXRight + ParkingLineEndXRight) * 0.5f;
        }
    }

public:
    ParkingArea() :GameObject(PARKING_AREA_X, PARKING_AREA_Y, PARKING_AREA_WIDTH,
        PARKING_AREA_HEIGHT) {
        ParkingLine();
    }

    void Draw(sf::RenderWindow& window) const override {
        sf::RectangleShape parkingArea(sf::Vector2f(PARKING_AREA_WIDTH, PARKING_AREA_HEIGHT));
        parkingArea.setPosition(PARKING_AREA_X, PARKING_AREA_Y);
        parkingArea.setFillColor(sf::Color(105, 105, 105));
        window.draw(parkingArea);

        sf::RectangleShape parkingLine(sf::Vector2f(ParkingLineEndXLeft - ParkingLineStartXLeft, ParkingLineEndY[0] - ParkingLineStartY[0]));
        parkingLine.setFillColor(sf::Color::White);

        for (int i = 0; i < MaxParkingSpace; i++) {
            parkingLine.setPosition(ParkingLineStartXLeft, ParkingLineStartY[i]);
            window.draw(parkingLine);
            parkingLine.setPosition(ParkingLineStartXRight, ParkingLineStartY[i]);
            window.draw(parkingLine);
        }
    }
};

class Road : public GameObject {
public:
    Road() : GameObject(10.f, 0.f, 200.f, 1500.f) {}
    Road(float startX, float startY, float roadWidth, float roadHeight)
        : GameObject(startX, startY, roadWidth, roadHeight) {}

    void Draw(sf::RenderWindow& window) const override {
        sf::RectangleShape road(sf::Vector2f(width, height));
        road.setPosition(x, y);
        road.setFillColor(sf::Color(105, 105, 105));
        window.draw(road);

        sf::RectangleShape parkingRoad(sf::Vector2f(391.f - 199.f, 800.f - 600.f));
        parkingRoad.setPosition(x + 199.f, y + 600.f);
        parkingRoad.setFillColor(sf::Color(105, 105, 105));
        window.draw(parkingRoad);

        sf::RectangleShape marking(sf::Vector2f(20.f, 40.f));
        marking.setFillColor(sf::Color::White);

        for (int i = 0; i < 19; i++) {
            marking.setPosition(x + 90.f, y + i * 80.f);
            window.draw(marking);
        }

        sf::RectangleShape parkingMarking(sf::Vector2f(40.f, 20.f));
        parkingMarking.setFillColor(sf::Color::White);

        for (int i = 0; i < 3; ++i) {
            parkingMarking.setPosition(x + 209.f + i * 60.f, y + 690.f);
            window.draw(parkingMarking);
        }
    }
};

class Obstacle : public GameObject {
public:
    Obstacle() :GameObject(0.f, 0.f, 0.f, 0.f) {
        shape.setFillColor(sf::Color::Black);
    };
    Obstacle(float startX, float startY, float obstacleWidth, float obstacleHeight)
        : GameObject(startX, startY, obstacleWidth, obstacleHeight) {
        shape.setFillColor(sf::Color::Black);
    }

    void Draw(sf::RenderWindow& window) const override {
        window.draw(shape);
    }

    void Reset() {
        shape.setPosition(-1.f, -1.f);
    }

    bool IsCollision(const GameObject& other) const {
        return shape.getGlobalBounds().intersects(other.shape.getGlobalBounds());
    }
};

class House : public GameObject {
private:
    sf::Color color;

public:
    House(float StartX, float StartY, float StartWidth, float StartHeight, sf::Color houseColor)
        :GameObject(StartX, StartY, StartWidth, StartHeight), color(houseColor) {
        shape.setFillColor(color);
    }

    void Draw(sf::RenderWindow& window) const override {
        window.draw(shape);
    }
};


class Yard : protected ParkingArea {
private:
    sf::RenderWindow window;  // Изменение типа окна на sf::RenderWindow
    ParkingArea parkingArea;  // Создаем объект парковочной области
    Road road;                  // Создаем объект дороги
    Obstacle obstacle; // Добавляем препятствие
    std::vector<House> houses;  // Вектор для хранения домов
    Car userCar;
    bool isParking = false;  // Стоит ли машина на парковочном месте
    double userCarAngle = 0.0;  // Добавленная переменная для хранения угла
    // поворота машины пользователя

    double targetUserCarAngle =
        0.0;  // Добавленная переменная для хранения целевого угла
    double userCarRotationSpeed = 0.1;  // Скорость поворота машины
    double maxRotationAngle = 0.1;  // Максимальный угол поворота за один кадр

    // Координаты домов
    int startX[5] = { 220, 220, 810, 810, 810 };
    int startWidth[5] = { 170, 170, 250, 250, 400 };
    int startY[5] = { 30, 200, 290, 500, 9 };
    int startHeight[5] = { 150, 350, 200, 300, 200 };

public:
    Yard() {
        srand(static_cast<unsigned>(time(nullptr)));  // Сбиваем значения

        //Генерируем машины
        GenerateRandomCars(MAX_CARS);

        //Добавляем дома
        for (int i = 0; i < 5; i++) {
            int randColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            if (i < 4) {
                houses.push_back(House(startX[i], startY[i], startWidth[i], startHeight[i], randColor));
            }
            else {
                randColor = RGB(255, 255, 255);
                houses.push_back(House(startX[i], startY[i], startWidth[i], startHeight[i], randColor));
            }
        }
        //Генерируем возможное препядствие
        GenerateRandomObstacle();
    }

    // Метод для генерации машин на случайных местах
    void GenerateRandomCars(int numCars) {
        // Задаем константы для настроек генерации машин
        const int MaxCars = 8;
        const int MaxParkingLines = 16;
        const int ParkingLineWidth = 50;
        const int ParkingLineHeight = 25;

        // Массивы для хранения координат машин при парковки
        double xParLeft[MaxCars] = {};
        double xParRight[MaxCars] = {};
        double yPar[MaxCars] = {};

        // Массивы для отслеживания использования координат при парковке под углом
        bool usedxParLeft[MaxCars] = { false };
        bool usedxParRight[MaxCars] = { false };
        bool usedyParLeft[MaxCars] = { false };
        bool usedyParRight[MaxCars] = { false };
        bool parked[MaxParkingLines] = { false };
        // Получение координат середины паркоыочных мест
        MidParking(xParLeft, xParRight, yPar);

        int i = 0;
        //Цикл для генерации машин
        while (i < numCars) {
            int index = rand() % MaxParkingLines - 1;
            int randColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            if (!parked[index]) {
                {
                    int xrand = rand() % MaxCars;
                    int yrand = rand() % MaxCars;
                    double xplace = (index % 2 == 0) ? xParLeft[xrand] : xParRight[xrand];
                    double yplace = yPar[yrand];
                    if (index % 2 == 0) {
                        if (!usedxParLeft[xrand] && !usedyParLeft[yrand]) {
                            cars.push_back(
                                Car(xplace - ParkingLineWidth, yplace - ParkingLineHeight));
                            if (!cars[i].IsCollision(obstacle)) {
                                cars[i].SetisParking(true);
                                cars[i].SetColor(randColor);
                                parked[index] = true;
                                usedxParLeft[xrand] = true;
                                usedyParLeft[yrand] = true;
                                i++;
                            }
                            else {
                                cars.pop_back();
                            }
                        }
                    }
                    else if (!usedxParRight[xrand] && !usedyParRight[yrand]) {
                        cars.push_back(
                            Car(xplace - ParkingLineWidth, yplace - ParkingLineHeight));
                        if (!cars[i].IsCollision(obstacle)) {
                            cars[i].SetisParking(true);
                            cars[i].SetColor(randColor);
                            parked[index] = true;
                            usedxParRight[xrand] = true;
                            usedyParRight[yrand] = true;
                            i++;
                        }
                        else {
                            cars.pop_back();
                        }
                    }
                }
            }
        }
    }

    // Метод для генерации случайного препятствия
    void GenerateRandomObstacle() {
        if (rand() % 100 < 99) { // Настройка шанса генерации препядствия
            obstacle.Reset();
            int x = 200 + rand() % 600;
            int y = rand() % 600;
            int width = 30; // Ширина препятствия
            int height = 30; // Высота препятствия
            obstacle.SetX(x);
            obstacle.SetY(y);
            obstacle.SetWidth(width);
            obstacle.SetHeight(height);
            // Проверка на столкновение с машинами
            for (const auto& car : cars) {
                if (obstacle.IsCollision(car)) {
                    GenerateRandomObstacle();
                    return;
                }
            }
            // Проверка столкновения с домами
            for (const auto& house : houses) {
                if (obstacle.IsCollision(house)) {
                    GenerateRandomObstacle();
                    return;
                }
            }
        }
    }

    // Метод для проверки столкновений с объектами
    bool CheckCollision(const Car& car) const {
        // Проверяем коллизию с каждой из машин во дворе
        for (const auto& otherCar : cars) {
            if (&car != &otherCar) {  // Исключаем проверку с самой собой
                if (car.GetX() < otherCar.GetX() + otherCar.GetWidth() &&
                    car.GetX() + car.GetWidth() > otherCar.GetX() &&
                    car.GetY() < otherCar.GetY() + otherCar.GetHeight() &&
                    car.GetY() + car.GetHeight() > otherCar.GetY()) {
                    return true;  // Коллизия обнаружена
                }
            }
        }

        // Проверяем коллизию с домами
        for (const auto& house : houses) {
            if (car.GetX() < house.GetX() + house.GetWidth() &&
                car.GetX() + car.GetWidth() > house.GetX() &&
                car.GetY() < house.GetY() + house.GetHeight() &&
                car.GetY() + car.GetHeight() > house.GetY()) {
                return true;  // Коллизия с домом
            }
        }

        // Проверяем коллизию с препядствием
        if (obstacle.IsCollision(car)) {
            return true;
        }

        return false;  // Нет коллизии
    }

    // Проверка на правильность парковки
    bool CheckCorrectParking(int number) const {
        // Получаем координаты машины
        double CarX = cars[number].GetX();
        double CarY = cars[number].GetY();
        double CarWidth = cars[number].GetWidth();
        double CarHeight = cars[number].GetHeight();

        // Получаем координаты и размеры парковочных областей
        double xParLeft[10], xParRight[10], yPar[10];
        MidParking(xParLeft, xParRight, yPar);

        // Проверяем, что машина находится внутри пределов парковочной области
        bool isInParkingArea = false;

        // Проверка припаркована ли машина
        for (int i = 0; i < 9; ++i) {
            if (CarY >= yPar[i] - 25 &&
                CarY + CarHeight <= yPar[i + 1] + PARKING_LINE_INTERVAL) {
                if ((CarX >= xParLeft[i] - 50 &&
                    CarX + CarWidth <=
                    xParLeft[i + 1] + PARKING_LINE_INTERVAL) ||
                    (CarX >= xParRight[i] - 50 &&
                        CarX + CarWidth <=
                        xParRight[i + 1] + PARKING_LINE_INTERVAL)) {
                    isInParkingArea = true;
                    break;
                }
            }
        }
        for (const auto& car : cars) {
            if (isInParkingArea && !CheckCollision(car)) {
                return true;
            }
            return false;
        }
    }

    // Функция линейной интерполяции
    double Lerp(double a, double b, double t) {
        return a + t * (b - a);
    }

    void Draw() {
        window.clear();  // Очистка окна перед отрисовкой
        parkingArea.Draw(window);  // Отображение парковочной площадки
        road.Draw(window);  // Отображение дорог
        obstacle.Draw(window);
        // Отображение домов
        for (const auto& house : houses) {
            house.Draw(window);
        }

        // Отображение машин
        for (const auto& car : cars) {
            car.Draw(window);
        }
        userCar.Draw(window);
        window.display();  // Отображение отрисованного содержимого
    }

    void ToggleFullscreen() {
        // Реализация переключения в полноэкранный режим
        // ...
    }

    sf::Color LerpColor(sf::Color color1, sf::Color color2, double t) {
        int r1 = color1.r;
        int g1 = color1.g;
        int b1 = color1.b;

        int r2 = color2.r;
        int g2 = color2.g;
        int b2 = color2.b;

        int r = static_cast<int>(Lerp(r1, r2, t));
        int g = static_cast<int>(Lerp(g1, g2, t));
        int b = static_cast<int>(Lerp(b1, b2, t));

        return sf::Color(r, g, b);
    }
};

int main() {
    // Создание окна SFML
    sf::RenderWindow window(sf::VideoMode(MAP_RIGHT_BORDER - MAP_LEFT_BORDER, MAP_BOTTOM_BORDER - MAP_TOP_BORDER), "Parking");
    window.setPosition(sf::Vector2i(MAP_LEFT_BORDER, MAP_TOP_BORDER));

    // Создание объекта двора
    Yard yard;

    // Создание таймера
    sf::Clock clock;

    // Главный цикл обработки событий и отрисовки
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                yard.ToggleFullscreen(window);
            }
        }

        // Обновление состояния двора
        float deltaTime = clock.restart().asSeconds();
        yard.Update(deltaTime);

        // Отрисовка
        window.clear();
        yard.Draw(window);
        window.display();
    }

    return 0;
}
