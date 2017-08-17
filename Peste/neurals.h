#include <cv.h>

bool pictTrain(int binColor[24], int target);

int neuron(int *binColor);

void binary (int r, int g, int b, int *a);

void train();

void test();

void random();

void readFile();

void writeFile();

void trainFromFile();

void onMouse(int event, int , int , int , void* );

void mouseHandling();

void videoTest();

void BFS(int, int, IplImage* img);

void readFromFile();