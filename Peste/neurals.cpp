#include "neurals.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cv.h>
#include <highgui.h>
#include <ctime>
#include <queue>
#include <conio.h>

#define ALFA 0.01
#define THRESHOLD 2
#define THRESHOLD2 10

using namespace std;

const double lBound = 3, lBound2 = 10;
const int MAXIMAGES = 43, MAXBINS = 24;
int dataCount = 0;
int xStart = - 1, yStart;
int selectCount = 0;
IplImage *image, *image1;
int Dataa[1000000][25], DataTarget[1000000];

long double fRatio[24], bRatio[24];

void onMouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		xStart = x;
		yStart = y;
	}
	else if (event == CV_EVENT_MOUSEMOVE && xStart != -1 && (xStart != x || yStart != y))
	{
		cvCopyImage(image1, image);
		cvRectangle(image, cvPoint(xStart, yStart), cvPoint(x, y), cvScalar(123, 36, 112), 1);
		cvShowImage("Input", image);
	}
	else if (event == CV_EVENT_RBUTTONDOWN && xStart != -1 && (xStart != x || yStart != y))
		xStart = -1;
	else if (event == CV_EVENT_LBUTTONUP && xStart != -1)
	{
		int target;
		cin >> target;
		cvRectangle(image, cvPoint(xStart, yStart), cvPoint(x, y), cvScalar(23, 86, 12), -1);
		cvShowImage("Input", image);
		fstream fout("sample1.txt");
		unsigned char* Data = (unsigned char*)image1 -> imageData;                    
		for (int i = xStart; i < x; i++)
			for (int j = yStart; j < y; j++)
			{
				int r = Data[j * image1 -> widthStep + i * image1 -> nChannels + 0];
				int g = Data[j * image1 -> widthStep + i * image1 -> nChannels + 1];
				int b = Data[j * image1 -> widthStep + i * image1 -> nChannels + 2];
				fout << r << " " << g << " " << b << " " << target << " ";
			}
		fout.close();
		xStart = -1;
	}
}

void mouseHandling()
{
	image = cvLoadImage("test.png");
	image1 = cvLoadImage("test.png");
	cvNamedWindow("Input");
	cvShowImage("Input", image);
	cvSetMouseCallback( "Input", onMouse, NULL);
	cvWaitKey(0);
}

bool pictTrain(int binColor[24], int target)//neuron Train
{
    long double ans = 0;
	for (int i = 0; i < MAXBINS; i++)
		ans += fRatio[i] * binColor[i];
	int ans2;
	if (ans >= THRESHOLD)
		ans2 = 1;
	else if (ans <= -THRESHOLD)
		ans2 = -1;
	else
		ans2 = 0;
	if (ans2 == target)
		return false;
//	cout << "Changed!" << endl;
	double error = (target - ans2) / 2.0;
	for (int i = 0; i < MAXBINS; i++)
		fRatio[i] += error * binColor[i] * ALFA;
	return true;
}

bool pictTrain2(int binColor[24], int target) //neuron Train
{
    long double ans = 0;
	for (int i = 0; i < MAXBINS; i++)
		ans += bRatio[i] * binColor[i];
	int ans2;
	if (ans >= THRESHOLD2)
		ans2 = 1;
	else if (ans <= -THRESHOLD2)
		ans2 = -1;
	else
		ans2 = 0;

	if (ans2 == target)
		return false;
	//cout << "Changed!" << endl;
	double error = (target - ans2) / 2.0;
	for (int i = 0; i < MAXBINS; i++)
		bRatio[i] += error * binColor[i] * ALFA;
	return true;
}

int neuron(int *binColor)
{
	long double ans = 0;
	for (int i = 0; i < 24; i++)
		ans += binColor[i] * fRatio[i];
	if (ans >= lBound)
		return 1;
	return -1;
}

int neuron2(int *binColor)
{
	long double ans = 0;
	for (int i = 0; i < 24; i++)
		ans += binColor[i] * bRatio[i];
	if (ans >= lBound2)
		return 1;
	return -1;
}

