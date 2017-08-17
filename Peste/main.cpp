#include <iostream>
#include <algorithm>
#include <fstream>
#include <cv.h>
#include <highgui.h>
#include <ctime>
#include "neurals.h"

using namespace std;

void edgeDetect(IplImage *in, IplImage *out);

int main()
{
	readFile();
	//random();
	//mouseHandling();
	readFromFile();
	trainFromFile();
	videoTest();
	//test();
	//fstream fout("sample1.txt", ios::out);
	writeFile();
	return 0;
    //train();
	/*IplImage *img = cvLoadImage("peste1.bmp"); 
	IplImage *out = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, img->nChannels);
	edgeDetect(img, out);
	cvShowImage("Canny", out);*/
}

void edgeDetect(IplImage *in, IplImage *out)
{
	cvCanny( in, out, 20*14, 40*14);
}

