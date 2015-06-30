#include "grid_seams.h"

#include <cmath>
#include <iostream>
#include <limits>

using namespace std;

double GridSeams::f(int x, int y) {
	return gradientMap.at<double>(y,x);
}

double GridSeams::g(int x, int S) {
	int d = (x+1) % S;
	d = abs(d - S/2);
	double r = (double)d / (S/2);
	return r;
}

void GridSeams::calculateEnergyMap(int S, int numSeams) {
	energyMap = Mat::zeros(size, CV_64F);	// initializing energy map

	/////////////////////////
	// Calculating energies
	/////////////////////////

	// first row
	for(int x = 0; x < size.width; x++) {
		if((x+1)%S == 0 && x+1 < S*numSeams) {	// on the border
			energyMap.at<double>(0,x) = numeric_limits<double>::max();
		} else {
			energyMap.at<double>(0,x) = f(x,0) + w*g(x,S);
		}
	}

	// Calculating energies of the rest using dynamic programming
	for(int y = 1; y < size.height; y++) {
		for(int x = 0; x < size.width; x++) {
			if((x+1)%S == 0 && x+1 < S*numSeams) {	// on the border
				energyMap.at<double>(y,x) = numeric_limits<double>::max();
			} else {
				double energy = f(x,y) + w*g(x,S);

				// Get the path with smallest energy
				double energyMin = numeric_limits<double>::max();
				for(int offset = -1; offset <= 1; offset++) {
					if(x+offset >= 0 && x+offset < size.width) {
						double energy = energyMap.at<double>(y-1,x+offset);
						if(energy < energyMin)
							energyMin = energy;
					}
				}

				energyMap.at<double>(y,x) = energy + energyMin;
			}
		}
	}
}

void GridSeams::getSeamMap(int S, int numSeams, int direction) {
	for(int i = 0; i < numSeams; i++) {	// find seams one by one
		int seamX; // seam x coordinate
		// find the end-point with minimum energy cost
		double xMin;
		double energyMin = numeric_limits<double>::max();
		if(i < numSeams - 1) {
			for(int x = i*S; x < (i+1)*S - 1; x++) {
				double energy = energyMap.at<double>(size.height-1, x);
				if(energy < energyMin) {
					energyMin = energy;
					xMin = x;
				}
			}
		} else {
			for(int x = i*S; x < size.width; x++) {
				double energy = energyMap.at<double>(size.height-1, x);
				if(energy < energyMin) {
					energyMin = energy;
					xMin = x;
				}
			}
		}
		seamX = xMin;
		if(direction == 0)
			seamMapV.at<uchar>(size.height-1, seamX) = 255;
		else
			seamMapH.at<uchar>(size.height-1, seamX) = 255;

		// find all points on a seam
		for(int y = size.height-2; y >= 0; y--) {
			double xMin;
			double energyMin = numeric_limits<double>::max();
			for(int offset = -1; offset <= 1; offset++) {
				if(i < numSeams - 1) {
					if(seamX+offset >= i*S && seamX+offset < (i+1)*S - 1) {
						double energy = energyMap.at<double>(y, seamX+offset);
						if(energy < energyMin) {
							energyMin = energy;
							xMin = seamX + offset;
						}
					}
				} else {
					if(seamX+offset >= i*S && seamX+offset < size.width) {
						double energy = energyMap.at<double>(y, seamX+offset);
						if(energy < energyMin) {
							energyMin = energy;
							xMin = seamX + offset;
						}
					}
				}
			}
			seamX = xMin;
			if(direction == 0)
				seamMapV.at<uchar>(y, seamX) = 255;
			else
				seamMapH.at<uchar>(y, seamX) = 255;
		}
	}
}

void GridSeams::processOne(int S, int numSeams, int direction) {
	calculateEnergyMap(S, numSeams);
	getSeamMap(S, numSeams, direction);
}

