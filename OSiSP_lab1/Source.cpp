#include <Windows.h>

LPCTSTR WindowName = TEXT("OSiSP_lab_1");
#define TIMER_START_MOVEMENTS 1001
#define TIMER_CONTINUE_MOVEMENTS 1002

const char VK_W = 0x57;
const char VK_A = 0x41;
const char VK_S = 0x53;
const char VK_D = 0x44;
constexpr auto step = 10;
constexpr auto PATH = L"C:\\Users\\Vera\\Desktop\\3 курс\\5 сем\\осисп\\OSiSP_lab1\\sprite.bmp";

COORD spritePosition = { 0, 0 };
HBITMAP hSprite;
SIZE spriteSize;
bool reversedx = false;
bool reversedy = false;

void loadSprite(HWND hwnd);
ATOM RegMyWindowClass(HINSTANCE);
SIZE GetSpriteSize(HBITMAP hBitmap);
bool showSprite(HWND hwnd, COORD coordiantes);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR cmdLine, _In_ int showMode)
{
    MSG msg;
    RegMyWindowClass(hInstance);

    HWND hWnd = CreateWindow(WindowName,  // Указатель на зарегистрированное имя класса.
        WindowName,                       // Указатель на имя окна.
        WS_OVERLAPPEDWINDOW,              // Стиль окна.
        CW_USEDEFAULT,                    // Горизонтальная позиция окна.
        CW_USEDEFAULT,                    // Вертикальная позиция окна.
        CW_USEDEFAULT,                    // Ширина окна.
        CW_USEDEFAULT,                    // Высота окна.
        NULL,                             // Дескриптор родительского или окна владельца.
        NULL,                             // Дескриптор меню или ID дочернего окна.
        hInstance,                        // Дескриптор экземпляра приложения.
        NULL);

    ShowWindow(hWnd, showMode);
    loadSprite(hWnd);
    SetTimer(hWnd, TIMER_START_MOVEMENTS, 1000, NULL);
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

ATOM RegMyWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX windowClassEx;

    windowClassEx.cbSize = sizeof(WNDCLASSEX);
    windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
    windowClassEx.lpfnWndProc = WndProc;
    windowClassEx.cbClsExtra = 0;
    windowClassEx.cbWndExtra = 0;
    windowClassEx.hInstance = hInstance;
    windowClassEx.hIcon = LoadIcon(0, IDI_WINLOGO);;
    windowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
    windowClassEx.hbrBackground = (HBRUSH)0;
    windowClassEx.lpszMenuName = 0;
    windowClassEx.lpszClassName = WindowName;
    windowClassEx.hIconSm = 0;

    return RegisterClassEx(&windowClassEx);
}

void loadSprite(HWND hwnd) {
    
    hSprite = (HBITMAP)LoadImageW(NULL, PATH, IMAGE_BITMAP, 200, 200, LR_LOADFROMFILE);
    if (hSprite != NULL) {
        spriteSize = GetSpriteSize(hSprite);
        showSprite(hwnd, {0,0});
    }
}

SIZE GetSpriteSize(HBITMAP hBitmap)
{
    BITMAP sprite;
    GetObject(hBitmap, sizeof(BITMAP), &sprite);

    SIZE spriteSize;
    spriteSize.cx = sprite.bmWidth;
    spriteSize.cy = sprite.bmHeight;

    return spriteSize;
}

bool showSprite(HWND hwnd, COORD coordiantes) {
    HDC winDC = GetDC(hwnd);//дескрипт инф окна
    HDC memDC = CreateCompatibleDC(winDC);//
    HGDIOBJ oldBmp = SelectObject(memDC, hSprite);

    BitBlt(winDC, coordiantes.X, coordiantes.Y, spriteSize.cx, spriteSize.cy, memDC, 0, 0, SRCCOPY);//выполняет побитовую передачу данных о цвете
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);//удаляет указанный контекст устройста dc
    ReleaseDC(hwnd, winDC);//отображает контекст устройства dc  
    return true;
}

SIZE GetClientWindowSize(HWND hWnd)
{
    RECT windowRectangle;
    GetClientRect(hWnd, &windowRectangle);

    SIZE windowSize;
    windowSize.cx = windowRectangle.right - windowRectangle.left;
    windowSize.cy = windowRectangle.bottom - windowRectangle.top;

    return windowSize;
}

