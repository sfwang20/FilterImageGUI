#include <iostream>
#include <string>
#include <sstream>

// OpenCV includes
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
using namespace cv;

Mat img;

// color space
enum class colorSpace { BGR, Gray, HSV, HSL };
void showBGR(int, void*);
void showGray(int, void*);
void showHSV(int, void*);
void showHSL(int, void*);
void applyColorSpace(colorSpace);

// Filters
void equalize(int, void*);
void lomo(int, void*);
void cartoon(int, void*);
void sketch(int, void*);

// Detect edges
bool applySobel = false;
bool applyBlur = false;
bool applyCanny = false;
bool applyLaplace = false;
void sobelImage(int, void*);
void cannyImage(int, void*);
void laplaceImage(int, void*);
void applyFilters();

void blurImage(int, void*);
void showHistogram(int, void*);

// Detector (face, nose, ... )
void faceDetect(int, void*);

// change contrast and brightness by Trackerbar
double alpha=1.0, beta = 0;
void contrastCallBack(int, void*);
void brightnessCallback(int, void*);
void adjustContrastAndBright();


int main(int argc, const char** argv) {
    // comaandLine pasrse
    const char* keys =
    {
        "{help h usage ? | | print this message}"
        "{@image | | Image to process}"
    };
    CommandLineParser parser(argc, argv, keys);
    parser.about("PhotoTool v1.0.0");

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    String imgFile = parser.get<String>(0);

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    // Load image
    img = imread(imgFile);
    if (!img.data)
    {
        std::cout << "Load image failed! Please check and try again." << std::endl;
        return EXIT_FAILURE;
    }

    // Create window
    namedWindow("Lena");

    /* Create UI buttons */
    // Color Space convertion
    createButton("RGB", showBGR, NULL, QT_RADIOBOX, 1);
    createButton("Gray", showGray, NULL, QT_RADIOBOX, 0);
    createButton("HSV", showHSV, NULL, QT_RADIOBOX, 0);
    createButton("HSL", showHSL, NULL, QT_RADIOBOX, 0);    
    
    // Filters
    createButton("Equalize histogram", equalize, NULL, QT_NEW_BUTTONBAR, 0);
    createButton("Lomography effect", lomo, NULL, QT_PUSH_BUTTON, 0);
    createButton("Cartonize effect", cartoon, NULL, QT_PUSH_BUTTON, 0);
    createButton("Sketch", sketch, NULL, QT_PUSH_BUTTON, 0);

    // Detect edges
    createButton("Sobel", sobelImage, NULL, QT_NEW_BUTTONBAR, 0);
    createButton("Canny", cannyImage, NULL, QT_PUSH_BUTTON, 0);
    createButton("Laplacian", laplaceImage, NULL, QT_PUSH_BUTTON, 0);
    // Blur image
    createButton("Blur", blurImage, NULL, QT_CHECKBOX, 0);

    // Detect faces (if the input image has faces)
    createButton("Show histogram", showHistogram, NULL, QT_NEW_BUTTONBAR, 0);
    // show histogram of image
    createButton("Face detect", faceDetect, NULL, QT_PUSH_BUTTON, 0);
    
    // contrast and brightness
    int initValue = 50;
    createTrackbar("Contrast", "Lena", &initValue, 100, contrastCallBack);
    contrastCallBack(50, 0);
    createTrackbar("Brightness", "Lena", &initValue, 100, brightnessCallback);
    brightnessCallback(50, 0);    

    imshow("Lena", img);
    waitKey(0);
    return EXIT_SUCCESS;
}


void showBGR(int state, void* userData)
{
    colorSpace s = colorSpace::BGR;
    applyColorSpace(s);
}

void showGray(int state, void* userData)
{
    colorSpace s = colorSpace::Gray;
    applyColorSpace(s);
}

void showHSV(int state, void* userData)
{
    colorSpace s = colorSpace::HSV;
    applyColorSpace(s);
}

void showHSL(int state, void* userData)
{
    colorSpace s = colorSpace::HSL;
    applyColorSpace(s);
}

