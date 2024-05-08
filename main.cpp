
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


cv::Mat& scanImageAndInvertColors(cv::Mat& I) {
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
            cv::Vec3b& pixel = I.at<cv::Vec3b>(i, j);
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


cv::Mat& scanImageAndTurnBlackAndWhite(cv::Mat& I) {
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
            cv::Vec3b& pixel = I.at<cv::Vec3b>(i, j);
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


cv::Vec3b sharpMaskAverage(cv::Mat I, int currX, int currY, float kernel[3][3]) {
    cv::Vec3b newPixel;
    float averageR = (I.at<cv::Vec3b>(currX-1, currY-1)[0]*kernel[0][0] +
                     I.at<cv::Vec3b>(currX, currY-1)[0]*kernel[0][1] +
                     I.at<cv::Vec3b>(currX, currY-1)[0]*kernel[0][2] +
                     I.at<cv::Vec3b>(currX-1, currY)[0]*kernel[1][0] +
                     I.at<cv::Vec3b>(currX, currY)[0]*kernel[1][1] +
                     I.at<cv::Vec3b>(currX+1, currY)[0]*kernel[1][2] +
                     I.at<cv::Vec3b>(currX-1, currY+1)[0]*kernel[2][0] +
                     I.at<cv::Vec3b>(currX, currY+1)[0]*kernel[2][1] +
                     I.at<cv::Vec3b>(currX+1, currY+1)[0]*kernel[2][2]);

    float averageG = (I.at<cv::Vec3b>(currX-1, currY-1)[1]*kernel[0][0] +
                     I.at<cv::Vec3b>(currX, currY-1)[1]*kernel[0][1] +
                     I.at<cv::Vec3b>(currX, currY-1)[1]*kernel[0][2] +
                     I.at<cv::Vec3b>(currX-1, currY)[1]*kernel[1][0] +
                     I.at<cv::Vec3b>(currX, currY)[1]*kernel[1][1] +
                     I.at<cv::Vec3b>(currX+1, currY)[1]*kernel[1][2] +
                     I.at<cv::Vec3b>(currX-1, currY+1)[1]*kernel[2][0] +
                     I.at<cv::Vec3b>(currX, currY+1)[1]*kernel[2][1] +
                     I.at<cv::Vec3b>(currX+1, currY+1)[1]*kernel[2][2]);

    float averageB = (I.at<cv::Vec3b>(currX-1, currY-1)[2]*kernel[0][0] +
                     I.at<cv::Vec3b>(currX, currY-1)[2]*kernel[0][1] +
                     I.at<cv::Vec3b>(currX, currY-1)[2]*kernel[0][2] +
                     I.at<cv::Vec3b>(currX-1, currY)[2]*kernel[1][0] +
                     I.at<cv::Vec3b>(currX, currY)[2]*kernel[1][1] +
                     I.at<cv::Vec3b>(currX+1, currY)[2]*kernel[1][2] +
                     I.at<cv::Vec3b>(currX-1, currY+1)[2]*kernel[2][0] +
                     I.at<cv::Vec3b>(currX, currY+1)[2]*kernel[2][1] +
                     I.at<cv::Vec3b>(currX+1, currY+1)[2]*kernel[2][2]);

    newPixel[0] = averageR;
    newPixel[1] = averageG;
    newPixel[2] = averageB;
    return newPixel;
}


cv::Mat& scanImageAndSharpen(cv::Mat& I) {
    float kernel [3][3] = { {0, -1, 0},{-1, 5, -1},{0, -1, 0} };
    CV_Assert(I.depth() == CV_8U);
    int channels = I.channels();
    int nRows = I.rows;
    int nCols = I.cols;
    if (I.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }
    int i,j;
    for (i = 1; i < nRows-1; ++i) {
        for (j = 1; j < nCols-1; ++j) {
            cv::Vec3b& pixel = I.at<cv::Vec3b>(i, j);
            cv::Mat copy = I.clone();
            cv::Vec3b newPixel = sharpMaskAverage(copy, i, j, kernel);
            
            
            pixel[0] = newPixel[0];
            pixel[1] = newPixel[1];
            pixel[2] = newPixel[2];
        }
    }
    return I;
}


int main(int argc, //number of strings in argv
         char** argv) {
    
    std::string imageName = "";
    std::string transformCommand = "";
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0) {
            std::cout << "-------HELP-------\n";
            std::cout << "List of arguments: [image path] [flag]\n";
            std::cout << "------------------\n";
            std::cout << "# image path: path to the image (jpg or png extensions only)\n";
            std::cout << "# flags: \n";
            std::cout << "   -h, -help:    show help \n";
            std::cout << "   -s, -show:    show image \n";
            std::cout << "   -n, -negative:    show negative image \n";
            std::cout << "   -bnw, -blacknwhite:    show black and white image \n";
            std::cout << "   -sharp:    sharpen image \n";
        } else {
            std::cout << "Enter a jpg or png image as first argument followed by flag command. Enter -h or -help to show help.\n";
        }
    } else if (argc >= 3) {
        imageName = argv[1];
        if ((imageName.find(".jpg") || imageName.find(".png")) != std::string::npos) {
            // Read the image file
            cv::Mat image = cv::imread(imageName);
            // Check for failure
            if (image.empty()) {
                std::cout << "Could not open or find the image\n";
                std::cin.get(); //wait for any key press
                return -1;
            }
        
            int argumentCount = 2;
            std::vector<std::string> windowsOpened;
            int coordinateX = 0;
            int coordinateY = 0;
            while (argumentCount < argc) {
                if (strcmp(argv[argumentCount], "-s") == 0 || strcmp(argv[argumentCount], "-show") == 0) {
                    std::string showWindow = "original";
                    cv::namedWindow(showWindow); 
                    imshow(showWindow, image); 
                    cv::moveWindow(showWindow, coordinateX, coordinateY);
                    coordinateX += 500;
                    coordinateY += 200;
                    windowsOpened.push_back(showWindow);
                } else if (strcmp(argv[argumentCount], "-n") == 0 || strcmp(argv[argumentCount], "-negative") == 0) {
                    //Negative image
                    cv::Mat negativeImage = image.clone();
                    scanImageAndInvertColors(negativeImage);
                    std::string negativeWindow = "negative"; 
                    cv::namedWindow(negativeWindow); 
                    imshow(negativeWindow, negativeImage); 
                    cv::moveWindow(negativeWindow, coordinateX, coordinateY);
                    coordinateX += 500;
                    coordinateY += 200;
                    windowsOpened.push_back(negativeWindow);
                } else if (strcmp(argv[argumentCount], "-bnw") == 0 || strcmp(argv[argumentCount], "-blacknwhite") == 0) {
                    //Black and white image
                    cv::Mat blackAndWhiteImage = image.clone();
                    scanImageAndTurnBlackAndWhite(blackAndWhiteImage);
                    std::string blackAndWhiteWindow = "black and white"; 
                    cv::namedWindow(blackAndWhiteWindow); 
                    imshow(blackAndWhiteWindow, blackAndWhiteImage); 
                    cv::moveWindow(blackAndWhiteWindow, coordinateX, coordinateY);
                    coordinateX += 500;
                    coordinateY += 200;
                    windowsOpened.push_back(blackAndWhiteWindow);
                } else if (strcmp(argv[argumentCount], "-sharp") == 0) {
                    //Sharp image
                    cv::Mat sharpImage = image.clone();
                    scanImageAndSharpen(sharpImage);
                    std::string sharpWindow = "sharp"; 
                    cv::namedWindow(sharpWindow); 
                    imshow(sharpWindow, sharpImage); 
                    cv::moveWindow(sharpWindow, coordinateX, coordinateY);
                    coordinateX += 500;
                    coordinateY += 200;
                    windowsOpened.push_back(sharpWindow);
                }
                argumentCount++;
                
            }
            cv::waitKey(0);
            for (int i=0;i<windowsOpened.size(); ++i) {
                cv::destroyWindow(windowsOpened[i]);
            }
        } else {
            std::cout << "Enter a jpg or png image as first argument followed by flag command. Enter -h or -help to show help.\n";
        }

            

        
    } else {
        std::cout << "Enter command followed by -h or -help to show help\n";
        return -1;
    }

    return 0;
}
