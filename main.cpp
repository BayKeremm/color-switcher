#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cstdio>
#include <iostream>
using namespace cv;
using namespace std;
static void help(char** argv)
{
    cout << "\nThis is a program that uses the watershed algorithm to find the wall in an image (Wall should be the largest segment)\n"
    "Usage:\n" << argv[0] <<" [image_name -- default is fruits.jpg]\n" << endl;
    cout << "Hot keys: \n"
    "\tESC - quit the program\n"
    "\tr - restore the original image\n"
    "\tc - open color picker to put RGB values (press enter to select color)\n"
    "\tw or SPACE - run watershed segmentation algorithm\n"
    "\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
    "\t (before that, roughly outline several markers on the image)\n";
}
Mat markerMask, img, img_copy;
int r = 0, g = 0, b = 0;
Point prevPt(-1, -1);
static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
    return;
    if( event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON) )
    prevPt = Point(-1,-1);
    else if( event == EVENT_LBUTTONDOWN )
    prevPt = Point(x,y);
    else if( event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON) )
    {
    Point pt(x, y);
    if( prevPt.x < 0 )
    prevPt = pt;
    line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
    line( img, prevPt, pt, Scalar::all(255), 5, 8, 0 );
    prevPt = pt;
    imshow("image", img);
    }
}
int main( int argc, char** argv )
{
    cv::CommandLineParser parser(argc, argv, "{help h | | }{ @input | fruits.jpg | }");
    if (parser.has("help"))
    {
        help(argv);
        return 0;
    }
    string filename = samples::findFile(parser.get<string>("@input"));
    Mat img0 = imread(filename, IMREAD_COLOR), imgGray;
    img0.copyTo(img_copy);
    if( img0.empty() )
    {
        cout << "Couldn't open image ";
        help(argv);
        return 0;
    }
    help(argv);

    namedWindow( "image", 1 );
    img0.copyTo(img);

    cvtColor(img, markerMask, COLOR_BGR2GRAY);
    cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);

    markerMask = Scalar::all(0);
    imshow( "image", img );
    setMouseCallback( "image", onMouse, 0 );
    for(;;)
        {
        char c = (char)waitKey(0);
        if( c == 27 )
            break;
        if( c == 'r' )
        {
            markerMask = Scalar::all(0);
            img0.copyTo(img);
            imshow( "image", img );
        }
        if(c == 'c'){
            namedWindow("Color Picker", WINDOW_NORMAL);


            createTrackbar("Red", "Color Picker", &r, 255);
            createTrackbar("Green", "Color Picker", &g, 255);
            createTrackbar("Blue", "Color Picker", &b, 255);

            Mat colorWindow(100, 100, CV_8UC3);

            while (true)
            {
                colorWindow.setTo(Vec3b(b, g, r));
                imshow("Color Picker", colorWindow);

                char key = waitKey(1);
                if (key == 13) // Press enter to continue
                    break;
                else if (key == ' ') // Press Spacebar to confirm color selection
                    break;
            }

            destroyWindow("Color Picker");

            cout << "Selected Color: R=" << r << " G=" << g << " B=" << b << endl;
        }
        if( c == 'w' || c == ' ' )
        {
            int i, j, compCount = 0;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
            if( contours.empty() )
            continue;

            Mat markers(markerMask.size(), CV_32S);
            markers = Scalar::all(0);
            int idx = 0;

            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
            drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);

            if( compCount == 0 )
            continue;

            watershed( img0, markers );

            // Find the largest segment
            int largestIndex = -1;
            int largestSize = 0;
            for (i = 0; i < compCount; i++)
            {
                int currentSize = countNonZero(markers == (i + 1));
                if (currentSize > largestSize)
                {
                    largestSize = currentSize;
                    largestIndex = i;
                }
            }

            // Create an image to show only the largest segment in color
            Mat largestSegment(img.size(), CV_8UC3, Scalar(0, 0, 0));
            for (i = 0; i < markers.rows; i++)
            {
                for (j = 0; j < markers.cols; j++)
                {
                    int index = markers.at<int>(i, j);
                    if (index == (largestIndex + 1))
                    {
                        largestSegment.at<Vec3b>(i, j) = Vec3b(b, g, r); // Light blue color
                    }else{
                        largestSegment.at<Vec3b>(i, j) = img_copy.at<Vec3b>(i, j);
                    }
                }
            }

            imshow("largest segment", largestSegment);
        }
    }
    return 0;
}
