#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <ctime>
#include <limits>
#include <random>
#include <vector>
#include <CommCtrl.h>
#define ID_TRACKBAR 100

// Константы для параметров мира
const int MAX_CARS = 10;
const int PARKING_SPACE_WIDTH = 100;
const int PARKING_SPACE_HEIGHT = 50;
const int PARKING_AREA_X = 400;
const int PARKING_AREA_Y = 0;
const int PARKING_AREA_WIDTH = 800;
const int PARKING_AREA_HEIGHT = 800;
const int PARKING_LINE_INTERVAL = 75;
const int MAP_LEFT_BORDER = 0;
const int MAP_RIGHT_BORDER = 1280;
const int MAP_TOP_BORDER = 0;
const int MAP_BOTTOM_BORDER = 800;

const double PI = 3.141592653589793238463;

// Объявление переменных перед WinMain
bool keyWPressed = false;
bool keySPressed = false;
bool keyAPressed = false;
bool keyDPressed = false;

// Определение полного экрана
bool fullscreen = false;

// Прототипы функций работы с окном(для коректной работы)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HBRUSH hBackgroundBrush;  // Цвет заднего фона

// Базовый класс для игровых объектов
class GameObject {
protected:
    // Данные игровых объектов
    int x, y, width, height;

public:
    // Конструктор по умолчанию
    GameObject(int startX, int startY, int objWidth, int objHeight)
        : x(startX), y(startY), width(objWidth), height(objHeight) {}

    // Метод для получения координаты x
    int GetX() const { return x; }

    //Метод для получение координаты y
    int GetY() const { return y; }

    // Метод для получаения ширины
    int GetWidth() const { return width; }

    //Метод для получения высоты
    int GetHeight() const { return height; }

    void SetX(int value) { x = value; }

    void SetY(int value) { y = value; }

    void SetWidth(int value) { width = value; }

    void SetHeight(int value) { height = value; }

    // Виртуальная функция для отрисовки объекта
    virtual void Draw(HDC hdc) const = 0;
};

// Класс машины, наследующийся от игрового объекта
class Car : public GameObject {
private:
    double angle;  // Угол поворота машины
    COLORREF color;  // Добавляем переменную для хранения цвета
    double speed = 0.0;      // Текущая скорость машины
    double direction = 0.0;  // Направление движения машины
    const double MaxSpeed = 10;
    HDC hdc;
    bool isParking = false;
    bool isParked = false;  // Добавляем переменную для определения, припаркована машина или нет
    double targetParkingX;
    double targetParkingY;
public:
    // Конструкторы машины
    Car()
        : GameObject(0, 0, PARKING_SPACE_WIDTH, PARKING_SPACE_HEIGHT),
        angle(0),
        color(RGB(0, 255, 0)),
        speed(0),
        direction(0) {}

    Car(int startX, int startY)
        : GameObject(startX, startY, PARKING_SPACE_WIDTH, PARKING_SPACE_HEIGHT),
        angle(0),
        color(RGB(0, 255, 0)),
        speed(0),
        direction(0) {}
    void Park() {
        isParked = true;
    }
    void SetTargetParkingPosition(double x, double y) {
        targetParkingX = x;
        targetParkingY = y;
    }
    // Метод для перемещения машины
    void Move() {
        // Если машина не припаркована, двигаемся к целевой парковочной позиции
        if (!isParked) {
            // Рассчитываем направление к целевой парковочной позиции
            double direction = atan2(targetParkingY - y, targetParkingX - x);

            // Рассчитываем новые координаты
            x += cos(direction) * speed;
            y += sin(direction) * speed;

            // Если машина достигла целевой парковочной позиции, паркуем машину
            if (abs(x - targetParkingX) < speed && abs(y - targetParkingY) < speed) {
                Park();
            }
        }
    }

    // Метод для установки цвета машины
    void SetColor(COLORREF newColor) { color = newColor; }

    // Новый метод для установки скорости и направления движения
    void SetMovement(double newSpeed, double newDirection) {
        if (newSpeed > MaxSpeed) {
            speed = newSpeed * 0.5;
        }
        else {
            speed = newSpeed;
        }

        direction = newDirection;
    }

