//
// Created by Henri on 23/09/2019.
//
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
 * @param mask
 */
void printMask(const int mask[3][3]) {
    for (int i = 0; i < 3; i++) {
        printf("\n");
        for (int j = 0; j < 3; j++) {
            printf(" %d ", mask[i][j]);
        }
    }
    printf("\n");
}

/**
 *
 * @param value
 * @return
 */
int verifyRGBValue(int value) {
    if (value < 0) return 0;
    // Edit value to 15 or 20 to have absolute 255 pixel value edge detection
    if (value > 255) return 255;
    else return value;
}

void initMatrix(byte **matrix, long nrh, long nch) {
    for (int i = 0; i < nrh; i++) {
        for (int j = 0; j < nch; j++) {
            matrix[i][j] = 0;
        }
    }
}

/**
 *
 * @param mask
 * @param matrix
 * @param outputMatrix
 * @param matrix_max_x
 * @param matrix_max_y
 */
void applyMaskToMatrix(int mask[3][3], byte **matrix, byte **outputMatrix, int matrix_max_x, int matrix_max_y) {

    for (int i = 1; i < matrix_max_x - 1; i++) {
        for (int j = 1; j < matrix_max_y - 1; j++) {
            int temp = (mask[0][0] * (int) matrix[i - 1][j - 1] + mask[0][1] * (int) matrix[i - 1][j] +
                        mask[0][2] * (int) matrix[i - 1][j + 1]
                        + mask[1][0] * (int) matrix[i][j - 1] + mask[1][1] * (int) matrix[i][j] +
                        mask[1][2] * (int) matrix[i][j + 1]
                        + mask[2][0] * (int) matrix[i + 1][j - 1] + mask[2][1] * (int) matrix[i + 1][j] +
                        mask[2][2] * (int) matrix[i + 1][j + 1]) / 9;
            outputMatrix[i][j] = verifyRGBValue(temp);
        }
    }
}

/**
 *
 * @param mask
 * @param matrix
 * @param outputMatrix
 * @param matrix_max_x
 * @param matrix_max_y
 */
void applyMaskToMatrix_bounded(const int mask[3][3], byte **matrix, byte **outputMatrix, int nrl, int nrh, int ncl,
                               int nch) {
    for (int i = nrl + 1; i < nrh - 1; i++) {
        for (int j = ncl + 1; j < nch - 1; j++) {
            int pixel_value = floor((mask[0][0] * matrix[i - 1][j - 1] + mask[0][1] * matrix[i - 1][j] +
                                     mask[0][2] * matrix[i - 1][j + 1]
                                     + mask[1][0] * matrix[i][j - 1] + mask[1][1] * matrix[i][j] +
                                     mask[1][2] * matrix[i][j + 1]
                                     + mask[2][0] * matrix[i + 1][j - 1] + mask[2][1] * matrix[i + 1][j] +
                                     mask[2][2] * matrix[i + 1][j + 1]) / 9);
            outputMatrix[i][j] = verifyRGBValue(pixel_value);
        }
    }
}

/**
 *
 * @param matrix
 * @param output
 * @param matrix_max_x
 * @param matrix_max_y
 * @param limit
 */
void binariesPicture(byte **matrix, byte **output, int matrix_max_x, int matrix_max_y, int limit) {
    for (int i = 1; i < matrix_max_x - 1; i++) {
        for (int j = 1; j < matrix_max_y - 1; j++) {
            if (matrix[i][j] > limit) {
                output[i][j] = 255;
            } else {
                output[i][j] = 0;
            }
        }
    }
}

/**
 *
 * @param matrix
 * @param output
 * @param matrix_max_x
 * @param matrix_max_y
 */
void greyScale(rgb8 **matrix, byte **output, int matrix_max_x, int matrix_max_y) {
    for (int i = 1; i < matrix_max_x - 1; i++) {
        for (int j = 1; j < matrix_max_y - 1; j++) {
            output[i][j] = (matrix[i][j].r + (int) matrix[i][j].g + (int) matrix[i][j].b) / 3;
        }
    }
}


///
/// \param img l'image binarisée
/// \param nrh nombre de ligne de l'image
/// \param nch nombre de colonne de l'image
/// \param histogramme histogramme de l'image

void histogramme(byte **img, int nrh, int nch, double *hist) {
    int max = (nrh+1) * (nch+1);

    //initialise l'histogramme
    for (int i = 0; i < 256; i++) {
        hist[i] = 0.0;
    }

    //classifie l'image suivant les différents niveaux de gris
    for (int j = 0; j <= nrh; j++) {
        for (int k = 0; k <= nch; k++) {
            int index = img[j][k];
            hist[index] = hist[index] + 1.0;
        }
    }
    //normalisation de l'histogramme
    for (int m = 0; m < 256; m++){
        hist[m] = hist[m] / (max * 1.0);
    }
}

///
/// \param img l'image binarisée
/// \param nrh nombre de ligne de l'image
/// \param nch nombre de colonne de l'image
/// \param histogrammeR histogramme du rouge
/// \param histogrammeG histogramme du vert
/// \param histogrammeB histogramme du bleu

