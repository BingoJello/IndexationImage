#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <dirent.h>
#include<string.h>
#include "./header/imageProcessingFunction.h"
#include "./nrc/nrio.h"
#include "./nrc/nralloc.h"
#include "header/mask.h"

/**
 * @brief Initialise une matrice à zero
 * @param matrix
 * @param nrh valeur la plus élevée de la ligne
 * @param nch valeur la plus élevée de la colonne
 */
void initMatrix(byte **matrix, long nrh, long nch) {
    for (int i = 0; i < nrh; i++) {
        for (int j = 0; j < nch; j++) {
            matrix[i][j] = 0;
        }
    }
}

/**
 * @brief Calcule le gradient de l'image selon un mask spécifié
 * @param mask masque
 * @param inputMatrix
 * @param outputMatrix
 * @param nrl nombre de ligne de l'image
 * @param nrh nombre de colonne de l'image
 * @param ncl valeur la plus petite de la colonne
 * @param nch valeur la plus élevée de la colonne
 */
void calculateGradient(const int mask[3][3], byte **intputMatrix, byte **outputMatrix, int nrl, int nrh, int ncl, int nch) {
    for (int i = nrl + 1; i < nrh - 1; i++) {
        for (int j = ncl + 1; j < nch - 1; j++) {
            int pixel_value = floor((mask[0][0] * intputMatrix[i - 1][j - 1] + mask[0][1] * intputMatrix[i - 1][j] +
                                     mask[0][2] * intputMatrix[i - 1][j + 1]
                                     + mask[1][0] * intputMatrix[i][j - 1] + mask[1][1] * intputMatrix[i][j] +
                                     mask[1][2] * intputMatrix[i][j + 1]
                                     + mask[2][0] * intputMatrix[i + 1][j - 1] + mask[2][1] * intputMatrix[i + 1][j] +
                                     mask[2][2] * intputMatrix[i + 1][j + 1]) / 9);

            if (pixel_value < 0) outputMatrix[i][j] = 0;
            else if (pixel_value > 255) outputMatrix[i][j] = 255;
            else outputMatrix[i][j] = pixel_value;
        }
    }
}

/**
 * Processus de niveau de gris d'une image
 * @param inputMatrix
 * @param outputMatrix
 * @param nrh nombre de ligne de l'image
 * @param nch nombre de colonne de l'image
 */
void graysScale(rgb8 **inputMatrix, byte **outputMatrix, int nrh, int nch) {
    for (int i = 1; i < nrh - 1; i++) {
        for (int j = 1; j < nch - 1; j++) {
            outputMatrix[i][j] = (inputMatrix[i][j].r + (int) inputMatrix[i][j].g + (int) inputMatrix[i][j].b) / 3;
        }
    }
}

/**
 *
 * @param binarizedImg l'image binarisée
 * @param nrh nombre de ligne de l'image
 * @param nch nombre de colonne de l'image
 * @param hist histogramme de l'image
 */
void histogramme(byte **binarizedImg, int nrh, int nch, double *hist) {
    int max = (nrh+1) * (nch+1);

    //initialisation de l'histogramme
    for (int i = 0; i < 256; i++) {
        hist[i] = 0.0;
    }

    for (int j = 0; j <= nrh; j++) {
        for (int k = 0; k <= nch; k++) {
            int index = binarizedImg[j][k];
            hist[index] = hist[index] + 1.0;
        }
    }
    //normalisation de l'histogramme
    for (int m = 0; m < 256; m++){
        hist[m] = hist[m] / (max * 1.0);
    }
}

/**
 *
 * @param binarizedImg Image binarisée
 * @param nrh nombre de ligne de l'image
 * @param nch nombre de colonne de l'image
 * @param histR
 * @param histG
 * @param histB
 */