    // Метод для установки скорости и направления движения
    void RotateInMotion(double rotationSpeed) {
        if (speed != 0) {
            angle += rotationSpeed * direction;
        }
    }

    // Метод для отрисовки машины
    void Draw(HDC hdc) const override {
        HBRUSH BlueBrush =
            CreateSolidBrush(RGB(0, 0, 255));  // Создаем кисть с цветом для окон
        HBRUSH carBrush = CreateSolidBrush(color);  // Создаем кисть с цветом машины
        SelectObject(hdc, carBrush);



        // Получаем координаты центра машины
        int centerX = x + width / 2;
        int centerY = y + height / 2;

        // Поворачиваем координатные оси для отрисовки повернутой машины
        SetGraphicsMode(hdc, GM_ADVANCED);
        XFORM xForm;
        xForm.eM11 = static_cast<FLOAT>(cos(angle));
        xForm.eM12 = static_cast<FLOAT>(sin(angle));
        xForm.eM21 = static_cast<FLOAT>(-sin(angle));
        xForm.eM22 = static_cast<FLOAT>(cos(angle));
        xForm.eDx = static_cast<FLOAT>(centerX);
        xForm.eDy = static_cast<FLOAT>(centerY);
        SetWorldTransform(hdc, &xForm);

        // Отрисовка прямоугольника, представляющего машину
        Rectangle(hdc, x - centerX, y - centerY, x - centerX + width,
            y - centerY + height);

        // Отображаем окна
        SelectObject(hdc, BlueBrush);
        Rectangle(hdc, x + 60 - centerX, y + 10 - centerY, x + 70 - centerX,
            y + 40 - centerY);

        // Сбрасываем трансформацию
        SetGraphicsMode(hdc, GM_COMPATIBLE);
        ModifyWorldTransform(hdc, nullptr, MWT_IDENTITY);

        // Освобождаем ресурсы кисти
        DeleteObject(carBrush);
        DeleteObject(BlueBrush);
    }
    void update() {
        x += speed * cos(direction * PI / 180.0);
        y += speed * sin(direction * PI / 180.0);
    }
    bool GetisParking()
    {
        return isParking;
    }
    void SetisParking(bool parking)
    {
        isParking = parking;
    }
    void LeaveParking() {
        update();
    }
    void EnterParking() {
        update();
    }
};

// Класс парковочной области, наследующийся от игрового объекта
class ParkingArea : public GameObject {
public:
    // Конструктор парковочной области
    ParkingArea()
        : GameObject(PARKING_AREA_X, PARKING_AREA_Y, PARKING_AREA_WIDTH,
            PARKING_AREA_HEIGHT) {}

    // Метод для отрисовки парковочной области
    void Draw(HDC hdc) const override {
        HBRUSH GrayBrush = CreateSolidBrush(RGB(105, 105, 105));
        SelectObject(hdc, GrayBrush);

        // Отрисовка прямоугольника парковочной области
        Rectangle(hdc, PARKING_AREA_X, PARKING_AREA_Y, PARKING_AREA_WIDTH,
            PARKING_AREA_HEIGHT);
        DeleteObject(GrayBrush);

        // Отрисовка линий парковки
        ParkingLine90();

        HBRUSH WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdc, WhiteBrush);

        // Отрисовка линий под углом 90 градусов
        for (int i = 0; i < MaxParkingSpace90; ++i) {  //                        x1-x3 y1-y3 x2-x4
            //                        y2-y4
            Rectangle(hdc, ParkingLineStartX90Left, ParkingLineStartY90[i],
                ParkingLineEndX90Left, ParkingLineEndY90[i]);
            Rectangle(hdc, ParkingLineStartX90Right, ParkingLineStartY90[i],
                ParkingLineEndX90Right, ParkingLineEndY90[i]);
        }
        DeleteObject(WhiteBrush);
    }