void random()
{
    int d = 123789;
	srand(time(0));
	for (int i = 0; i < 24; i++)
	{
		fRatio[i] = rand() / (double)RAND_MAX * 2 - 1;
		bRatio[i] = rand() / (double)RAND_MAX * 2 - 1;
	}
}

void trainFromFile ()
{
	int it = 0;
	while (it++ < 500)
		for (int i = 0; i < dataCount; i++)
		{
			if (DataTarget[i] == 1)
			{
				pictTrain(Dataa[i], 1);
				pictTrain2(Dataa[i], -1);
			}
			else if (DataTarget[i] == 2)
			{
				pictTrain(Dataa[i], -1);
				pictTrain2(Dataa[i], 1);
			}
			else
			{
				pictTrain(Dataa[i], -1);
				pictTrain2(Dataa[i], -1);
			}
		}
}

void readFromFile ()
{
	ifstream fin ("sample1.txt");
	int r, g, b, target;
	while (fin >> r >> g >> b >> target && dataCount < 1000000)
	{
		binary (r, g, b, Dataa[dataCount]);
		DataTarget[dataCount] = target;
		dataCount++;
	}
	fin.close();
}

void train()
{
	bool checkFinishTrain = true;
	int t = 0;
    while (checkFinishTrain && t < 30)
    {
		checkFinishTrain = false;
		t++;
		if (t % 10 == 0)
		{
			cout << t << endl;
			cout.setf(ios::showpoint | ios::fixed);
			cout.precision(2);
			for (int i = 0; i < 24; i++)
			{
				cout.width(5);
				cout << fRatio[i] << "	";
			}
			cout << endl << "--------------------" << endl;
		}
        for (int it = 19; it < MAXIMAGES; it++)
        {
            string filename = "IMAG00";
            filename += char(it/10 + '0');
			filename += char(it%10 + '0');
            filename += ".jpg";
            //cout << filename.c_str() << endl;
            IplImage *src = cvLoadImage(filename.c_str());
            //cvShowImage("Salam", src);
			int binColor[24];
            unsigned char* Data = (unsigned char*)src -> imageData;
            for (int i = 0; i < src->width; i++)
                for (int j = 0; j < src->height; j++)
                {
                    int r = Data[j * src -> widthStep + i * src -> nChannels + 0];
                    int g = Data[j * src -> widthStep + i * src -> nChannels + 1];
                    int b = Data[j * src -> widthStep + i * src -> nChannels + 2];
                    if (r+g+b >= 30)
                    {
						binary(r, g, b, binColor);
                        checkFinishTrain |= pictTrain(binColor,1);
                    }
                }
            cvReleaseImage(&src);
        }
		for (int it = 0; it < 1; it++)
        {
            string filename = "chert";
            filename += char(it + '0');
            filename += ".bmp";
            //cout << filename.c_str() << endl;
            IplImage *src = cvLoadImage(filename.c_str());
            //cvShowImage("Salam", src);
			int binColor[24];
            unsigned char* Data = (unsigned char*)src -> imageData;
            for (int i = 0; i < src->width; i++)
                for (int j = 0; j < src->height; j++)
                {
                    int r = Data[j * src -> widthStep + i * src -> nChannels + 0];
                    int g = Data[j * src -> widthStep + i * src -> nChannels + 1];
                    int b = Data[j * src -> widthStep + i * src -> nChannels + 2];
                    if (!(r == 0 && g == 0 && b == 0))
                    {
						binary(r, g, b, binColor);
						checkFinishTrain |= pictTrain(binColor,0);
                    }
                }
            cvReleaseImage(&src);
        }
    }
}