void histogrammeWithColor(rgb8 **binarizedImg, int nrh, int nch, double *histR, double* histG, double* histB) {
    int max = (nrh+1) * (nch+1);

    //initialise l'histogramme
    for (int i = 0; i < 256; i++) {
        histR[i] = 0.0;
        histG[i] = 0.0;
        histB[i] = 0.0;
    }

    //classifie l'image suivant les différents niveaux de gris
    for (int j = 0; j <= nrh; j++) {
        for (int k = 0; k <= nch; k++) {
            int indexR = binarizedImg[j][k].r;
            int indexG = binarizedImg[j][k].g;
            int indexB = binarizedImg[j][k].b;
            histR[indexR] = histR[indexR] + 1.0;
            histG[indexG] = histG[indexG] + 1.0;
            histB[indexB] = histB[indexB] + 1.0;
        }
    }
    //normalisation de l'histogramme
    for (int m = 0; m < 256; m++) {
        histR[m] = histR[m] / (max * 1.0);
        histG[m] = histG[m] / (max * 1.0);
        histB[m] = histB[m] / (max * 1.0);
    }
}

/**
 * @brief Distance de Bhattachryaa entre deux histogrammes
 * @param hist1
 * @param hist2
 * @return distance
 */
double bhattacharyyaDistance(double *hist1, double *hist2) {
    double distance = 0.0;
    for (int i = 0; i < 256; i++) {
        distance += sqrt(hist1[i] * hist2[i]);
    }
    return -log(distance);
}

/**
 *
 * @param img
 * @param rateR taux de rouge
 * @param rateG taux de vert
 * @param rateB taux de bleu
 * @param nrl nombre de ligne de l'image
 * @param nrh nombre de colonne de l'image
 * @param ncl valeur la plus petite de la colonne
 * @param nch valeur la plus élevée de la colonne
*/

void calculateColorRate(rgb8 **img, double* rateR, double *rateG, double *rateB, long nrl, long nrh, long ncl, long nch) {
    (*rateR) = 0.0;
    (*rateG) = 0.0;
    (*rateB) = 0.0;
    for (int i = 0; i < nrh-nrl; i++) {
        for (int j = 0; j < nch-ncl; j++) {
            (*rateR) += img[i][j].r;
            (*rateG) += img[i][j].g;
            (*rateB) += img[i][j].b;
        }
    }
    int total=((*rateR) + (*rateG) + (*rateB));
    (*rateR) /= total;
    (*rateG) /= total;
    (*rateB) /= total;
}

/**
 * @brief Detection des bordures des pixels de l'image
 * @param img
 * @param output
 * @param threshold
 * @param meanGradient
 * @param nbPixelBord
 * @param nrl
 * @param nrh
 * @param ncl
 * @param nch
 */
void pixelBorderDetection(byte **img, byte **output, long threshold, double *meanGradient, double *nbPixelBord, long nrl,
                    long nrh, long ncl, long nch) {
    byte **gradient_x, **gradient_y;
    double normeGradient;
    *nbPixelBord = 0.0;
    *meanGradient = 0;

    gradient_x = bmatrix(nrl, nrh, ncl, nch);
    gradient_y = bmatrix(nrl, nrh, ncl, nch);

    initMatrix(gradient_x, nrh, nch);
    initMatrix(gradient_y, nrh, nch);

    calculateGradient(horizontal_gradient, img, gradient_x, nrl, nrh, ncl, nch);
    calculateGradient(vertical_gradient, img, gradient_y, nrl, nrh, ncl, nch);

    for (int i = 1; i < nrh - nrl; i++) {
        for (int j = 1; j < nch - ncl; j++) {
            normeGradient = sqrt(pow(gradient_x[i][j], 2) + pow(gradient_y[i][j], 2));
            *meanGradient += normeGradient;
            if (normeGradient >= threshold) {
                output[i][j] = 255;
                (*nbPixelBord)++;
            } else {
                output[i][j] = 0;
            }
        }
    }
    *meanGradient = *meanGradient / ((nrh - nrl - 1) * (nch - ncl - 1));
    (*nbPixelBord) = *meanGradient / ((nrh - nrl - 1) * (nch - ncl - 1));
}

/**
 *
 * @param histogramme histogramme à stocker
 * @param f fichier de sortie (.CSV)
 */
