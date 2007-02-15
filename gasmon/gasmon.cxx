// $Id: gasmon.cxx,v 1.2 2007-02-15 16:08:12 jschamba Exp $
// $Log: not supported by cvs2svn $
// Revision 1.1  2007/02/15 15:52:47  jschamba
// original gas monitoring files from Frank Geurts
//
// Revision 1.2  2003/09/30 20:41:50  geurts
// Added CVS keywords
//
//
//   TOFr Gas Monitoring v1.0  -- Feb.2002
//    a collection of test, calibration and readout routines
//
//   Usage:
//     .interactive mode (menu drive): gasmon -i 
//     .create/append to  calibration data file: gasmon -cal
//     .(re-)calculate calibration functions and update results file: gasmon -recal
//     .test readout on all channels: gasmon -test
//     .single readout, update slowcontrols event file: gasmon -readout
//
//   To compile:
//      gcc -Wall gasmon.cxx -o gasmon /usr/lib/libgsl.a
//
//
//--------------------------------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <time.h>
#include <gsl/gsl_fit.h>
#include "pci-das08.h"



//--------------------------------------------------------------------//
//-- global variables
const int MAXDEV(8);
const int MAXSAMPLES = 1000;
const int MAXGAS = 3;
int fdADC[MAXDEV];
int mGasAdc[MAXGAS],maxGasFlow[MAXGAS];
enum gastype {FREON=0, ISOBUTANE=1, SF6=2};
double mc0[MAXDEV], mc1[MAXDEV], mcov00[MAXDEV],
    mcov01[MAXDEV], mcov11[MAXDEV], mchisq[MAXDEV];

bool Verbose;
const float VMAX = 5.;


//--------------------------------------------------------------------//
void openDevices()
{
  // open all MAXDEV(=8) devices

  char str[80],deviceName[80];
  for (int i=0;i<MAXDEV;i++){
    sprintf(deviceName,"/dev/pci-das08_adc0%d",i);
    fdADC[i] = open(deviceName, ADC_SOFT_TRIGGER);
    if (fdADC[i]<0){
      perror(str);
      printf("error opening device %s\n", deviceName);
      exit(2);
    }
  }
}


//--------------------------------------------------------------------//
void closeDevices()
{
  for (int i=0;i<MAXDEV;i++)
    close(fdADC[i]);
}


//--------------------------------------------------------------------//
void readConfigFile(){

  if (Verbose) printf("#-- reading config file --#\n");
  char str[80];
  FILE *configFile;
  if ((configFile=fopen("/home/tof/gasmon/gasmon.conf","r")) ==NULL){
    printf("Unable to open configuration file");
    exit(0);
  }

  // ADC# for gas (1=freon, 2=isobutane, 3=SF6) 
  fscanf(configFile,"%d,%d,%d %s", &mGasAdc[FREON],&mGasAdc[ISOBUTANE],&mGasAdc[SF6],str);

  // max gas flow 
  fscanf(configFile,"%d,%d,%d %s", &maxGasFlow[FREON],&maxGasFlow[ISOBUTANE],&maxGasFlow[SF6],str);

#ifdef DEBUG
  printf("%s\t: freon @adc%d isobutane @adc%d SF6 @adc%d\n",
	 str,mGasAdc[FREON],mGasAdc[ISOBUTANE],mGasAdc[SF6]);
  printf("%s\t: freon %dccm isobutane %dccm SF6 %dccm\n",
	 str,maxGasFlow[FREON],maxGasFlow[ISOBUTANE],maxGasFlow[SF6]);
#endif
  
  fclose(configFile);
}


//--------------------------------------------------------------------//
void testADC(){

  // screen dump raw adc and calibrated voltages for all channels

  WORD value[10];
  WORD tmp[MAXDEV];

  while (1){
    for (int i=0;i<MAXDEV;i++){
      read(fdADC[i], value, 1);
      printf("(%d)=%#x ", i, value[0]);
      tmp[i]=value[0];
    }
    printf("\n");
    for (int i=0;i<MAXDEV;i++)
      printf("(%d)=%5.3f ", i, (mc0[i] + mc1[i]*tmp[i]));
    printf("\n");
    usleep(100000);
  }
}


