/**
 * \file dataset.h
 * \brief fihier header pour les fonctions de chargement des données.
 * \author Anis CHALI
 * \version 0.1
 * \date 15 Juillet 2019
 *
 * Fonction et structures pour le chargement des données.
 *
 */

#ifndef DATASET_H
#define DATASET_H
#include "param.h"
#include <stdio.h>
#include <stdlib.h>


#define RED_PRINT "\x1b[31m"
#define GREEN_PRINT "\x1b[32m"
#define BLUE_PRINT "\x1b[34m"
#define DEFAULT_PRINT "\x1b[0m"
/** 
 * \enum DB
 * \brief Indice pour l'acces au base de données
*/
enum DB
{
    SOM_DB = 0,
    SFAM_DB = 1
};

/**
 * \enum NORMALIZATION
 * \brief Type de normalisation pour la base de données
 */

enum NORMALIZATION
{
    NO_NORM = 0,
    L1_NORM,
    L2_NORM,
    L1_CC,
    L2_CC
};


typedef struct dataset_t dataset_t;


/**
 * \struct data_t
 * \brief Objet pour une données de la base de données.
 * 
 * La structure permet de stocker les composantes des vecteur de données,
 * la chaine de caractère pour le label et un entier representant le label.  
 */
struct data_t
{
    char *label;
    int ilabel;
    double *vector;
};


/**
 * \struct dataset_t
 * \brief Objet permettant de stocker la base de données
 * 
 * Cette structure permet de stocker toutes la base de données 
 * avec la taille de la base de données, la dimension des vecteurs 
 * de données et un tableau des données ainsi que un pointeur de 
 * fonction pour le type de norm choisi pour les données.
 */
struct dataset_t
{
    int vsize;
    int db_size;
    double (*norm_func)(double *v, int s);
    struct data_t *data;
};



int iris_class(char *label);
dataset_t *dataset(data_property_t *properties);
void free_dataset(dataset_t *db);
void print_dataset(dataset_t *db);
#endif