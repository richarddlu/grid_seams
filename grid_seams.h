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

	void calculateEnergyMap(int S, int numSeams, int direction);

	void getSeamMap(int S, int numSeams, int direction);

	// direction - 0 for vertical, 1 for horizontal
	void processOne(int S, int numSeams, int direction);

	void getLabelMap();

	// video coherence
	Mat seamMapVLast;
	Mat seamMapHLast;

	double u;

	// Mat preSeamEnergyMapV;
	// Mat preSeamEnergyMapH;

	// direction - 0 for vertical, 1 for horizontal
	double l(int x, int y, int direction);

	void getPreSeamEnergyMap();

public:

	Mat seamMap;

	Mat labelMap;

	Mat preSeamEnergyMapV;
	Mat preSeamEnergyMapH;

	// SX - number of pixels per virtical grid
	// SY - number of pixels per horizontal grid
	// w - grid weight
	// u - previous seams weight
	void process(const Mat& img, int SX, int SY, double w, double u);

	void display(const Mat& img);

};

#endif