void applyColorSpace(colorSpace s) {
    Mat result;
    img.copyTo(result);
    switch (s) {
        case colorSpace::BGR:
            imshow("Lena", img);
            break;
        case colorSpace::Gray:
            cvtColor(result, result, COLOR_BGR2GRAY);
            imshow("Lena", result);
            break;
        case colorSpace::HSV:
            cvtColor(result, result, COLOR_BGR2HSV);
            imshow("Lena", result);
            break;
        case colorSpace::HSL:
            cvtColor(result, result, COLOR_BGR2HLS);
            imshow("Lena", result);
            break;
    }
}


void showHistogram(int state, void* userData)
{
    // Separate image in BGR
    std::vector<Mat> bgr;
    split(img, bgr);

    // 256 bins (the number of possibles values)
    int numbins = 256;

    // Set the ranges for B,G,R
    float range[] = { 0, 256 };
    const float* histRange = { range };

    Mat b_hist, g_hist, r_hist;

    calcHist(&bgr[0], 1, 0, Mat(), b_hist, 1, &numbins, &histRange);
    calcHist(&bgr[1], 1, 0, Mat(), g_hist, 1, &numbins, &histRange);
    calcHist(&bgr[2], 1, 0, Mat(), r_hist, 1, &numbins, &histRange);

    // Draw the histogram
    int width = 512;
    int height = 300;
    Mat histImage(height, width, CV_8UC3, Scalar(20, 20, 20));

    // Normalize the histograms to height of image
    normalize(b_hist, b_hist, 0, height, NORM_MINMAX);
    normalize(g_hist, g_hist, 0, height, NORM_MINMAX);
    normalize(r_hist, r_hist, 0, height, NORM_MINMAX);

    // draw lines for each channel
    int binStep = cvRound((float)width / (float)numbins);
    for (int i = 1; i < numbins; i++)
    {
        line(histImage,
            Point(binStep * (i - 1), height - cvRound(b_hist.at<float>(i - 1))),
            Point(binStep * (i), height - cvRound(b_hist.at<float>(i))),
            Scalar(255, 0, 0)
        );
        line(histImage,
            Point(binStep * (i - 1), height - cvRound(g_hist.at<float>(i - 1))),
            Point(binStep * (i), height - cvRound(g_hist.at<float>(i))),
            Scalar(0, 255, 0)
        );
        line(histImage,
            Point(binStep * (i - 1), height - cvRound(r_hist.at<float>(i - 1))),
            Point(binStep * (i), height - cvRound(r_hist.at<float>(i))),
            Scalar(0, 0, 255)
        );
    }

    imshow("Histogram", histImage);
}


void equalize(int state, void* userData)
{
    Mat result, ycrcb;
    cvtColor(img, ycrcb, COLOR_BGR2YCrCb);

    std::vector<Mat> channels;
    split(ycrcb, channels);

    // Equalize the Y channel only
    equalizeHist(channels[0], channels[0]);

    merge(channels, ycrcb);

    cvtColor(ycrcb, result, COLOR_YCrCb2BGR);

    imshow("Equalized", result);
}


void lomo(int state, void* userData)
{
    Mat result;

    const double E = std::exp(1.0);
    // Create Lookup table for color curve effect
    Mat lut(1, 256, CV_8UC1);
    for (int i = 0; i < 256; i++)
    {
        float x = (float)i / 256.0;
        lut.at<uchar>(i) = cvRound(256 * (1 / (1 + pow(E, -((x - 0.5) / 0.1)))));
    }

    // Split the image channels and apply curve transform only to red channel
    std::vector<Mat> bgr;
    split(img, bgr);
    LUT(bgr[2], lut, bgr[2]);
    merge(bgr, result);


    Mat halo(img.rows, img.cols, CV_32FC3, Scalar(0.3, 0.3, 0.3));
    circle(halo, Point(img.cols / 2, img.rows / 2), img.cols / 3, Scalar(1, 1, 1), -1);
    blur(halo, halo, Size(img.cols / 3, img.cols / 3));

    // Convert the result to float to allow multiply by 1 factor
    Mat resultf;
    result.convertTo(resultf, CV_32FC3);

    multiply(resultf, halo, resultf);

    // convert to 8 bits
    resultf.convertTo(result, CV_8UC3);

    imshow("Lomograpy", result);
}


