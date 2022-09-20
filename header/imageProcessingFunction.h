//
// Created by arthur on 19/09/2022.
//

#ifndef INDEXATIONIMAGE_IMAGEPROCESSINGFUNCTION_H
#define INDEXATIONIMAGE_IMAGEPROCESSINGFUNCTION_H

#include "../nrc/def.h"

void initMatrix(byte **matrix, long nrh, long nch);
void calculateGradient(const int mask[3][3], byte **intputMatrix, byte **outputMatrix, int nrl, int nrh, int ncl, int nch);
void graysScale(rgb8 **inputMatrix, byte **outputMatrix, int nrh, int nch);
void histogramme(byte **binarizedImg, int nrh, int nch, double *hist);
void histogrammeWithColor(rgb8 **binarizedImg, int nrh, int nch, double *histR, double * histG, double* histB);
double bhattacharyyaDistance(double* hist1,double* hist2);
void calculateColorRate(rgb8 **img, double* rateR, double *rateG, double *rateB, long nrl, long nrh, long ncl, long nch);
void pixelBorderDetection(byte** img, byte** output, long threshold, double* meanGradient, double* nbPixelBord, long nrl , long nrh,long ncl,long nch);
void sauvegardeHistogramme(double* histogramme,FILE* f);
int process(char *nomdossier);
int colored(double* histWithoutColor ,double* histColor,double threshold);
void calculateDistanceMatrix(double **disttable, int size, int formulaDistance);
double euclidienneDistance(double* hist1,double* hist2);
void sauvegardeTableHistogramme(double *histogramme, FILE *f,char * nom,int *index);
extern const int horizontal_gradient [3][3] ;
extern const int vertical_gradient[3][3];

#endif //INDEXATIONIMAGE_IMAGEPROCESSINGFUNCTION_H