private:
    const int MaxParkingSpace90 = 9;
    const int MaxParkingSpaceParallel = 4;
    // Задаем координаты начальных и конечных линий для парковки под углом 90
    // градусов

    // Для парковки слева
    int ParkingLineStartX90Left = 400;
    int ParkingLineEndX90Left = 500;

    // Для парковки справа
    int ParkingLineStartX90Right = 700;
    int ParkingLineEndX90Right = 800;


    // Массивы для хранения координат середин парковочного места

    // Для координат парковочного места под углом в 90 градусов
    mutable int ParkingLineStartY90[11] = { 1 };
    mutable int ParkingLineEndY90[11] = { 6 };


    // Метод для получения координат начала и конца парковочных линий под углом в
    // 90 градусов
    void ParkingLine90() const {
        for (int i = 1; i < MaxParkingSpace90; ++i) {
            ParkingLineStartY90[i] =
                PARKING_LINE_INTERVAL + ParkingLineStartY90[i - 1];
            ParkingLineEndY90[i] = PARKING_LINE_INTERVAL + ParkingLineEndY90[i - 1];
        }
    }

protected:
    // Метод для получаения координат середины парковочного места под углом в 90
    // градусов
    void MidParking90(double xParLeft[9], double xParRight[9],
        double yPar[11]) const {
        // Вызываем для получения координат парковочных линий для парковки в 90
        // градусов (получаем середину через прямоугольники)
        ParkingLine90();
        for (int i = 0; i < 8; i++) {
            xParLeft[i] = (ParkingLineStartX90Left + ParkingLineEndX90Left) * 0.5;
            yPar[i] = (ParkingLineStartY90[i] + ParkingLineEndY90[i + 1]) * 0.5;
            xParRight[i] = (ParkingLineStartX90Right + ParkingLineEndX90Right) * 0.5;
        }
    }
};

// Класс дорог и разметки, наследуется от игрового объекта
class Road
    : public GameObject {
public:
    // Конструктор дорог
    Road() : GameObject(10, 0, 200, 1500) {}
    Road(int startX, int startY, int roadWidth, int roadHeight)
        : GameObject(startX, startY, roadWidth, roadHeight) {}

    // Метод для отрисовки дорог и разметки
    void Draw(HDC hdc) const override {
        // Отображение дороги (прямоугольник)
        HBRUSH GrayBrush = CreateSolidBrush(
            RGB(105, 105, 105));  // Создаем кисть для отрисовки дорог
        SelectObject(hdc, GrayBrush);

        Rectangle(hdc, x, y, x + width, y + height);  // Отрисовываем главную дорогу
        Rectangle(hdc, x + 199, y + 600, x + 391,
            y + 800);  // Отрисовываем дорогу до парковочной области

        // Освобождаем ресурсы кисти
        DeleteObject(GrayBrush);

        // Рисуем разметку
        HBRUSH WhiteBrush = CreateSolidBrush(
            RGB(255, 255, 255));  // Создаем кисть для отрисовки разметки
        SelectObject(hdc, WhiteBrush);

        // Задаем начальные координаты
        int y1 = y, y2 = y;

        // Рисуем разметку на главной дороге
        for (int i = 0; i < 19; i++) {
            Rectangle(hdc, x + 90, y1, x + 110, y2 + 40);
            y1 = y2 + 60;
            y2 += 80;
        }

        // Задаем начальные координаты для дороги до парковочного места
        int x1 = x + 209, x2 = x + 249;

        // Рисуем разметку на дороге до парковочного места
        for (int i = 0; i < 3; ++i) {
            Rectangle(hdc, x1, y + 690, x2, y + 710);
            x1 = x2 + 20;
            x2 += 60;
        }

        // Освобождаем ресурсы кистм
        DeleteObject(WhiteBrush);
    }
};

// Класс дом, наследуется от игрового объекта
class House : public GameObject {
private:
    // Получаем цвет
    COLORREF color;

public:
    // Конструктор дома
    House(COLORREF houseColor, int* startX, int* startY, int* endX, int* endY)
        : GameObject(startX[0], startY[0], endX[0], endY[0]), color(houseColor) {}

    // Метод для отрисовки дома
    void Draw(HDC hdc) const override {
        // Отображение дома (прямоугольник) с цветом

        HBRUSH hBrush = CreateSolidBrush(color);
        //HBRUSH whiteBrush = CreateSolidBrush(RGB(255,255,255));
        SelectObject(hdc, hBrush);

        // Рисуем дом
        Rectangle(hdc, x, y, x + width, y + height);

        // SelectObject(hdc,whiteBrush);
         // Очищаем ресурсы кисти
        DeleteObject(hBrush);
    }
};

