#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

int main(int argc, char *argv[])
{
    Mat img = imread("arnold_schwarzenegger.jpg", CV_LOAD_IMAGE_COLOR);
    if(img.empty()) 
       return -1;
    namedWindow("arnold_schwarzenegger", CV_WINDOW_AUTOSIZE );
    imshow("arnold_schwarzenegger", img);
    waitKey(0);
    return 0;
}



