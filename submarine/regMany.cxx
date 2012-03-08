#include "regMany.h"

void regressor::init(int xBins,int yBins)
{ 
  N=0;
  bX=xBins;
  bY=yBins;
  sumX=new float[xBins];
  sumY=new float[yBins];
  sumXX=new float[xBins];
  sumYY=new float[yBins];
  sumXY=new float[xBins*yBins];

  meanX=new float[xBins];
  meanY=new float[yBins];
  varXX=new float[xBins];
  varYY=new float[yBins];
  varXY=new float[xBins*yBins];
  cor=new float[xBins*yBins];


  for(int aa=0;aa<bX;aa++)
    {
      sumX[aa]=0;
      sumXX[aa]=0;
    }

  for(int bb=0;bb<bY;bb++)
    {
      sumY[bb]=0;
      sumYY[bb]=0;
    }
  for(int aa=0;aa<bX;aa++)
    {
      for(int bb=0;bb<bY;bb++)
	{
	  sumXY[aa*bY+bb]=0;
	}
    }

  return;
}


void regressor::get_cor(float corr[]){
  for(int aa=0;aa<bX;aa++){for(int bb=0;bb<bY;bb++){corr[aa*bY+bb]=cor[aa*bY+bb];}}
  return;
}

void regressor::get_sumXY(float out[]){
  for(int aa=0;aa<bX;aa++){for(int bb=0;bb<bY;bb++){out[aa*bY+bb]=sumXY[aa*bY+bb];}}
  return;
}

void regressor::get_meanX(float out[]){
  for(int aa=0;aa<bX;aa++){out[aa]=meanX[aa];}
  return;
}

void regressor::get_meanY(float out[]){
  for(int aa=0;aa<bY;aa++){out[aa]=meanY[aa];}
  return;
}

void regressor::input(float x[], float y[])
{
  for(int bb=0;bb<bY;bb++){sumY[bb]+=y[bb];sumYY[bb]+=y[bb]*y[bb]; }
  for(int aa=0;aa<bX;aa++){
    sumX[aa]+=x[aa];sumXX[aa]+=x[aa]*x[aa];
    for(int bb=0;bb<bY;bb++){
      sumXY[aa*bY+bb]+=x[aa]*y[bb];
    }
  }

  N++;
  return;
}

float regressor::calc()
{
  float nE=N;
  for(int aa=0;aa<bX;aa++){
    meanX[aa]=sumX[aa]/nE;
    varXX[aa]=sumXX[aa]-meanX[aa]*meanX[aa]*nE;
  }

  for(int bb=0;bb<bY;bb++){
    meanY[bb]=sumY[bb]/nE;
    varYY[bb]=sumYY[bb]-meanY[bb]*meanY[bb]*nE;
  }

  for(int aa=0;aa<bX;aa++){
    for(int bb=0;bb<bY;bb++){
      int xyB=aa*bY+bb;

      varXY[xyB]=sumXY[xyB]-meanX[aa]*meanY[bb]*nE;
      cor[xyB]=0;
      if(varXX[aa]>0 && varYY[bb]>0){ 
	cor[xyB]=varXY[xyB];
	cor[xyB]/=sqrt(varXX[aa]*varYY[bb]);
      }
    }}

  return 0;

}

float regressor::output()
{
  float nE=N;


  std::cout<<std::endl<<"Calculating averages and variances."<<std::endl<<std::endl;

  for(int aa=0;aa<bX;aa++){
    meanX[aa]=sumX[aa]/nE;
    std::cout<<"mean X["<<aa<<"]: "<<meanX[aa]<<std::endl;
    varXX[aa]=sumXX[aa]-meanX[aa]*meanX[aa]*nE;
    std::cout<<"var X["<<aa<<"]: "<<varXX[aa]<<std::endl;
  }

  std::cout<<std::endl;

  for(int bb=0;bb<bY;bb++){
    meanY[bb]=sumY[bb]/nE;
    std::cout<<"mean Y["<<bb<<"]: "<<meanY[bb]<<std::endl;
    varYY[bb]=sumYY[bb]-meanY[bb]*meanY[bb]*nE;
    std::cout<<"var Y["<<bb<<"]: "<<varYY[bb]<<std::endl;
  }

  std::cout<<std::endl<<"Calculating covariances and correlations."<<std::endl<<std::endl;

  for(int aa=0;aa<bX;aa++){
    for(int bb=0;bb<bY;bb++){

      std::cout<<"Working on ("<<aa<<","<<bb<<")"<<std::endl;

      int xyB=aa*bY+bb;
      std::cout<<"xyBins: "<<xyB<<std::endl;

      varXY[xyB]=sumXY[xyB]-meanX[aa]*meanY[bb]*nE;
      std::cout<<"cov XY: "<<varXY[xyB]<<"\tsumXY: "<<sumXY[xyB]<<"\tmeanX: "
	       <<meanX[aa]<<"\tmeanY: "<<meanY[bb]<<std::endl;
      cor[xyB]=0;
      if(varXX[aa]>0 && varYY[bb]>0){ 
	cor[xyB]=varXY[xyB];
	cor[xyB]/=sqrt(varXX[aa]*varYY[bb]);
      }
      std::cout<<"R of XY: "<<cor[xyB]<<std::endl<<std::endl;
    }}

  std::cout<<"Stats calculated on "<<N<<" events."<<std::endl;

  return 0;
}

/*
  int main()
  {
  regressor* test=new regressor();

  const int Xbins=3;
  const int Ybins=3;
  float x[Xbins];
  float y[Ybins];

  test->init(Xbins,Ybins);

  x[0]=1;x[1]=-1;x[2]=3;
  y[0]=1;y[1]=-1;y[2]=3;
  test->input(x,y);
  x[0]=2;x[1]=-2;x[2]=0;
  y[0]=2;y[1]=-2;y[2]=0;
  test->input(x,y);
  x[0]=3;x[1]=-3;x[2]=2;
  y[0]=3;y[1]=-3;y[2]=2;
  test->input(x,y);
  x[0]=1;x[1]=-1;x[2]=10;
  y[0]=1;y[1]=-1;y[2]=10;
  test->input(x,y);

  x[0]=40;x[1]=-40;x[2]=0;
  y[0]=40;y[1]=-40;y[2]=0;
  test->input(x,y);




  test->output();

  return 0;
  }
*/

