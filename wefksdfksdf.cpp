#include <windows.h>
#include <algorithm>
#include <cmath>
#include<vector>
#include<time.h>
#include <CommCtrl.h>
#include<ctime>
#include<limits>
#include<cstdlib>
#define ID_TRACKBAR 101
#define ID_BUTTON_PARKING 102
// Определение полного экрана
bool fullscreen = false;
const int MAP_LEFT_BORDER = 0;
const int MAP_RIGHT_BORDER = 1280;
const int MAP_TOP_BORDER = 0;
const int MAP_BOTTOM_BORDER = 800;
// Максимально возможное количество машин
int MAX_CARS = 10;

// Прототип функции работы с окном(для коректной работы)
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

class Car : public GameObject {
private:
    double angle; // Угол поворота машины
    COLORREF color; // Для хранения цвета машины
    bool isParking = false;
    double speed = 0.0;
    double direction = 0.0;
    const double MaxSpeed = 10;

public:

    Car() 
        :GameObject(0, 0, 100, 50), angle(0), color(RGB(0,255,0)) {};
    Car(int startX, int startY)
        :GameObject(startX, startY, 100, 50),angle(0),
        color(RGB(0, 255, 0)) {}

    void SetisParking(bool Parking) { isParking = Parking; }
    
    bool GetisParking() { return isParking; }
   
    bool IsCollision(const GameObject& other) const {
        if (x < other.GetX() + other.GetWidth() &&
            x + width > other.GetX() &&
            y < other.GetY() + other.GetHeight() &&
            y + height > other.GetY()) {
            return true; // Столкновение обнаружено
        }
        return false; // Нет столкновения
    }


    // Метод для перемещения машины
    void Move(int newX, int newY) {
        x = newX;
        y = newY;
        //angle = newAngle;
    }
    void MoveForward(double distance) {
        x += cos(angle) * distance;
        y += sin(angle) * distance;
    }
    void TurnLeft(double angle) {
        this->angle -= angle;
    }
    void TurnRight(double angle) {
        this->angle += angle;
    }
    //Устанавливаем цвет машины
    void SetColor(COLORREF newColor) { color = newColor;}

    //Рисуем машину
    void Draw(HDC hdc)const override {
        HBRUSH ColorBrush = CreateSolidBrush(color);
        HBRUSH WindowBrush = CreateSolidBrush(RGB(0, 0, 255));
        SelectObject(hdc, ColorBrush);

        //Пока не понятно как отривовыветь угол поворота машины
        // Добавить потом
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
        //Отрисовка машины
        //Rectangle(hdc, x, y, width, height);
        // Отрисовка прямоугольника, представляющего машину
        Rectangle(hdc, x - centerX, y - centerY, x - centerX + width,
            y - centerY + height);

        // Отрисовка окон
        SelectObject(hdc, WindowBrush);
        //Rectangle(hdc, x, y, width, height);//Пробные значения
        Rectangle(hdc, x + 60 - centerX, y + 10 - centerY, x + 70 - centerX,
            y + 40 - centerY);
        // Сбрасываем трансформацию
        SetGraphicsMode(hdc, GM_COMPATIBLE);
        ModifyWorldTransform(hdc, nullptr, MWT_IDENTITY);
        DeleteObject(ColorBrush);
        DeleteObject(WindowBrush);
    }

    
};



class ParkingArea : public GameObject {
private:
    const int PARKING_AREA_X = 300;
    const int PARKING_AREA_Y = 0;
    const int PARKING_AREA_WIDTH = 800;
    const int PARKING_AREA_HEIGHT = 800;


    //Количество парковочных мест
    const int MaxParkingSpace = 9;

    // Начальные координаты для парковки слева
    int ParkingLineStartXLeft = 300;
    int ParkingLineEndXLeft = 400;

    // Начальные координаты для парковки справа
    int ParkingLineStartXRight = 700;
    int ParkingLineEndXRight = 800;

    // Массивы для хранения координат парковочных линий

    mutable int ParkingLineStartY[11] = { 1 };
    mutable int ParkingLineEndY[11] = { 6 };

    // Метод для получения координат начала и конца парковочных линий
    void ParkingLine() const {
        for (int i = 1; i < MaxParkingSpace; ++i) {
            ParkingLineStartY[i] =
                PARKING_LINE_INTERVAL + ParkingLineStartY[i - 1];
            ParkingLineEndY[i] = PARKING_LINE_INTERVAL + ParkingLineEndY[i - 1];
        }
    }
protected:
    // Парковочный интервал
    const int PARKING_LINE_INTERVAL = 75;

