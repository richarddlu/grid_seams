#include "grid_seams.h"

#include <iostream>
#include <ctime>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat img = imread("lena.jpg");

	GridSeams gs;
	clock_t t;
	t = clock();
	gs.process(img, 30, 30, 0.1);
	t = clock() - t;
	cout<<"Took "<<((float)t)/CLOCKS_PER_SEC<<" seconds"<<endl;

	gs.display(img);

	imshow("img", img);

	waitKey(0);

	return 0;
}