void histogrammeRGB(rgb8 **img, int nrh, int nch, double *histogrammeR, double * histogrammeG, double* histogrammeB) {
    int max = (nrh+1) * (nch+1);

    //initialise l'histogramme
    for (int i = 0; i < 256; i++) {
        histogrammeR[i] = 0.0;
        histogrammeG[i] = 0.0;
        histogrammeB[i] = 0.0;
    }

    //classifie l'image suivant les différents niveaux de gris
    for (int j = 0; j <= nrh; j++) {
        for (int k = 0; k <= nch; k++) {
            int indexR = img[j][k].r;
            int indexG = img[j][k].g;
            int indexB = img[j][k].b;
            histogrammeR[indexR] = histogrammeR[indexR] + 1.0;
            histogrammeG[indexG] = histogrammeG[indexG] + 1.0;
            histogrammeB[indexB] = histogrammeB[indexB] + 1.0;
        }
    }
    //normalisation de l'histogramme
    for (int m = 0; m < 256; m++) {
        histogrammeR[m] = histogrammeR[m] / (max * 1.0);
        histogrammeG[m] = histogrammeG[m] / (max * 1.0);
        histogrammeB[m] = histogrammeB[m] / (max * 1.0);
    }
}

/// Calcul la Distance entre 2 histogramme
/// \param hist1
/// \param hist2
/// \return distance de Bhattachryaa
double bhattacharyyaDistance(double *hist1, double *hist2) {
    double distance = 0.0;
    for (int i = 0; i < 256; i++) {
        distance += sqrt(hist1[i] * hist2[i]);
    }
    distance = -log(distance);
    return distance;
}

///
/// \param img
/// \param tauxr
/// \param tauxg
/// \param tauxb
/// \param nrl
/// \param nrh
/// \param ncl
/// \param nch
void tauxDeCouleurs(rgb8 **img,double *tauxr, double *tauxg, double *tauxb, long nrl, long nrh, long ncl, long nch) {
    (*tauxr) = 0.0;
    (*tauxg) = 0.0;
    (*tauxb) = 0.0;
    for (int i = 0; i < nrh-nrl; i++) {
        for (int j = 0; j < nch-ncl; j++) {
            (*tauxr) += img[i][j].r;
            (*tauxg) += img[i][j].g;
            (*tauxb) += img[i][j].b;
        }
    }
    int total=((*tauxr) + (*tauxb) + (*tauxg));
    (*tauxr) = (*tauxr) / total;
    (*tauxg) = (*tauxg) / total;
    (*tauxb) = (*tauxb) / total;
}


/**
 *
 * @param inputImage
 * @param gradientImage
 * @param nrl
 * @param nrh
 * @param ncl
 * @param nch
 */