    //Метод для получения координат середины парковочных линий
    void MidParking(double xParLeft[9], double xParRight[9],
        double yPar[11]) const {
        // Вызываем для получения координат парковочных линий для парковки 
        for (int i = 0; i < 8; i++) {
            xParLeft[i] = (ParkingLineStartXLeft + ParkingLineEndXLeft) * 0.5;
            yPar[i] = (ParkingLineStartY[i] + ParkingLineEndY[i + 1]) * 0.5;
            xParRight[i] = (ParkingLineStartXRight + ParkingLineEndXRight) * 0.5;
        }
    }

public:
    ParkingArea() :GameObject(PARKING_AREA_X, PARKING_AREA_Y, PARKING_AREA_WIDTH,
        PARKING_AREA_HEIGHT) {
        ParkingLine();
    }

    // Отрисовка парковочной арены
    void Draw(HDC hdc)const override {
        HBRUSH GrayBrush = CreateSolidBrush(RGB(105, 105, 105));
        SelectObject(hdc, GrayBrush);

        Rectangle(hdc, PARKING_AREA_X, PARKING_AREA_Y, PARKING_AREA_WIDTH, PARKING_AREA_HEIGHT);
        DeleteObject(GrayBrush);

        HBRUSH WhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        SelectObject(hdc, WhiteBrush);

        //Отрисовка парковочных мест
        for (int i = 0; i < MaxParkingSpace; i++) {
            Rectangle(hdc, ParkingLineStartXLeft, ParkingLineStartY[i],
                ParkingLineEndXLeft, ParkingLineEndY[i]);
            Rectangle(hdc, ParkingLineStartXRight, ParkingLineStartY[i],
                ParkingLineEndXRight, ParkingLineEndY[i]);
        }
        DeleteObject(WhiteBrush);
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
        Rectangle(hdc, x , y + 600, x + 291,
            y + 800);  // Отрисовываем дорогу до парковочной области

        // Освобождаем ресурсы кисти
        DeleteObject(GrayBrush);

        // Рисуем разметку
        HBRUSH WhiteBrush = CreateSolidBrush(
            RGB(255, 255, 255));  // Создаем кисть для отрисовки разметки
        SelectObject(hdc, WhiteBrush);


        // Задаем начальные координаты для дороги до парковочной области
        int x1 = x , x2 = x + 49;

        // Рисуем разметку на дороге до парковочного места
        for (int i = 0; i < 5; ++i) {
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
    House(int StartX,int StartY,int StartWidth,int StarttHeight,COLORREF houseColor) 
        :GameObject(StartX,StartY,StartWidth, StarttHeight),color(houseColor) {}

    // Метод для отрисовки дома
    void Draw(HDC hdc) const override {
        // Отображение дома (прямоугольник) с цветом

        HBRUSH hBrush = CreateSolidBrush(color);
        SelectObject(hdc, hBrush);

        // Рисуем дом
        Rectangle(hdc, x, y, x + width, y + height);

         // Очищаем ресурсы кисти
        DeleteObject(hBrush);
    }
};

// Класс препядствия, наследуется от игрового объекта
class Obstacle : public GameObject {
public:
    Obstacle() :GameObject(0, 0, 0, 0) {};
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

std::vector<Car>cars;

class Yard : protected ParkingArea {
private:
  
    ParkingArea parkingArea;  // Создаем объект парковочной области
    Road road;                  // Создаем объект дороги
    Obstacle obstacle; // Добавляем препятствие
    std::vector<House> houses;  // Вектор для хранения домов
    bool isParking = false;  // Стоит ли машина на парковочном месте
    double userCarAngle = 0.0;  // Добавленная переменная для хранения угла
    // поворота машины пользователя
 


    double targetUserCarAngle =
        0.0;  // Добавленная переменная для хранения целевого угла
    double userCarRotationSpeed = 0.1;  // Скорость поворота машины
    double maxRotationAngle = 0.1;  // Максимальный угол поворота за один кадр

    // Координаты домов
    int startX[5] = { 120, 120, 810, 810 ,810 };
    int startWidth[5] = { 170, 170, 250, 250 ,400 };
    int startY[5] = { 30, 200, 290, 500 ,9 };
    int startHeight[5] = { 150, 350, 200, 300,200 };

public:
    Yard() {
        srand(static_cast<unsigned>(time(nullptr)));  // Сбиваем значения
       
        //Генерируем машины
        GenerateRandomCars(MAX_CARS);

        //Добавляем дома
        for (int i = 0; i <5; i++) {
            int randColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            if (i < 4)
            {
                houses.push_back(House(startX[i], startY[i], startWidth[i], startHeight[i],randColor));
            }
            else {
                randColor = RGB(255, 255, 255);
                houses.push_back(House(startX[i],startY[i],startWidth[i],startHeight[i],randColor));
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
        while (i < numCars)
        {
            int index = rand() % MaxParkingLines - 1;
            int randColor = RGB(rand() % 255, rand() % 255, rand() % 255);
            if (!parked[index]) {
                {
                    int xrand = rand() % MaxCars;
                    int yrand = rand() % MaxCars;
                    double xplace = (index % 2 == 0) ? xParLeft[xrand] : xParRight[xrand];
                    double yplace = yPar[yrand];
                    if (index % 2 == 0) {
                        if (!usedxParLeft[xrand] && !usedyParLeft[yrand])
                        {
                            cars.push_back(
                                Car(xplace - ParkingLineWidth, yplace - ParkingLineHeight));;
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
                    else 
                        if (!usedxParRight[xrand] && !usedyParRight[yrand])
                        {
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
            for (const auto&car:cars)
            {
                if (obstacle.IsCollision(car))
                {
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
        if (obstacle.IsCollision(car))
        {
            return true;
        }

        return false;  // Нет коллизии
    }

    // Проверка на правильность парковки
    bool CheckCorrectParking(int number)const {
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
        for (const auto& car : cars)
        {
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
   
    void Draw(HDC hdc)const  {
        parkingArea.Draw(hdc);  // Отображение парковочной площадки
        road.Draw(hdc);  // Отображение дорог
        obstacle.Draw(hdc);
        // Отображение домов
        for (const auto& house : houses) {
            house.Draw(hdc);
        }

        // Отображение машин
        for (const auto&car:cars) {
            car.Draw(hdc);
        }
    }
    void toParking(HWND hwnd) {
        bool isParking[16] = { false };
        cars.push_back(Car(0, 1100));
        
        int number = cars.size() - 1;
        cars[number].Move(400, 1100);
        UpdateWindow(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        cars[number].Move(400, 900);
        UpdateWindow(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        // Массивы для хранения координат машин при парковки
        double xParLeft[8] = {};
        double xParRight[8] = {};
        double yPar[8] = {};
        MidParking(xParLeft, xParRight, yPar);
        while (!cars[number].GetisParking())
        {
            int index = rand() % 16;
            for (int i = 0;i<cars.size()-2;i++) {
                int x = cars[i].GetX();
                int y = cars[i].GetY();
                for (int j = 0; j < 8; j++) {
                    if (x == xParRight[j] && y == yPar[j] || x == xParLeft[j] && y == yPar[j]) {
                        cars[number].Move(400, y);
                        UpdateWindow(hwnd);
                        InvalidateRect(hwnd, NULL, TRUE);
                        cars[number].Move(x, y);
                        UpdateWindow(hwnd);
                        InvalidateRect(hwnd, NULL, TRUE);
                        cars[number].SetisParking(true);
                    }
                }
                
            }
        }

    }

    // --------------------------------------------------------------------------------------------------------
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static Yard yard;
    PAINTSTRUCT ps;
    HDC hdc;
    bool isSliderMoving = false;
    switch (uMsg) {



    case WM_KEYDOWN:
        if (wParam == VK_F11) {
            yard.ToggleFullscreen(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            break;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        yard.Draw(hdc);
        EndPaint(hwnd, &ps);
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
        // Создайте кнопку
        // Создайте кнопку
        HWND hwndButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Parking", // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            10,         // x position 
            10,         // y position 
            100,        // Button width
            30,         // Button height
            hwnd,       // Parent window
            (HMENU)ID_BUTTON_PARKING, // Button ID
            GetModuleHandle(NULL),
            NULL);      // Pointer not needed.
        break;
        // Установка шрифта
        SendMessage(hwndText, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_BUTTON_PARKING: {
            // Вызовите функцию toParking при нажатии на кнопку
            yard.toParking(hwnd);
            break;
        }
        }
        break;
    }
    case WM_HSCROLL:
    {

        if (GetDlgCtrlID((HWND)lParam) == ID_TRACKBAR)
        {
            int pos = SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);

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
            cars.resize(numCars);
            yard.GenerateRandomCars(numCars);
            hdc = BeginPaint(hwnd, &ps);
            yard.Draw(hdc);
            EndPaint(hwnd, &ps);
            if (pos <= 12) {
                // Утро и день
                double t = pos / 12.0;  // Нормализация положения ползунка от 0 до 1
                backgroundColor = yard.LerpColor(nightColor, dayColor, t);
            }
            else {
                // Вечер и ночь
                double t = (pos - 12) / 12.0;  // Нормализация положения ползунка от 0 до 1
                backgroundColor = yard.LerpColor(dayColor, nightColor, t);
            }

            SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(backgroundColor));
            InvalidateRect(hwnd, NULL, TRUE);
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
}