void sauvegardeHistogramme(double *histogramme, FILE *f) {
    for (int i = 0; i < 255; i++) {
        fprintf(f, "%lf|", histogramme[i]);
    }
    fprintf(f, "%lf", histogramme[255]);
}

/**
 * @brief Vérifie qu'une image est colorée ou non
 * @param histWithoutColor
 * @param histColor
 * @param threshold
 * @return
 */
int colored(double* histWithoutColor ,double* histColor,double threshold) {
    int i = 0;
    int j = 0;
    int color = 0;
    double distance = bhattacharyyaDistance(histWithoutColor,histColor);
    if(distance > threshold) color=1;

    return color;
}

/**
 *
 * @param inputma
 * @param size
 * @param formula (1 : euclidian distance, 2 : bhattacharyya distance
 */
void calculateDistanceMatrix(double **inputMatrix, int size, int formulaDistance) {
    FILE *outputMatrix;

    if(1 == formulaDistance) outputMatrix = fopen("D:/c++/IndexationImage/matrice_distance_euclidian.csv", "wa");
    else outputMatrix = fopen("D:/c++/IndexationImage/matrice_distance_bhattacharyya.csv", "wa");

    double distanceMatrix[size][size];
    double distance = 0.0;
    double maxDistance = 0.0;

    //calcul des distance entre les histogrammes des images
    for (int i = 0; i < size; i++) {
        double *hist = inputMatrix[i];
        for (int j = 0; j < size; j++) {
            double *hist2 = inputMatrix[j];
            if(1 == formulaDistance) distance = euclidienneDistance(hist, hist2);
            else distance = bhattacharyyaDistance(hist, hist2);

            if(distance > maxDistance)
                maxDistance = distance;
            distanceMatrix[i][j] = distance;
        }
    }

    //normalisation
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            distanceMatrix[i][j] = (distanceMatrix[i][j] * 100) / maxDistance;
        }
    }
    //sauvegarde
    for (int k = 0; k < size-1; k++) {
        fprintf(outputMatrix, "%d,", k + 1);
    }

    fprintf(outputMatrix, "%d\n", size);

    for (int l = 0; l < size; l++) {
        for (int m = 0; m < size-1; m++) {
            fprintf(outputMatrix, "%lf,", distanceMatrix[l][m]);
        }
        fprintf(outputMatrix, "%lf", distanceMatrix[l][size-1]);
        fprintf(outputMatrix, "\n");
    }
    fclose(outputMatrix);
}

double euclidienneDistance(double* hist1,double* hist2){
    double distance=0.0;
    for(int i=0;i < 256 ; i++){
        distance+=(hist1[i]-hist2[i])*(hist1[i]-hist2[i]);
    }
    distance=sqrt(distance);
    return distance;
}

void sauvegardeTableHistogramme(double *histogramme, FILE *f,char * nom,int *index) {
    for (int i = 0; i < 256; i++) {
        fprintf(f, "%d,%s.jpg,%d,%lf\n",*index,nom,i,histogramme[i]);
        (*index)++;
    }
}

/**
 * @brief Effectue le processus de traitement d'images
 * @param nomdossier
 * @return
 */
