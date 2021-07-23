#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <math.h>
#include <chrono>
#include <thread>

using namespace std;

/**
 * @brief a higher resolution sleep function than is provided by windows.h
 * 
 * @param seconds 
 */
void preciseSleep(double seconds) {
    using namespace std;
    using namespace std::chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        this_thread::sleep_for(milliseconds(1));
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}

/**
 * @brief given the change in position of the mouse, this function will move the mouse relative 
 * to its current position by the amount of the passed parameters, and take the duration passed to do it
 * 
 * @param deltaX change in x position
 * @param deltaY change in y position
 * @param duration delay in ms
 */
void moveCursorRelative(int deltaX, int deltaY, unsigned int duration) {
    POINT startingMousePosition;
    GetCursorPos(&startingMousePosition);
    int startX = startingMousePosition.x;
    int startY = startingMousePosition.y;
    int destX, destY;
    const int stepCount = 50;
    int xSteps[stepCount];
    int ySteps[stepCount];
    double sleepDuration = abs(duration / (stepCount * 1.0));
    float iterativeDifX = (deltaX / (stepCount * 1.0));
    float iterativeDifY = (deltaY / (stepCount * 1.0));

    // populate step buffers
    for (size_t i = 0; i < stepCount; i++)
    {
        xSteps[i] = startX + (int)(i * iterativeDifX);
        ySteps[i] = startY + (int)(i * iterativeDifY);
    }
    
    // move the mouse
    for (size_t i = 0; i < stepCount; i++)
    {
        SetCursorPos(xSteps[i], ySteps[i]);
        preciseSleep(sleepDuration / 1000.0);
    }
}

/**
 * @brief Swings the club in pga tour 2k21 to the specified power
 * 
 * @param power power of shot
 */
void swingClub(int power){
    int downSwingDelay = 680;
    int upSwingDelay = 98;
    int downSwingX = (int)((100 / (power * 1.0)) * 1000);
    int upSwingX = (int)(((100 / (power * 1.0)) * 1000) * - 1);

    // swing the club
    // left down 
    INPUT    Input={0};
    Input.type      = INPUT_MOUSE;
    Input.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
    ::SendInput(1,&Input,sizeof(INPUT));
    // end of mouse button down ---------------------------------------------------
    // move mouse 
    moveCursorRelative(0, downSwingX, downSwingDelay);
    moveCursorRelative(0, upSwingX, upSwingDelay);
    // end of move mouse -----------------------------------------------------
    // left up
    ::ZeroMemory(&Input,sizeof(INPUT));
    Input.type      = INPUT_MOUSE;
    Input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
    ::SendInput(1,&Input,sizeof(INPUT));
    // end of mouse button up -----------------------------------------------------
}

int main()
{
    while (1)
    {
        char input = _getch();
        Sleep(100);
        if (input == 'n' || input == 'N') {
            /**
             * @brief At this point the user has indicated that they would like to swing so well go ahead and prompt
             * them to enter their power level as an integer and well swing the club
             * 
             */

            int powerLevel; 
            cout << "Please enter an integer value: ";
            cin >> powerLevel;
            cout << endl << "The value you entered is " << powerLevel;
            Sleep(3000);
            swingClub(powerLevel);
            cout << endl << "done swinging" << endl;
        }
    }
}