//--------------------------------------------------------------------//
void doCalibration(){

  // read calibration values and update calibration data file

  printf("#-- ADC Calibration --#\n");
  WORD value[1000];
  double Vset[MAXSAMPLES];
  double average[MAXSAMPLES];

  // Open calibration file in "append" mode
  FILE *calibFile;
  if ((calibFile=fopen("/home/tof/gasmon/gasmon.caldat","a")) ==NULL){
    printf("Cannot open calibration data file.\n");
    return;
  }

  // Loop over devices
  while(1){
    printf(" -which channel (<0 to exit): ");
    int adc;
    scanf("%d", &adc);
    if ((adc<0)||(adc>(MAXDEV-1))) break;

    // Loop over max samples (or break-out)
    int sample;
    for (sample=0;sample<MAXSAMPLES; sample++){
      printf("for adc %d enter Vset (<0 to exit): ", adc);
      float dummy;
      scanf("%e",&dummy);
      if (dummy<0) break;
      Vset[sample]=dummy;

      // perform "count" reads per sample and average
      const int count = 100;
      read(fdADC[adc], value, count);
      for (int k=0;k<count;k++) average[sample]+=value[k];
      average[sample] /= count;
      printf ("a%d-s%d: Vset=%f adc=%f \n",adc,sample+1,Vset[sample],average[sample]);
      fprintf (calibFile,"%d %f %f\n",adc,Vset[sample],average[sample]);
    }
    printf ("# adc %d\t#samples: %d\n",adc,sample);

    // do the least squares straight line fit based on this subset
    double c0, c1, cov00, cov01, cov11, chisq;
    gsl_fit_linear(average,1,Vset,1,sample,&c0,&c1,&cov00,&cov01,&cov11,&chisq);
    printf("# best fit: Y = %g + %g X\n", c0, c1);
    printf("# covariance matrix:\n");
    printf("# [ %g, %g\n#   %g, %g]\n", cov00, cov01, cov01, cov11);
    printf("# chisq = %g\n", chisq);
  }
  fclose(calibFile);
}


//--------------------------------------------------------------------//
void readCalibration(){
  if (Verbose) printf("#-- Read calibration results --#\n");
  int adc, calibrated=0, notCalibrated=0;
  FILE *calResultFile;
  calResultFile=fopen("/home/tof/gasmon/gasmon.calres","r");
  if (calResultFile){
    double c0,c1,cov00,cov01,cov11,chisq;
    for (int i=0;i<MAXDEV;i++){
      if (fscanf(calResultFile,"%d %lE %lE %lE %lE %lE %lE",
	     &adc,&c0,&c1,&cov00,&cov01,&cov11,&chisq)==EOF){
	printf("unexpected EOF in calibration results file\n");
	break;
      }
      mc0[adc]=c0;
      mc1[adc]=c1;
      mcov00[adc]=cov00;
      mcov01[adc]=cov01;
      mcov11[adc]=cov11;
      mchisq[adc]=chisq;      
      if (c0==0 && c1==0 && chisq==0) notCalibrated++;
      else calibrated++;
    }
    fclose(calResultFile);
  }
  else
    printf("unable to open calibration results file\n");

  if (Verbose) printf(" #calibrated: %d  #uncalibrated %d\n", calibrated, notCalibrated);
}


//--------------------------------------------------------------------//
void recalcCalibration(){

  // read and refit the calibration data, update the results

  printf("#-- Recalculate Calibration --#\n");
  double Vset[MAXSAMPLES],sample[MAXSAMPLES];

  //-- get the previous results
  readCalibration();

  //-- process the calibration data and update ADC results
  while(1){
    // Always reopen calibration file in "read" mode
    FILE *calibDataFile;
    if ((calibDataFile=fopen("/home/tof/gasmon/gasmon.caldat","r")) ==NULL){
      printf("Cannot open calibration data file.\n");
      return;
    }
    printf(" -which channel (<0 to exit): ");
    int adc;
    scanf("%d", &adc);
    if ((adc<0)||(adc>(MAXDEV-1))) break;

    int mAdc,i=0;
    float mVset,mReadout;
    while ((fscanf(calibDataFile,"%d %f %f",&mAdc,&mVset,&mReadout)!=EOF)
	   && i<MAXSAMPLES){
      if (mAdc==adc){
	Vset[i]=(double)mVset;
	sample[i]=(double)mReadout;
	i++;
      }
    }

    // do the least squares straight line fit based on this subset
    gsl_fit_linear(sample,1,Vset,1,i,&mc0[adc],&mc1[adc],&mcov00[adc],&mcov01[adc],&mcov11[adc],&mchisq[adc]);
    printf("# best fit: Y = %g + %g X\n", mc0[adc], mc1[adc]);
    printf("# covariance matrix:\n");
    printf("# [ %g, %g\n#   %g, %g]\n", mcov00[adc], mcov01[adc], mcov01[adc], mcov11[adc]);
    printf("# chisq = %g\n", mchisq[adc]);

    fclose(calibDataFile);
  }

  // recreate the updated calibration results file
  FILE *calResultFile=fopen("/home/tof/gasmon/gasmon.calres","w");
  for (int i=0;i<MAXDEV;i++)
    fprintf(calResultFile,"%d %lE %lE %lE %lE %lE %lE \n",
	    i,mc0[i],mc1[i],mcov00[i],mcov01[i],mcov11[i],mchisq[i]);
  fclose(calResultFile);
  printf(" updated calibration results file\n");
}



