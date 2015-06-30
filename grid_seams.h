#ifndef __GRID_SEAMS_H__
#define __GRID_SEAMS_H__

#include <opencv2/opencv.hpp>

using namespace cv;

class GridSeams {

private:

	Size size;

	Mat imgGray;

	Mat gradientMap;

	Mat energyMap;

	int SX, SY;

	int M, N;	// number of vertical and horizontal seams

	double w;

	Mat seamMapV;

	Mat seamMapH;

	double f(int x, int y);
	double g(int x, int S);

	void calculateEnergyMap(int S, int numSeams);

	void getSeamMap(int S, int numSeams, int direction);

	// direction - 0 for vertical, 1 for horizontal
	void processOne(int S, int numSeams, int direction);

	void getLabelMap();

public:

	Mat seamMap;

	Mat labelMap;

	// M - number of vertical seams
	// N - number of horizontal seams
	// w - weight
	void process(const Mat& img, Mat& spMap, int SX, int SY, double w);

	void display(const Mat& img);

};

#endif