// Класс препядствия, наследуется от игрового объекта
class Obstacle : public GameObject {
public:
    // Конструктор по умолчанию
    Obstacle() : GameObject(0, 0, 0, 0) {}

    // Конструктор для создания препятствия с заданными координатами, шириной и высотой
    Obstacle(int startX, int startY, int obstacleWidth, int obstacleHeight)
        : GameObject(startX, startY, obstacleWidth, obstacleHeight) {}


    // Метод для отображения препятствия
    void Draw(HDC hdc) const override {
        HBRUSH BlackBrush = CreateSolidBrush(RGB(0, 0, 0));
        SelectObject(hdc, BlackBrush);
        Rectangle(hdc, x, y, x + width, y + height); // Создаем прямоугольник как препядствие 
        DeleteObject(BlackBrush);
    }

    void Reset()
    {
        x = -1;
        y = -1;
    }
    // Метод для проверки столкновения с другим объектом
    bool IsCollision(const GameObject& other) const {
        // Проверяем столкновение с другим объектом
        if (x < other.GetX() + other.GetWidth() &&
            x + width > other.GetX() &&
            y < other.GetY() + other.GetHeight() &&
            y + height > other.GetY()) {
            return true; // Столкновение обнаружено
        }
        return false; // Нет столкновения
    }
};
std::vector<Car> cars;  // Создаем вектор для хранения данных машин
// Класс для работы с остальными классами, наследуется от игрового объекта
class Backyard : protected ParkingArea {
private:
    // Создаем игровые объекты

    Car userCar;  // Создаем объект пользовательской машины
    ParkingArea parkingArea;  // Создаем объект парковочной области
    Road road;                  // Создаем объект дороги
    std::vector<House> houses;  // Вектор для хранения домов
    Obstacle obstacle; // Добавляем препятствие

    // Координаты домов
    int startX[5] = { 220, 220, 810, 810 ,810 };
    int endX[5] = { 170, 170, 250, 250 ,400 };
    int startY[5] = { 30, 200, 290, 500 ,9 };
    int endY[5] = { 150, 350, 200, 300,200 };

    double userCarAngle = 0.0;  // Добавленная переменная для хранения угла
    // поворота машины пользователя

    double targetUserCarAngle =
        0.0;  // Добавленная переменная для хранения целевого угла
    double userCarRotationSpeed = 0.1;  // Скорость поворота машины
    double maxRotationAngle = 0.1;  // Максимальный угол поворота за один кадр