int process(char *myRepo) {
    DIR *rep = NULL;
    DIR *repcount = NULL;
    struct dirent *currentfile = NULL;
    int nbimg = 0;
    int size = 0;

    repcount = opendir(myRepo);
    if (repcount == NULL) {
        printf("Problème au niveau de la lecture du dossier");
    }

    currentfile = readdir(repcount);
    currentfile = readdir(repcount);
    while ((currentfile = readdir(repcount)) != NULL) {
        //vérification du type
        char fileName[255];
        sprintf(fileName, "%s\\%s", myRepo, currentfile->d_name);

        char *fileType = NULL;
        //récupération de l'extension
        fileType = strtok(currentfile->d_name, ".");
        fileType = strtok(NULL, ".");
        //Si PPM (image de couleur) alors on augmente la taille
        if (!strcmp(fileType, "ppm")) {
            size++;
        }
    }
    if (closedir(repcount) == -1) {
        printf("Problème au niveau de la fermeture du dossier");
        return 1;
    }

    double **matrix = (double **) malloc(size * sizeof(double *));

    for (int i = 0; i < size; i++) {
        matrix[i] = (double *) malloc(256 * sizeof(double));
    }

    //############ lecture des images ############

    FILE *f = NULL;
    struct dirent *currentImg = NULL;
    f = fopen("D:/c++/IndexationImage/output.csv", "wa");
    if (f == NULL) {
        printf("Problème au niveau de l'ouverture du fichier de sortie");
        return 1;
    }
    fprintf(f,"nom,couleur,contour,TR,TV,TB,meanGradient,histogramme\n");

    // ouverture du dossier
    rep = opendir(myRepo);
    if (rep == NULL) {
        printf("Problème au niveau de la lecture du dossier");
        return 1;
    }
    //lecture du dossier
    currentImg = readdir(rep);
    currentImg = readdir(rep);
    while ((currentImg = readdir(rep)) != NULL) {
        //vérification du type
        char imgName[255];
        sprintf(imgName, "%s//%s", myRepo, currentImg->d_name);
        char *imgType = NULL;
        //récupération de l'extension
        imgType = strtok(currentImg->d_name, ".");
        imgType = strtok(NULL, ".");

        if(imgType == NULL){

        }
        else if (!strcmp(imgType, "ppm")) {
            printf("%s\n", currentImg->d_name);
            double tauxR, tauxG, tauxB = 0.0;
            double meanGradient = 0.0;
            int color = 0;
            double nbPixelContour = 0;
            
            rgb8 **image;
            byte **imageMatrix;
            byte **gradient;

            /**
             * nrl = row lowest value
             * ncl = column lowest value
             * nrh = row highest value
             * nch = column highest value
             */
            long nrl, nrh, ncl, nch;
            
            image = LoadPPM_rgb8matrix(imgName, &nrl, &nrh, &ncl, &nch);
            imageMatrix = bmatrix(nrl, nrh, ncl, nch);
            gradient = bmatrix(nrl, nrh, ncl, nch);

            graysScale(image, imageMatrix, nrh, nch);

            double *hist = (double*)malloc(256 * sizeof(double));
            histogramme(imageMatrix, nrh, nch, hist);

            double *histR = (double*)malloc(256 * sizeof(double));
            double *histG = (double*)malloc(256 * sizeof(double));
            double *histB = (double*)malloc(256 * sizeof(double));
            histogrammeWithColor(image,nrh,nch,histR,histG,histB);

            color = colored(hist,histR,0.006);
            for (int k = 0; k < 256; k++) {
                matrix[nbimg][k] = hist[k];
            }

            nbimg++;

            if (color) {
                calculateColorRate(image,&tauxR,&tauxG,&tauxB, nrl, nrh, ncl, nch);
            } else {
                tauxR = 0.0;
                tauxG = 0.0;
                tauxB = 0.0;
            }

            pixelBorderDetection(imageMatrix, gradient, 20, &meanGradient, &nbPixelContour,
                           nrl, nrh, ncl, nch);
            fprintf(f, "%s.jpg,%d,%lf,%lf,%lf,%lf,%lf,", currentImg->d_name, color, nbPixelContour, tauxR, tauxG, tauxB,
                    meanGradient);
            sauvegardeHistogramme(hist, f);
            fprintf(f, "\n");
            free_rgb8matrix(image, nrl, nrh, ncl, nch);
            free_bmatrix(gradient, nrl, nrh, ncl, nch);
            free(hist);
        }
    }
    calculateDistanceMatrix(matrix, size, 1);
    calculateDistanceMatrix(matrix, size, 2);
    for (int j = 0; j < size; j++) {
        free(matrix[j]);
    }
    fclose(f);
    if (closedir(rep) == -1) {
        printf("Problème au niveau de la fermeture du dossier");
        return 1;
    }
    return 0;
}