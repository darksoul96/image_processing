
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

Mat& scanImageAndInvertColors(Mat& I) {
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


Mat& scanImageAndTurnBlackAndWhite(Mat& I) {
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
            float average = (pixel[0] + pixel[1] + pixel[2])/3.0;
            if (average > 128) {
                pixel[0] = 255;
                pixel[1] = 255;
                pixel[2] = 255;
            } else {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
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
            cout << "   -bnw, -blacknwhite:    show black and white image " << endl;
        } else {
            cout << "Enter a jpg or png image as first argument followed by flag command. Enter -h or -help to show help." << endl;
        }
    } else if (argc >= 3) {
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
        
            int argumentCount = 2;
            vector<String> windowsOpened;
            while (argumentCount < argc) {
                if (strcmp(argv[argumentCount], "-s") == 0 || strcmp(argv[argumentCount], "-show") == 0) {
                    String showWindow = "original";
                    namedWindow(showWindow); 
                    imshow(showWindow, image); 
                    windowsOpened.push_back(showWindow);
                } else if (strcmp(argv[argumentCount], "-n") == 0 || strcmp(argv[argumentCount], "-negative") == 0) {
                    //Negative image
                    Mat negativeImage = image.clone();
                    scanImageAndInvertColors(negativeImage);
                    String negativeWindow = "negative"; 
                    namedWindow(negativeWindow); 
                    imshow(negativeWindow, negativeImage); 
                    windowsOpened.push_back(negativeWindow);
                } else if (strcmp(argv[argumentCount], "-bnw") == 0 || strcmp(argv[argumentCount], "-blacknwhite") == 0) {
                    //Black and white image
                    Mat blackAndWhiteImage = image.clone();
                    scanImageAndTurnBlackAndWhite(blackAndWhiteImage);
                    String blackAndWhiteWindow = "black and white"; 
                    namedWindow(blackAndWhiteWindow); 
                    imshow(blackAndWhiteWindow, blackAndWhiteImage); 
                    windowsOpened.push_back(blackAndWhiteWindow);
                }
                argumentCount++;
                
            }
            waitKey(0);
            for (int i=0;i<windowsOpened.size(); ++i) {
                destroyWindow(windowsOpened[i]);
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
