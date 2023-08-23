#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    namedWindow("Color Picker", WINDOW_NORMAL);

    int r = 0, g = 0, b = 0;

    createTrackbar("Red", "Color Picker", &r, 255);
    createTrackbar("Green", "Color Picker", &g, 255);
    createTrackbar("Blue", "Color Picker", &b, 255);

    Mat colorWindow(100, 100, CV_8UC3);

    while (true)
    {
        colorWindow.setTo(Vec3b(b, g, r));
        imshow("Color Picker", colorWindow);

        char key = waitKey(1);
        if (key == 27) // Press ESC to exit
            break;
        else if (key == ' ') // Press Spacebar to confirm color selection
            break;
    }

    destroyWindow("Color Picker");

    cout << "Selected Color: R=" << r << " G=" << g << " B=" << b << endl;

    return 0;
}
