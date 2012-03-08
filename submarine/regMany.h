#include <iostream>
#include <math.h>


class regressor {

 public:

  void init(int,int);
  void input(float x[], float y[]);
  void get_cor(float corr[]);
  void get_meanX(float mean[]);
  void get_sumXY(float sumXY[]);
  void get_meanY(float mean[]);
  float output(); //calc and output
  float calc(); 

 private:

  int bX;//bins in X
  int bY;//bins in Y
  int N;//# of events
  float* sumX;
  float* sumY;
  float* sumXX;
  float* sumYY;
  float* sumXY;
  float* varXX;
  float* varYY;
  float* varXY;
  float* meanX;
  float* meanY;
  float* cor;//final correlations

};
