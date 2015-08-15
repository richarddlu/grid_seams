#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include "grid_seams.h"

using namespace std;
using namespace cv;

int main()
{
	VideoCapture cap("./traffic.avi");
	if(!cap.isOpened()){
		cout<<"cannot open file."<<endl;
		return -1;
	}else
		cout<<"open file successfully."<<endl;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);

	double fps = cap.get(CV_CAP_PROP_FPS);
	cout<<"fps: "<<fps<<endl;

	namedWindow("video", CV_WINDOW_AUTOSIZE);

	GridSeams gs;

	while(1){
		Mat frame;

		bool bSuccess = cap.read(frame);

		if(!bSuccess){
			cout<<"cannot read frame."<<endl;
			break;
		}

		resize(frame, frame, Size(640,360));

		gs.process(frame, 7, 7, 0.1, 0.0);
		gs.display(frame);

		imshow("video", frame);

		if(waitKey(1000/(int)fps) == 27){
			cout<<"close"<<endl;
			break;
		}
	}
	return 0;
}