void test()
{
	/*int t1 = 0, t_1 = 0;
    ofstream fout("out.out");
    for (int it = 0; it < MAXIMAGES; it++)
    {
        string filename = "peste";
        filename += char(it + '0');
        filename += ".bmp";
        //cout << filename.c_str() << endl;
        IplImage *src = cvLoadImage(filename.c_str());
        //cvShowImage("Salam", src);
		int nc = src -> nChannels;
        for (int j = 0; j < src->height; j++)
		{
			unsigned char* Data = (unsigned char*)(src -> imageData + j * src -> widthStep);
			for (int i = 0; i < src->width; i++)
            {
                int r = Data[i * nc + 0];
                int g = Data[i * nc + 1];
                int b = Data[i * nc + 2];
                if (!(r == 0 && g == 0 && b == 0))
                {
					int binColor[24];
					binary(r, g, b, binColor);
					if (neuron(binColor) > 0)
						t1++;
					else
						t_1++;
					fout << neuron(binColor) << endl;
				}
            }
		}
        cvReleaseImage(&src);
    }
	cout << t1 << " " << t_1 << endl;
	t1 = t_1 = 0;
	for (int it = 0; it < 1; it++)
    {
        string filename = "chert";
        filename += char(it + '0');
        filename += ".bmp";
        //cout << filename.c_str() << endl;
        IplImage *src = cvLoadImage(filename.c_str());
        //cvShowImage("Salam", src);
        unsigned char* Data = (unsigned char*)src -> imageData;
		cout << src->width << " " << src->height << " " << src->width * src->height << endl;
        for (int i = 0; i < src->width; i++)
            for (int j = 0; j < src->height; j++)
            {
                int r = Data[j * src -> widthStep + i * src -> nChannels + 0];
                int g = Data[j * src -> widthStep + i * src -> nChannels + 1];
                int b = Data[j * src -> widthStep + i * src -> nChannels + 2];            
				int binColor[24];
				binary(r, g, b, binColor);
				if (neuron(binColor) < 0)
					t1++;
				else
					t_1++;
				fout << neuron(binColor) << endl;				
            }
        cvReleaseImage(&src);
    }
	cout << t1 << " " << t_1 << endl;
    fout.close();*/
	IplImage *src = cvLoadImage("IMAG0037.jpg");
	cvShowImage(" ", src);
	unsigned char* Data = (unsigned char*)src -> imageData;
	for (int j = 0; j < src->height; j++)
		for (int i = 0; i < src->width; i++)
        {
			int r = Data[j * src -> widthStep + i * src -> nChannels + 0];
            int g = Data[j * src -> widthStep + i * src -> nChannels + 1];
            int b = Data[j * src -> widthStep + i * src -> nChannels + 2];            
			int binColor[24];
			binary(r, g, b, binColor);
			if (neuron(binColor) > 0)
			{
				Data[j * src -> widthStep + i * src -> nChannels + 2] = 255;
				Data[j * src -> widthStep + i * src -> nChannels + 1] = Data[j * src -> widthStep + i * src -> nChannels + 0] = 0;
			}
		}
	delete Data;
	//cvShowImage("Test", src);
	cvWaitKey(50000);
	cvReleaseImage(&src);
}

void readFile()
{
    ifstream fin("data.txt");
    for (int i = 0; i < 24; i++)
        fin >> fRatio[i];
	for (int i = 0; i < 24; i++)
        fin >> bRatio[i];
	fin.close();
}

void writeFile()
{
    ofstream fout("data.txt");
	for (int i = 0; i < 24; i++)
		fout << fRatio[i] << " ";
	fout << endl;
	for (int i = 0; i < 24; i++)
		fout << bRatio[i] << " ";
	fout << endl;
    fout.close();
}

void binary (int r, int g, int b, int *a)
{
	memset(a, 0, 24 * (sizeof (int)));
	int i = 0;
	r = r + g * 256 + b * 256 * 256;
	while (r > 0)
	{
		a[i] = r % 2 * 2 - 1;
		r /= 2;
		i++;
	}
}

int color(int b, int g, int r)
{
    if (b == 0 && g == 0 && r == 255)//poost
        return 1;
    else if (b == 255 && g == 0 && r == 0)//maghz
        return 2;
    else
        return 0;
}

bool mark[2000][2000] = {};
queue<pair<int, int> > q;

void enque(int x, int y, int &t)
{
    mark[x][y] = true;
    q.push(make_pair(x, y));
    t++;
}

