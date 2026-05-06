#include <iostream>
#include <windows.h>

bool clicking = false;
int targetR = 255, targetG = 0, targetB = 0;
int tolerance = 30;

void SmartClick(int x, int y) {
    // Метод 1: Физическое движение мыши
    SetCursorPos(x, y);
    Sleep(30);

    // Метод 2: Клик через SendInput (самый надежный)
    INPUT inputs[2] = {0};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, inputs, sizeof(INPUT));

    // Метод 3: Дополнительно отправляем сообщение в активное окно
    HWND activeWindow = GetForegroundWindow();
    if (activeWindow && activeWindow != GetConsoleWindow()) {
        POINT pt = {x, y};
        ScreenToClient(activeWindow, &pt);
        PostMessage(activeWindow, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
        PostMessage(activeWindow, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
    }
}

// Функция для получения цвета пикселя под курсором
COLORREF GetPixelColorAtCursor() {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HDC hdc = GetDC(NULL);
    COLORREF color = GetPixel(hdc, cursorPos.x, cursorPos.y);
    ReleaseDC(NULL, hdc);
    return color;
}

// Функция проверки совпадения цвета с учетом допуска
bool IsColorMatch(COLORREF color) {
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    
    return (abs(r - targetR) <= tolerance && 
            abs(g - targetG) <= tolerance && 
            abs(b - targetB) <= tolerance);
}

// Функция автокликера
void AutoClicker() {
    while (true) {
        if (clicking) {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            COLORREF currentColor = GetPixelColorAtCursor();
            
            if (IsColorMatch(currentColor)) {
                SmartClick(cursorPos.x, cursorPos.y);
                std::cout << "Clicked at (" << cursorPos.x << ", " << cursorPos.y << ")" << std::endl;
            }
        }
        Sleep(50); // Небольшая задержка между проверками
    }
}

int main() { 
    std::cout << "=== SMART WINDOW CLICKER ===\n"; 
    std::cout << "5 - Capture color\n"; 
    std::cout << "7 - Start smart clicking\n"; 
    std::cout << "8 - Stop clicking\n"; 
    std::cout << "9 - Exit\n\n"; 
     
    std::cout << "INSTRUCTIONS:\n"; 
    std::cout << "1. First click on the target window (browser, game, etc)\n"; 
    std::cout << "2. Then use this program\n"; 
    std::cout << "3. Make sure target window is ACTIVE (in foreground)\n\n"; 
     
    while (true) { 
        if (GetAsyncKeyState('5') & 0x8000) { 
            POINT p; 
            GetCursorPos(&p); 
            HDC hdc = GetDC(0); 
            COLORREF color = GetPixel(hdc, p.x, p.y); 
            ReleaseDC(0, hdc); 
            targetR = GetRValue(color); 
            targetG = GetGValue(color); 
            targetB = GetBValue(color); 
            std::cout << "Target color captured: RGB(" << targetR << ", " << targetG << ", " << targetB << ")\n"; 
            Sleep(500); 
        } 
         
        if (GetAsyncKeyState('7') & 0x8000) { 
            clicking = true; 
            std::cout << "* SMART CLICKING ACTIVATED *\n"; 
            std::cout << "Make sure your target window is active!\n"; 
            Sleep(500); 
        } 
         
        if (GetAsyncKeyState('8') & 0x8000) { 
            clicking = false; 
            std::cout << "* CLICKING STOPPED *\n"; 
            Sleep(500); 
        } 
         
        if (GetAsyncKeyState('9') & 0x8000) break; 
         
        if (clicking) { 
            POINT p; 
            GetCursorPos(&p); 
            HDC hdc = GetDC(0); 
            COLORREF color = GetPixel(hdc, p.x, p.y); 
            ReleaseDC(0, hdc); 
             
            int r = GetRValue(color); 
            int g = GetGValue(color); 
            int b = GetBValue(color); 
             
            if (abs(r - targetR) <= tolerance &&  
                abs(g - targetG) <= tolerance &&  
                abs(b - targetB) <= tolerance) { 
                 
                std::cout << "SMART CLICK at " << p.x << ", " << p.y << "\n"; 
                SmartClick(p.x, p.y); 
                Sleep(500); 
            } 
        } 
         
        Sleep(50); 
    } 
     
    return 0; 
}