void cartoon(int state, void* userData)
{
    /** EDGES **/
    Mat imgMedian;
    medianBlur(img, imgMedian, 7);

    // Detect edges
    Mat imgCanny;
    Canny(imgMedian, imgCanny, 50, 150);

    // Dilate the edges
    Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
    dilate(imgCanny, imgCanny, kernel);

    // Scale edges values to 1 and invert values
    imgCanny = imgCanny / 255;
    imgCanny = 1 - imgCanny;

    // Use float values to allow multiply between 0 and 1
    Mat imgCannyf;
    imgCanny.convertTo(imgCannyf, CV_32FC3);

    // Blur the edgest to do smooth effect
    blur(imgCannyf, imgCannyf, Size(5, 5));

    /** COLOR **/
    // Apply bilateral filter to homogenizes color
    Mat imgBF;
    bilateralFilter(img, imgBF, 9, 150.0, 150.0);

    // truncate colors
    Mat result = imgBF / 25;
    result = result * 25;

    /** MERGES COLOR + EDGES **/
    // Create a 3 channles for edges
    Mat imgCanny3c;
    Mat cannyChannels[] = { imgCannyf, imgCannyf, imgCannyf };
    merge(cannyChannels, 3, imgCanny3c);

    Mat resultf;
    result.convertTo(resultf, CV_32FC3);

    // Multiply color and edges matrices
    multiply(resultf, imgCanny3c, resultf);

    resultf.convertTo(result, CV_8UC3);

    imshow("Result", result);
}


void sketch(int, void*)
{
    Mat result;
    cvtColor(img, result, COLOR_BGR2GRAY);
    Canny(result, result, 50, 150);
    result /= 255;
    result = 1 - result;
    result *= 255;
    imshow("Sketch", result);
}


void blurImage(int state, void* userData)
{
    applyBlur = (bool)state;
    applyFilters();
}

void sobelImage(int state, void* userData)
{
    applySobel = !applySobel;
    applyFilters();
}

void cannyImage(int state, void* userData)
{
    applyCanny = !applyCanny;
    applyFilters();
}
void laplaceImage(int state, void* userData)
{
    applyLaplace = !applyLaplace;
    applyFilters();
}

void applyFilters() 
{
    Mat result, result_gray;
    img.copyTo(result);
    cvtColor(result, result_gray, COLOR_BGR2GRAY);

    if (applySobel) {
        Sobel(result, result, CV_8U, 1, 1);
    }
    if (applyCanny) {
        Canny(result_gray, result, 50, 150);
    }
    if (applyLaplace) {
        Laplacian(result_gray, result, CV_8U);
    }
    if (applyBlur) {
        blur(result, result, Size(5, 5));
    }
    
    imshow("Lena", result);
}

/* Detect human faces */
void faceDetect(int, void*)
{
    Mat result;
    img.copyTo(result);
    CascadeClassifier faceDectetor("../data/haarcascade_frontalface_alt.xml");
    std::vector<Rect> faces;
    faceDectetor.detectMultiScale(result, faces);
    for (const Rect face : faces) {
        rectangle(result, face, Scalar(0, 0, 255), 2);
    }
    imshow("Lena", result);
}


/* change the contrast of image */
void contrastCallBack(int value, void* userData)
{  
    alpha = value / 50.f;
    adjustContrastAndBright();
}

/* change the brightness of image */
void brightnessCallback(int value, void* userData)
{   
    beta = value / 200.f;
    adjustContrastAndBright();
}

void adjustContrastAndBright() 
{
    Mat result;
    img.convertTo(result, CV_32F);
    result = result / 255;
    result.convertTo(result, CV_32F, alpha, beta);
    result = result * 255;
    result.convertTo(result, CV_8U);
    imshow("Lena", result);
}