void GridSeams::getLabelMap() {
	Mat labelMapV(size, CV_32S);
	for(int y = 0; y < size.height; y++) {
		int currLabelV = 0;
		for(int x = 0; x < size.width; x++) {
			labelMapV.at<int>(y,x) = currLabelV;
			if(seamMapV.at<uchar>(y,x) != 0)
				currLabelV++;
		}
	}

	for(int x = 0; x < size.width; x++) {
		int currLabelH = 0;
		for(int y = 0; y < size.height; y++) {
			int labelV = labelMapV.at<int>(y,x);
			labelMap.at<int>(y,x) = currLabelH * (M+1) + labelV;
			if(seamMapH.at<uchar>(y,x) != 0)
				currLabelH++;
		}
	}
}

void GridSeams::process(const Mat& img, Mat& spMap, int SX, int SY, double w) {
	size = img.size();	// get image size

	this->w = w;	// weight

	// grid size
	// the last grid might be big
	this->SX = SX;
	this->SY = SY;
	
	// number of seams
	M = size.width / this->SX;
	N = size.height / this->SY;

	// the seams are marked as non-zero values
	seamMapV = Mat::zeros(img.size(), CV_8U);
	seamMapH = Mat::zeros(img.size(), CV_8U);
	seamMap = Mat::zeros(img.size(), CV_8U);

	// Calculate gray image
	cvtColor(img, imgGray, CV_BGR2GRAY);

	// Calculate gradient map
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	Sobel(imgGray, grad_x, CV_16S, 1, 0, 3, 1, 0);
	convertScaleAbs(grad_x, abs_grad_x);
	Sobel(imgGray, grad_y, CV_16S, 1, 0, 3, 1, 0);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradientMap);
	normalize(gradientMap, gradientMap, 0, 1, NORM_MINMAX, CV_64F);
	gradientMap = Mat::ones(size, CV_64F) - gradientMap;

	// Generate vertical seams
	processOne(this->SX, M, 0);

	// Generate horizontal seams
	transpose(gradientMap, gradientMap);
	transpose(energyMap, energyMap);
	transpose(seamMapH, seamMapH);
	size = Size(size.height, size.width);
	processOne(this->SY, N, 1);
	transpose(seamMapH, seamMapH);
	// transpose(energyMap, energyMap);	// efficiency consideration
	// transpose(seamMapH, seamMapH);	// efficiency consideration
	size = Size(size.height, size.width);

	// combine vertical and horizontal seam maps
	for(int y = 0; y < size.height; y++) {
		for(int x = 0; x < size.width; x++) {
			if(seamMapV.at<uchar>(y,x) != 0 || seamMapH.at<uchar>(y,x) != 0)
				seamMap.at<uchar>(y,x) = 255;
		}
	}

	// labeling
	labelMap = Mat::zeros(size, CV_32S);
	getLabelMap();
}

void GridSeams::display(const Mat& img) {
	Mat imgSeams = img.clone();
	MatIterator_<Vec3b> it1, end1;
	MatIterator_<uchar> it2, end2;
	for(it1 = imgSeams.begin<Vec3b>(), end1 = imgSeams.end<Vec3b>(), it2 = seamMap.begin<uchar>(), end2 = seamMap.end<uchar>(); it1 != end1 && it2 != end2; it1++, it2++) {
		if(*it2 != 0) {
			(*it1)[0] = 255;
			(*it1)[1] = 255;
			(*it1)[2] = 255;
		}
	}
	imshow("s", seamMap);
	imshow("seams", imgSeams);

	RNG rng(123457);
	vector<Vec3b> colors((M+1)*(N+1));
	for(int i = 0; i < (M+1)*(N+1); i++) {
		colors[i][0] = rng.uniform(0, 256);
		colors[i][1] = rng.uniform(0, 256);
		colors[i][2] = rng.uniform(0, 256);
	}
	Mat colorMap(size, CV_8UC3);
	for(int y = 0; y < size.height; y++) {
		for(int x = 0; x < size.width; x++) {
			int label = labelMap.at<int>(y,x);
			colorMap.at<Vec3b>(y,x) = colors[label];
		}
	}
	imshow("colorMap", colorMap);
}