int BFS(IplImage *img, int i, int j, bool t)
{
    queue <pair <int, int> >q;
    int ans = 0;
    enque(i, j, ans);
	unsigned char* Data = (unsigned char*)img -> imageData;
    while (!q.empty())
    {
        pair <int, int> qf = q.front();
        q.pop();
		int x = qf.first, y = qf.second;
        for (int i = -1; i <= 1; i += 2)
            for (int j = -1; j <= 1; j += 2)
            {
                int b = Data[y+i * img-> widthStep + x+j * img-> nChannels + 0];// x & y should change
                int g = Data[y+i * img-> widthStep + x+j * img-> nChannels + 1];
                int r = Data[y+i * img-> widthStep + x+j * img-> nChannels + 2]; 
                if (!mark[qf.first][qf.second] && color(b, g, r) - 1 == t)
                    enque(qf.first + i, qf.second + j, ans);
            }
    }
    return ans;
}

bool isGood(IplImage *img)
{
	int n = img->width, m = img->height;// width & height
    int maxPoost = 0, maxMaghz = 0;
	unsigned char* Data = (unsigned char*)img -> imageData;
    for (int y = 0; y < m; y++)
        for (int x = n / 3; x < 2 * (n / 3); x++)
        {
            int b = Data[y * img-> widthStep + x * img-> nChannels + 0];// x & y should change
            int g = Data[y * img-> widthStep + x * img-> nChannels + 1];
            int r = Data[y * img-> widthStep + x * img-> nChannels + 2]; 
            if (!mark[x][y] && color(b, g, r) == 1)
                maxPoost = max(maxPoost, BFS(img, x, y, 0));
            else if (!mark[x][y] && color(b, g, r) == 2)
                maxMaghz = max(maxMaghz, BFS(img, x, y, 1));
        }
    if (maxMaghz * 5 > maxPoost)
        return true;
    return false;
}


void videoTest()
{
	CvCapture *cap = cvCaptureFromFile("VIDEO0003.avi"); 
	int frameW = (int) cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int) cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT);
	IplImage *src = cvCreateImage(cvSize(1920, 1080), 8, 3);
	//int tMax;
	while (src = cvQueryFrame(cap))
	{
		cvShowImage("Source", src);
		IplImage *cop = cvCreateImage(cvSize(src->width, src->height), src->depth, src->nChannels);
		cvCopyImage(src, cop);
		unsigned char* Data = (unsigned char*)cop -> imageData;
		for (int j = 0; j < cop->height; j++)
			for (int i = 0; i < cop->width; i++)
			{
				int r = Data[j * cop -> widthStep + i * cop -> nChannels + 0];
				int g = Data[j * cop -> widthStep + i * cop -> nChannels + 1];
				int b = Data[j * cop -> widthStep + i * cop -> nChannels + 2];            
				int binColor[24];
				binary(r, g, b, binColor);
				if (neuron(binColor) > 0)
				{
					Data[j * cop -> widthStep + i * cop -> nChannels + 2] = 255;
					Data[j * cop -> widthStep + i * cop -> nChannels + 1] = Data[j * cop -> widthStep + i * cop -> nChannels + 0] = 0;
				}
				else if (neuron2(binColor) > 0)
				{
					Data[j * cop -> widthStep + i * cop -> nChannels + 0] = 255;
					Data[j * cop -> widthStep + i * cop -> nChannels + 2] = Data[j * cop -> widthStep + i * cop -> nChannels + 1] = 0;
				}
			}
		cout << isGood(cop) << endl;
		cvShowImage(" ", cop);
		cvWaitKey(100);
		/*if (kbhit() == 'p')
			return;
		//*/
	}
	cout << endl;
}

bool isRed(int x, int y, IplImage* img)
{
	unsigned char* Data = (unsigned char*)img->imageData;
	int b = Data[y * img-> widthStep + x * img-> nChannels + 0];
	int g = Data[y * img-> widthStep + x * img-> nChannels + 1];
	int r = Data[y * img-> widthStep + x * img-> nChannels + 2]; 
	return (r == 255 && g == 0 && b == 0) || (r == 0 && g == 0 && b == 255);
}

bool isInside(int x, int y, int width, int height)
{
	return x < width && x >= 0 && y < height && y >= 0;
}
// end