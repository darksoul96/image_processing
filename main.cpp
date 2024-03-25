
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

Mat& ScanImageAndReduceC(Mat& I) {
    // accept only char type matrices
    CV_Assert(I.depth() == CV_8U);

    int channels = I.channels();
    int nRows = I.rows;
    int nCols = I.cols;

    if (I.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int i,j;
    for (i = 0; i < nRows; ++i) {
        for (j = 0; j < nCols; ++j) {
            Vec3b& pixel = I.at<Vec3b>(i, j);
            for (int c = 0; c < channels; ++c) {
                if (pixel[c] >= 255)
                    pixel[c] = 0; // To prevent overflow, reset to 0 if greater than 255
                else
                    pixel[c] = 255 - pixel[c]; // Subtract 255 from each channel
            }
        }
    }
    return I;
}

int main(int argc, //number of strings in argv
         char** argv) {
    
    string imageName = "";
    string transformCommand = "";
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
            cout << "-------HELP-------" << endl;
            cout << "List of arguments: [image path] [flag]" << endl;
            cout << "------------------" << endl;
            cout << "# image path: path to the image (jpg or png extensions only)" << endl;
            cout << "# flags: " << endl;
            cout << "   -h, -help:    show help " << endl;
            cout << "   -s, -show:    show image " << endl;
            cout << "   -n, -negative:    show negative image " << endl;
        }
    } else if (argc == 3) {
        imageName = argv[1];
        if ((imageName.find(".jpg") || imageName.find(".png")) != string::npos) {
            // Read the image file
            Mat image = imread(imageName);
            // Check for failure
            if (image.empty()) {
                cout << "Could not open or find the image" << endl;
                cin.get(); //wait for any key press
                return -1;
            }

            if (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-show") == 0) {
                String windowName = "Show image"; //Name of the window

                namedWindow(windowName); // Create a window

                imshow(windowName, image); // Show our image inside the created window.

                waitKey(0); // Wait for any keystroke in the window

                destroyWindow(windowName); //destroy the created window
                //ScanImageAndReduceC(image);
            } else if (strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-show") == 0) {
                
            }

        } else {
            cout << "Enter a jpg or png image as first argument followed by flag command. Enter -h or -help to show help." << endl;
        }
    } else {
        cout << "Enter command followed by -h or -help to show help" << endl;
        return -1;
    }

    return 0;
}