COORD createNewSpritePosition(HWND hwnd, short xSTEP, short yStep) {
    SIZE windowSize = GetClientWindowSize(hwnd);

    COORD newSpritePosition;

    newSpritePosition.X = (spritePosition.X + xSTEP);// если зашло за левую границу
    if (newSpritePosition.X < 0)
    {
        newSpritePosition.X = 0 + 2 * step;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)//за правую
    {
        newSpritePosition.X = windowSize.cx - spriteSize.cx - 2 * step;
    }

    newSpritePosition.Y = (spritePosition.Y + yStep);
    if (newSpritePosition.Y < 0)//за верхнюю
    {
        newSpritePosition.Y = 0 + 2 * step;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)//за нижнюю
    {
        newSpritePosition.Y = windowSize.cy - spriteSize.cy - 2 * step;
    }

    return newSpritePosition;
}

int FillWindowWithColor(HWND hWnd, COLORREF color)
{
    RECT windowRectangle; //определяет прямюугольник координатами
    GetClientRect(hWnd, &windowRectangle);//получает координаты клиент области окна

    HDC windowDC = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(color);

    int result = FillRect(windowDC, &windowRectangle, hBrush);

    DeleteObject(hBrush);
    ReleaseDC(hWnd, windowDC);

    return result;
}

void changeSpritePosition(HWND hwnd, short x, short y) {
    spritePosition = createNewSpritePosition(hwnd, x, y);
    FillWindowWithColor(hwnd, GetSysColor(COLOR_WINDOW));
    showSprite(hwnd, spritePosition);
}

void setTimer(HWND hwnd, UINT_PTR timerId) {
    if (timerId == TIMER_START_MOVEMENTS) {
        SetTimer(hwnd, timerId, 1000, NULL);
        KillTimer(hwnd, TIMER_CONTINUE_MOVEMENTS);
    }
    else {
        SetTimer(hwnd, timerId, 100, NULL);
        KillTimer(hwnd, TIMER_START_MOVEMENTS);
    }
}

void createAutomaticMovements(HWND hwnd) {
    SIZE windowSize = GetClientWindowSize(hwnd);

    COORD newSpritePosition;
    int xStep = reversedx ? -step : step;
    newSpritePosition.X = (spritePosition.X + xStep);
    if (newSpritePosition.X < 0)
    {
        reversedx = false;
        newSpritePosition.X = 0 + step;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)
    {
        reversedx = true;
        newSpritePosition.X = (SHORT)(windowSize.cx - spriteSize.cx - step);
    }
    int yStep = reversedy ? -step : step;
    newSpritePosition.Y = (spritePosition.Y + yStep);
    if (newSpritePosition.Y < 0)
    {
        reversedy = false;
        newSpritePosition.Y = 0 + step;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)
    {
        reversedy = true;
        newSpritePosition.Y = (SHORT)(windowSize.cy - spriteSize.cy - step);
    }

    spritePosition = newSpritePosition;
    FillWindowWithColor(hwnd, GetSysColor(COLOR_WINDOW));
    showSprite(hwnd, spritePosition);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
     COORD spritePosition = { 0, 0 };

    switch (message)
    {
    case WM_TIMER:
        if (wParam == TIMER_START_MOVEMENTS)
        {
            setTimer(hWnd, TIMER_CONTINUE_MOVEMENTS);
        }
        else {
            createAutomaticMovements(hWnd);
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
          case VK_UP:
          case VK_W:
            changeSpritePosition(hWnd, 0, -step);
            setTimer(hWnd, TIMER_START_MOVEMENTS);
            break;
          case VK_RIGHT:
          case VK_D:
            changeSpritePosition(hWnd, step, 0);
            setTimer(hWnd, TIMER_START_MOVEMENTS);
            break;
          case VK_DOWN:
          case VK_S:
            changeSpritePosition(hWnd, 0, step);
            setTimer(hWnd, TIMER_START_MOVEMENTS);
            break;
          case VK_LEFT:
          case VK_A:
            changeSpritePosition(hWnd, -step, 0);
            setTimer(hWnd, TIMER_START_MOVEMENTS);
            break;
        default:
            break;
        }
        break;

    case WM_MOUSEWHEEL:
        if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
        {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                changeSpritePosition(hWnd, 0, -step);
                setTimer(hWnd, TIMER_START_MOVEMENTS);
            }
            else
            {
                changeSpritePosition(hWnd, 0, step);
                setTimer(hWnd, TIMER_START_MOVEMENTS);
            }
        }
        else {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                changeSpritePosition(hWnd, step, 0);
                setTimer(hWnd, TIMER_START_MOVEMENTS);
            }
            else
            {
                changeSpritePosition(hWnd, -step, 0);
                setTimer(hWnd, TIMER_START_MOVEMENTS);
            }
        }
        break;
   
    case WM_DESTROY:
        KillTimer(hWnd, TIMER_START_MOVEMENTS);
        KillTimer(hWnd, TIMER_CONTINUE_MOVEMENTS);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