//--------------------------------------------------------------------//
void readOut(){

  // do a single readout of the gas channels, calculate gas flow
  // and ratio and update the slowcontrols event file

  const int count = 10;
  WORD value[count];
  float gasFlow[MAXGAS];

  //-- explicitly readout the 3 gas types
  for (int i=0;i<MAXGAS;i++){
    // read 'count' samples and average
    read(fdADC[mGasAdc[i]], value, count);
    float average =0;
    for (int k=0;k<count;k++) average+=value[i]/count;
    //gasFlow[i] = mc0[mGasAdc[i]] + mc1[mGasAdc[i]]*average;
    gasFlow[i] = (mc0[mGasAdc[i]] + mc1[mGasAdc[i]]*average)*maxGasFlow[i]/VMAX;
  }

  // get the current local time and format it ...
  time_t t;
  time(&t);
  tm* tm=localtime(&t);

  // normalize these ratios to the readout voltages, not the flow
  //  isobutane and sf6 should be in slave-mode so these ratios
  //  typically ought to be 1.0
  float ratioIsobutane = (gasFlow[ISOBUTANE]/gasFlow[FREON])*(maxGasFlow[FREON]/maxGasFlow[ISOBUTANE]);
  float ratioSf6 = (gasFlow[SF6]/gasFlow[FREON])*(maxGasFlow[FREON]/maxGasFlow[SF6]);

  printf("%04d-%02d-%02d %02d:%02d:%02d  ",
	 (1900+tm->tm_year),(1+tm->tm_mon),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
  printf("#-- GasFlow: %4.1fccm %4.2fccm %4.2fccm #-- Ratios: %4.2f %4.2f\n",
	 gasFlow[FREON],gasFlow[ISOBUTANE],gasFlow[SF6],ratioIsobutane,ratioSf6);

  // Update Slow-Controls event file
  FILE *SlowControlsEventFile=fopen("/TOFrSC/tofrgas.evt","w");
  if (!SlowControlsEventFile){
    printf("CRITICAL ERROR: UNABLE TO OPEN SLOWCONTROLS EVENT FILE\n");
    exit(0);
  }
  fprintf(SlowControlsEventFile,"%ld %5.1f %5.2f %5.2f %5.2f %5.2f\n",
	  (long)t,gasFlow[FREON],gasFlow[ISOBUTANE],gasFlow[SF6],
	  ratioIsobutane,ratioSf6);
  int err=fclose(SlowControlsEventFile);
  if (err) perror("CRITICAL ERROR: ");

}


//--------------------------------------------------------------------//
void interactiveMenu(){

  // present all functions as menu items (of some you might not return!)

  printf("#-- interactive mode --#\n");
  int choice;
  while(1){
    printf(" Menu: 0-exit 1-test adc  2-calibrate 3-recalculate calib 4-readout\n");
    printf(" option: ");
    scanf("%d", &choice);
    switch(choice){
    case 0:
      return;
    case 1:
      testADC();
      break;
    case 2:
      doCalibration();
      break;
    case 3:
      recalcCalibration();
      break;
    case 4:
      readOut();
      break;
    default:
      break;
    }
  }
}

//--------------------------------------------------------------------//
void readCommandLine(int argc, char **argv){

  // allow all functions to be accessed through the commandline

  int i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-v") == 0) {
      Verbose=true;
    } else if (strcmp(argv[i], "-i") == 0) {
      return;
    } else if (strcmp(argv[i], "-test") == 0) {
      testADC();
      exit(1);
    } else if (strcmp(argv[i], "-cal") == 0) {
      doCalibration();
      exit(1);
    } else if (strcmp(argv[i], "-recal") == 0) {
      recalcCalibration();
      exit(1);
    } else if (strcmp(argv[i], "-readout") == 0) {
      readOut();
      exit(1);
    } else {
      printf("Unknown option: %s",argv[i]);
      exit(0);
    }
    i++;
  }
}


//--------------------------------------------------------------------//
int main(int argc, char **argv)
{
  Verbose = false;
  if (Verbose) printf("+---- TOFr GAS Monitor v1.1 (%s - %s)\n" ,__DATE__,__TIME__);

  openDevices();
  readConfigFile();
  readCalibration();

  readCommandLine(argc,argv);
  interactiveMenu();

  closeDevices();
  return(1);
}