void detectionBords(byte **img, byte **output, long threshold, double *meanGradient, double *nbPixelBord, long nrl,
                    long nrh, long ncl, long nch) {
    byte **deriv_x, **deriv_y;
    double normeGradient;
    *nbPixelBord = 0.0;
    *meanGradient = 0;
    deriv_x = bmatrix(nrl, nrh, ncl, nch);
    deriv_y = bmatrix(nrl, nrh, ncl, nch);

    initMatrix(deriv_x, nrh, nch);
    initMatrix(deriv_y, nrh, nch);

    applyMaskToMatrix_bounded(horizontal_gradient, img, deriv_x, nrl, nrh, ncl, nch);
    applyMaskToMatrix_bounded(vertical_gradient, img, deriv_y, nrl, nrh, ncl, nch);

    for (int i = 1; i < nrh - nrl; i++) {
        for (int j = 1; j < nch - ncl; j++) {
            normeGradient = sqrt(pow(deriv_x[i][j], 2) + pow(deriv_y[i][j], 2));
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
 * Sauvegarde d'un histogramme
 * @param nom nom du fichier
 * @param histogramme histogramme a enregistré
 */
void sauvegardeHistogramme(double *histogramme, FILE *f) {
    for (int i = 0; i < 255; i++) {
        fprintf(f, "%lf|", histogramme[i]);
    }
    fprintf(f, "%lf", histogramme[255]);
}

int colored(double* histogrammeBW ,double* histogrammeC,double SEUIL) {
    int i = 0;
    int j = 0;
    int color = 0;
    double distance = bhattacharyyaDistance(histogrammeBW,histogrammeC);
    if(distance > SEUIL) color=1;

    return color;
}

void matriceDesDistance(double **disttable, int size) {
    FILE *matrice = fopen("D:/c++/IndexationImage/img/matrice.csv", "wa");
    double matricedistance[size][size];
    double distance = 0.0;
    double distanceMax = 0.0;

    //calcul des distance
    for (int i = 0; i < size; i++) {
        double *hist = disttable[i];
        for (int j = 0; j < size; j++) {
            double *hist2 = disttable[j];
            distance = euclidienneDistance(hist, hist2);
            if(distance > distanceMax)
                distanceMax=distance;
            matricedistance[i][j]=distance;
        }
    }

    //normalisation
    for (int index = 0; index < size; index++) {
        for (int index2 = 0; index2 < size; index2++) {
            matricedistance[index][index2]=matricedistance[index][index2]*100/distanceMax;
        }
    }
    //sauvegarde
    for (int k = 0; k < size-1; k++) {
        fprintf(matrice, "%d,", k + 1);
    }

    fprintf(matrice, "%d\n", size);

    for (int l = 0; l < size; l++) {
        double *hist = disttable[l];
        for (int m = 0; m < size-1; m++) {
            fprintf(matrice, "%lf,", matricedistance[l][m]);
        }
        double *hist2 = disttable[size-1];
        fprintf(matrice, "%lf", matricedistance[l][size-1]);
        fprintf(matrice, "\n");
    }
    fclose(matrice);
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
    double **matrice = (double **) malloc(size * sizeof(double *));
    for (int i = 0; i < size; i++) {
        matrice[i] = (double *) malloc(256 * sizeof(double));
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
        else if (!strcmp(imgType, "jpg")) {
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

            greyScale(image, imageMatrix, nrh, nch);

            double *hist = (double*)malloc(256 * sizeof(double));
            histogramme(imageMatrix, nrh, nch, hist);

            double *histR = (double*)malloc(256 * sizeof(double));
            double *histG = (double*)malloc(256 * sizeof(double));
            double *histB = (double*)malloc(256 * sizeof(double));
            histogrammeRGB(image,nrh,nch,histR,histG,histB);

            color = colored(hist,histR,0.006);
            for (int k = 0; k < 256; k++) {
                matrice[nbimg][k] = hist[k];
            }

            nbimg++;

            if (color) {
                tauxDeCouleurs(image,&tauxR,&tauxG,&tauxB, nrl, nrh, ncl, nch);
            } else {
                tauxR = 0.33;
                tauxG = 0.33;
                tauxB = 0.33;
            }
            detectionBords(imageMatrix, gradient, 20, &meanGradient, &nbPixelContour, nrl, nrh, ncl, nch);
            fprintf(f, "%s.jpg,%d,%lf,%lf,%lf,%lf,%lf,", currentImg->d_name, color, nbPixelContour, tauxR, tauxG, tauxB,
                    meanGradient);
            sauvegardeHistogramme(hist, f);
            fprintf(f, "\n");
            free_rgb8matrix(image, nrl, nrh, ncl, nch);
            free_bmatrix(gradient, nrl, nrh, ncl, nch);
            free(hist);
        }
    }
    matriceDesDistance(matrice, size);
    for (int j = 0; j < size; j++) {
        free(matrice[j]);
    }
    fclose(f);
    if (closedir(rep) == -1) {
        printf("Error close directory");
        return 1;
    }
    return 0;
}

/*
void b_distToCSV(char *directory) {
    DIR *rep = NULL;
    int nbimg = 0;
    int size = 0;
    DIR *repcount1 = NULL;
    DIR *repcount2 = NULL;
    struct dirent *currentfile = NULL;
    repcount1 = opendir(directory);
    repcount2 = opendir(directory);
    if (repcount1 == NULL) {
        printf("Error read directory");
        return;
    }
    currentfile = readdir(repcount1);
    currentfile = readdir(repcount1);
    while ((currentfile = readdir(repcount1)) != NULL) {
        char currImgName[255];
        sprintf(currImgName, "%s\\%s", directory, currentfile->d_name);
        rgb8 **image;
        byte **imageMatrix;
        long nrl, nrh, ncl, nch;
        image = LoadPPM_rgb8matrix(currImgName, &nrl, &nrh, &ncl, &nch);
        imageMatrix = bmatrix(nrl, nrh, ncl, nch);
        greyScale(image, imageMatrix, nrh, nch);
        double *hist = malloc(256 * sizeof(double));
        histogramme(imageMatrix, nrh, nch, hist);
        while ((currentfile = readdir(repcount2)) != NULL) {
            char comparedImgName[255];
            sprintf(comparedImgName, "%s\\%s", directory, currentfile->d_name);
            rgb8 **image_comp;
            byte **imageMatrix_comp;
            long nrl_comp, nrh_comp, ncl_comp, nch_comp;
            image_comp = LoadPPM_rgb8matrix(comparedImgName, &nrl_comp, &nrh_comp, &ncl_comp, &nch_comp);
            imageMatrix_comp = bmatrix(nrl_comp, nrh_comp, ncl_comp, nch_comp);
            greyScale(image_comp, imageMatrix_comp, nrh_comp, nch_comp);
            double *hist_comp = malloc(256 * sizeof(double));
            histogramme(imageMatrix_comp, nrh_comp, nch_comp, hist_comp);
            double distance = bhattacharyyaDistance(hist, hist_comp);
            printf("%c %c %f", currImgName, comparedImgName, distance);
            free_rgb8matrix(image_comp, nrl_comp, nrh_comp, ncl_comp, nch_comp);
            free_bmatrix(imageMatrix_comp, nrl_comp, nrh_comp, ncl_comp, nch_comp);
        }
        free_rgb8matrix(image, nrl, nrh, ncl, nch);
        free_bmatrix(imageMatrix, nrl, nrh, ncl, nch);
    }
}
*/