    bool isParking = false;  // Стоит ли машина на парковочном месте


public:
    // Конструктор двора
    Backyard() {
        srand(static_cast<unsigned>(time(nullptr)));  // Сбиваем значения

        //Генерируем машины
        GenerateRandomCars(MAX_CARS);


        // Добавляем дома
        for (int i = 0; i < 5; ++i) {
            int randColor = RGB(rand() % 256, rand() % 256,
                rand() % 256);  // Задаем случайный цвет для дома
            if (i < 4)
            {
                houses.push_back(
                    House(randColor, &startX[i], &startY[i], &endX[i], &endY[i]));
            }
            else
            {
                randColor = RGB(255, 255, 255);
                houses.push_back(
                    House(randColor, &startX[i], &startY[i], &endX[i], &endY[i]));
            }
        }
        //Генерируем возможное препядствие
        GenerateRandomObstacle();
    }
    void GenerateCarsAtTopLeft(int numCars) {
        for (int i = 0; i < numCars; ++i) {
            cars.push_back(Car(0, 0));  // Создаем машину в левом верхнем углу
        }
    }
    void MoveCarsToFreeParkingSpaces() {
        for (auto& car : cars) {
            if (!car.isParked) {
                for (const auto& parkingSpace : parkingSpaces) {
                    if (!parkingSpace.isOccupied) {
                        car.SetTargetParkingPosition(parkingSpace.x, parkingSpace.y);
                        break;
                    }
                }
            }
        }
    }
    void CheckCarCollisions() {
        for (auto& car : cars) {
            if (obstacle.IsCollision(car)) {
                // Измените направление машины
            }
            for (auto& otherCar : cars) {
                if (&car != &otherCar && car.IsCollision(otherCar)) {
                    // Измените направление машины
                }
            }
        }
    }
    void MoveParkedCarsToBottomLeft() {
        for (auto& car : cars) {
            if (car.isParked) {
                car.SetTargetParkingPosition(0, screenHeight);  // screenHeight - высота экрана
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
        if (obstacle.IsCollision(car))
        {
            return true;
        }

        return false;  // Нет коллизии
    }

    // Метод для генерации машин на случайных местах
    void GenerateRandomCars(int numCars) {
        // Задаем константы для настроек генерации машин
        constexpr int MaxCars = 8;
        constexpr int MaxParkingLines = 16;
        constexpr int ParkingLineWidth = 50;
        constexpr int ParkingLineHeight = 25;

        // Массивы для отслеживания состояния парковочных линий и параллельных линий
        bool parked90[MaxParkingLines] = { false };

        // Массивы для хранения координат машин при парковке под углом
        double xParLeft[MaxCars] = {};
        double xParRight[MaxCars] = {};
        double yPar[MaxCars] = {};

        // Массивы для отслеживания использования координат при парковке под углом
        bool usedxParLeft[MaxCars] = { false };
        bool usedxParRight[MaxCars] = { false };
        bool usedyParLeft[MaxCars] = { false };
        bool usedyParRight[MaxCars] = { false };

        // Генерация координат машин для парковки под углом
        MidParking90(xParLeft, xParRight, yPar);

        int i = 0;

        // Цикл генерации машин
        while (i < numCars) {
            // Генерация случайного индекса для выбора парковочной линии или
            // параллельной линии
            int index = rand() % (MaxParkingLines);

            // Проверка, что линия еще не занята
            if (!parked90[index]) {
                int xrand90 = rand() % MaxCars;
                int yrand90 = rand() % MaxCars;

                double xplace =
                    (index % 2 == 0) ? xParLeft[xrand90] : xParRight[xrand90];
                double yplace = yPar[yrand90];
                if (index % 2 == 0) {
                    // Проверка, если место еще не использовано
                    if (!usedxParLeft[xrand90] && !usedyParLeft[yrand90]) {
                        cars.push_back(
                            Car(xplace - ParkingLineWidth, yplace - ParkingLineHeight));
                        parked90[index] = true;
                        i++;
                        usedxParLeft[xrand90] = true;
                        usedyParLeft[yrand90] = true;
                        
                    }
                }
                else if (!usedxParRight[xrand90] && !usedyParRight[yrand90]) {
                    cars.push_back(
                        Car(xplace - ParkingLineWidth, yplace - ParkingLineHeight));
                    parked90[index] = true;
                    
                    i++;
                    usedxParRight[xrand90] = true;
                    usedyParRight[yrand90] = true;
                }
            }
        }
    }


    // Метод для генерации случайного препятствия

    void GenerateRandomObstacle() {
        // Шанс появления препятствия
        if (rand() % 100 < 99) { // Настройка шанса генерации препядствия
            obstacle.Reset();
            int x = 200+rand() % 600;
            int y = rand() % 600;
            int width = 30; // Ширина препятствия
            int height = 30; // Высота препятствия
            //Obstacle newObstacle(x, y, width, height);
            obstacle.SetX(x);
            obstacle.SetY(y);
            obstacle.SetWidth(width);
            obstacle.SetHeight(height);
            // Проверяем столкновение с другими машинами
            for (const auto& car : cars) {
                if (obstacle.IsCollision(car)) {
                    // Обработка столкновения с другими машинами (пересоздание препятствия)
                    //newObstacle.Reset();
                    GenerateRandomObstacle();
                    return;
                }
            }

            // Проверяем столкновение с домами
            for (const auto& house : houses) {
                if (obstacle.IsCollision(house)) {
                    // Обработка столкновения с домами (пересоздание препятствия
                    //newObstacle.Reset();
                    GenerateRandomObstacle();
                    return;
                }
            }
        }
    }

    //// Метод для движения машины пользователя
    //void MoveUserCar(int dx, int dy) {
    //    // Запоминаем предыдущие координаты машины пользователя
    //    int prevX = userCar.GetX();
    //    int prevY = userCar.GetY();

    //    // Вычисляем угол направления движения курсора
    //    double angle = atan2(dy, dx);
    //    double targetAngle = atan2(dy, dx);

    //    // Устанавливаем целевой угол поворота машины пользователя
    //    targetUserCarAngle = angle;

    //    // Вычисляем разницу между текущим и целевым углом
    //    double angleDiff = targetUserCarAngle - userCarAngle;

    //    // Интерполяция координат и угла для создания эффекта плавного движения
    //    double interpolationFactor = 0.1;
    //    userCarAngle = Lerp(userCarAngle, targetUserCarAngle, interpolationFactor);
    //    userCar.SetMovement(sqrt(dx * dx + dy * dy), userCarAngle);

    //    // Обновляем координаты машины пользователя с учетом интерполяции
    //    double newX = Lerp(prevX, prevX + dx, interpolationFactor);
    //    double newY = Lerp(prevY, prevY + dy, interpolationFactor);
    //    userCar.Move(static_cast<int>(newX), static_cast<int>(newY), userCarAngle);

    //    // Устанавливаем скорость и угол движения машины пользователя
    //    userCar.SetMovement(sqrt(dx * dx + dy * dy), angle);

    //    // Поворачиваем машину в движении с учетом скорости вращения
    //    userCar.RotateInMotion(userCarRotationSpeed);
    //    // Перемещаем машину пользователя
    //    userCar.Move(prevX + dx, prevY + dy, userCarAngle);

    //    // Проверка на выход за границы карты и возврат на предыдущие координаты при
    //    // выходе
    //    if (userCar.GetX() < MAP_LEFT_BORDER ||
    //        userCar.GetX() + userCar.GetWidth() > MAP_RIGHT_BORDER ||
    //        userCar.GetY() < MAP_TOP_BORDER ||
    //        userCar.GetY() + userCar.GetHeight() > MAP_BOTTOM_BORDER) {
    //        userCar.Move(prevX, prevY, userCarAngle);
    //    }
    //    // Проверка на столкновение с препятствием
    //    if (obstacle.IsCollision(userCar)) {
    //        userCar.SetColor(RGB(255, 0, 0));
    //        //obstacle.Reset();
    //    }
    //    // Проверка на наличие парковки и подсветка цвета машины
    //    if (!isParking) {
    //        // Проверка корректности парковки и установка цвета машины
    //        userCar.SetColor(CheckCorrectParking() ? RGB(0, 0, 0) : RGB(0, 255, 0));
    //    }

    //    // Проверка на столкновение с другими машинами и возврат на предыдущие
    //    // координаты при столкновении
    //    if (CheckCollision(userCar)) {
    //        userCar.Move(prevX, prevY, userCarAngle);
    //    }
    //}

    // Функция линейной интерполяции
    double Lerp(double a, double b, double t) {
        return a + t * (b - a);
    }

    // Метод проверки корректности парковки машины пользователя
    bool CheckCorrectParking() const {
        // Получаем координаты и размеры машины пользователя
        double userCarX = userCar.GetX();
        double userCarY = userCar.GetY();
        int userCarWidth = userCar.GetWidth();
        int userCarHeight = userCar.GetHeight();

        // Получаем координаты и размеры парковочных областей
        double xParLeft[10], xParRight[10], yPar[10];
        MidParking90(xParLeft, xParRight, yPar);

        // Проверяем, что машина находится внутри пределов парковочной области
        bool isInParkingArea = false;

        // Проверка для линий под углом 90 градусов
        for (int i = 0; i < 9; ++i) {
            if (userCarY >= yPar[i] - 25 &&
                userCarY + userCarHeight <= yPar[i + 1] + PARKING_LINE_INTERVAL) {
                if ((userCarX >= xParLeft[i] - 50 &&
                    userCarX + userCarWidth <=
                    xParLeft[i + 1] + PARKING_LINE_INTERVAL) ||
                    (userCarX >= xParRight[i] - 50 &&
                        userCarX + userCarWidth <=
                        xParRight[i + 1] + PARKING_LINE_INTERVAL)) {
                    isInParkingArea = true;
                    break;
                }
            }
        }

        // Проверяем, что машина не пересекает другие машины и дома
        if (isInParkingArea && !CheckCollision(userCar)) {
            return true;  // Парковка корректна
        }
        return false;  // Парковка не корректна
    }



    // Метод для отображения
    void Draw(HDC hdc) const {
        parkingArea.Draw(hdc);  // Отображение парковочной площадки
        road.Draw(hdc);  // Отображение дорог
        obstacle.Draw(hdc);
        // Отображение домов
        for (const auto& house : houses) {
            house.Draw(hdc);
        }

        // Отображение машин
        for (const auto& car : cars) {
            car.Draw(hdc);
        }
        

        // Отображение пользовательской машины
        //userCar.Draw(hdc);
    }

    void ToggleFullscreen(HWND hwnd) {
        fullscreen = !fullscreen;

        if (fullscreen) {
            SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
        }
        else {
            SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            SetWindowPos(hwnd, HWND_TOP, MAP_LEFT_BORDER, MAP_TOP_BORDER, MAP_RIGHT_BORDER, MAP_BOTTOM_BORDER, SWP_FRAMECHANGED);
        }
    }

    COLORREF LerpColor(COLORREF color1, COLORREF color2, double t) {
        int r1 = GetRValue(color1);
        int g1 = GetGValue(color1);
        int b1 = GetBValue(color1);

        int r2 = GetRValue(color2);
        int g2 = GetGValue(color2);
        int b2 = GetBValue(color2);

        int r = r1 + t * (r2 - r1);
        int g = g1 + t * (g2 - g1);
        int b = b1 + t * (b2 - b1);

        return RGB(r, g, b);
    }

};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    // Инициализация структуры WNDCLASSEX
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    hBackgroundBrush = CreateSolidBrush(RGB(135, 206, 235));
    wc.hbrBackground = hBackgroundBrush;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"WindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Регистрация класса окна
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Failed to register window class.", L"Error",
            MB_ICONERROR);
        return 1;
    }

    // Создание окна
    HWND hwnd = CreateWindowEx(0, L"WindowClass", L"Parking", WS_OVERLAPPEDWINDOW,
        MAP_LEFT_BORDER, MAP_TOP_BORDER, MAP_RIGHT_BORDER,
        MAP_BOTTOM_BORDER, NULL, NULL, hInstance, NULL);

    // Проверка успешного создания окна
    if (!hwnd) {
        MessageBox(NULL, L"Failed to create window.", L"Error", MB_ICONERROR);
        return 2;
    }
    // Проверка наличмя экрана
    if (!GetSystemMetrics(SM_CXSCREEN) || !GetSystemMetrics(SM_CYSCREEN)) {
        MessageBox(NULL, L"Unable to detect screen resolution.", L"Error", MB_ICONERROR);
        return 3;
    }

    // Отображение окна
    SetTimer(hwnd, 1, 1000, NULL); // устанавливает таймер с идентификатором 1, который срабатывает каждую секунду
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Обработка сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Удаление таймера и объекта кисти
    KillTimer(hwnd, 1);
    DeleteObject(hBackgroundBrush);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
    LPARAM lParam) {
    static Backyard backyard;
    //static bool fullscreen = false;  // Флаг полноэкранного режима
    PAINTSTRUCT ps;
    HDC hdc;
    bool isSliderMoving = false;
    switch (uMsg) {


    case WM_KEYDOWN:
        if (wParam == VK_F11) {
            backyard.ToggleFullscreen(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        backyard.Draw(hdc);
        EndPaint(hwnd, &ps);

        break;

    case WM_SIZE:
        break;

    case WM_CREATE:
    {
        // Создание ползунка времени
        HWND hwndTrack = CreateWindowEx(
            0,                               // no extended styles 
            TRACKBAR_CLASS,                   // class name 
            L"Trackbar Control",              // title (caption) 
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ, // style 
            850, 10,                          // position 
            300, 30,                         // size 
            hwnd,                            // parent window 
            (HMENU)ID_TRACKBAR,              // control identifier 
            GetModuleHandle(NULL),           // instance 
            NULL                             // no WM_CREATE parameter 
        );

        // Установка диапазона и позиции ползунка
        SendMessage(hwndTrack, TBM_SETRANGE,
            (WPARAM)TRUE,                    // redraw flag 
            (LPARAM)MAKELONG(0, 24));        // min. & max. positions 

        SendMessage(hwndTrack, TBM_SETPOS,
            (WPARAM)TRUE,                    // redraw flag 
            (LPARAM)12);                     // current position 

        // Создание шрифта
        HFONT hFont = CreateFont(
            10, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, TEXT("Arial"));

        for (int i = 0; i <= 24; i += 6) {
            WCHAR str[5];
            wsprintf(str, L"%d", i);

            // Объявление и создание статического элемента управления
            HWND hwndStatic = CreateWindowEx(
                0,                               // no extended styles 
                L"STATIC",                       // class name 
                str,                             // window text 
                WS_CHILD | WS_VISIBLE | SS_CENTER, // style 
                850 + i * (155 / 12), 40,         // position 
                10, 10,                          // size 
                hwnd,                            // parent window 
                NULL,                            // no menu 
                GetModuleHandle(NULL),           // instance 
                NULL                             // no WM_CREATE parameter 
            );

            // Установка шрифта
            SendMessage(hwndStatic, WM_SETFONT, (WPARAM)hFont, TRUE);
        }

        // Создание статического элемента управления для текста
        HWND hwndText = CreateWindowEx(
            0,                               // no extended styles 
            L"STATIC",                       // class name 
            L"",                             // window text 
            WS_CHILD | WS_VISIBLE | SS_CENTER, // style 
            880, 70,                          // position 
            300, 40,                         // size 
            hwnd,                            // parent window 
            NULL,                            // no menu 
            GetModuleHandle(NULL),           // instance 
            NULL                             // no WM_CREATE parameter 
        );

        // Установка шрифта
        SendMessage(hwndText, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }
    case WM_HSCROLL:
    {

        if (GetDlgCtrlID((HWND)lParam) == ID_TRACKBAR)
        {
            int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
            isSliderMoving = true;

            // Изменение фона в зависимости от времени
            COLORREF dayColor = RGB(135, 206, 235);
            COLORREF nightColor = RGB(25, 25, 112);
            COLORREF backgroundColor;

            // Изменение количества машин на парковке
            int numCars;

            if (pos >= 8 && pos <= 16) {
                // Меньше машин в период с 8 до 16s
                numCars = MAX_CARS / 2;
            }
            else {
                // Больше машин в остальное время
                numCars = MAX_CARS - 1;  // Оставляем одно свободное место
            }

            // Обновление вектора машин
            cars.resize(numCars);
            backyard.GenerateRandomCars(numCars);
            hdc = BeginPaint(hwnd, &ps);
            backyard.Draw(hdc);
            EndPaint(hwnd, &ps);
            //backyard.Draw(hdc);

            if (pos <= 12) {
                // Утро и день
                double t = pos / 12.0;  // Нормализация положения ползунка от 0 до 1
                backgroundColor = backyard.LerpColor(nightColor, dayColor, t);
            }
            else {
                // Вечер и ночь
                double t = (pos - 12) / 12.0;  // Нормализация положения ползунка от 0 до 1
                backgroundColor = backyard.LerpColor(dayColor, nightColor, t);
            }

            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(backgroundColor));
            InvalidateRect(hwnd, NULL, TRUE);
        }
        else {
            isSliderMoving = false;
        }
        break;
    }

    case WM_TIMER:
    {
        // Проверяем, двигается ли ползунок
        if (!isSliderMoving)
        {
            // Генерируем случайное число от 0 до MAX_CARS
            int randomCarIndex = rand() % MAX_CARS;
            backyard.GenerateCarsAtTopLeft(numCars);
            // Проверяем, находится ли машина на парковке
            if (cars[randomCarIndex].GetisParking())
            {
                // Если машина на парковке, она выезжает
                backyard.MoveParkedCarsToBottomLeft();
            }
            else
            {
                // Если машина не на парковке, она въезжает
                backyard.MoveCarsToFreeParkingSpaces();
                backyard.CheckCarCollisions();
            }
        }
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);  // Закрытие окна при нажатии кнопки закрытия
        break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        DeleteObject(hBackgroundBrush);
        PostQuitMessage(0);  // Завершение работы приложения
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
