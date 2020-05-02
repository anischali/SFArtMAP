#include "dataset.h"
#include "func.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/**
 * \fn int iris_class(char *label)
 * \brief Permet de donner un entier pour les classes d'iris
 * \param label la chaine de caractère qui correspond à une clsse iris
 * \return un entier representant une classe 
 */

int iris_class(char *label)
{
    if (strncmp(label, "Iris-virginica", 15) == 0)
        return 0;
    else if (strncmp(label, "Iris-versicolor", 16) == 0)
        return 1;
    else if (strncmp(label, "Iris-setosa", 12) == 0)
        return 2;
    return -1;
}

/**
 * \fn double *get_vector(char *str, char *delim, int vsize, char **label)
 * \brief Fonction qui récupère les données et leurs label à partir d'une chaine de caractère brute
 * \param str chaine de caractère correspondant à une ligne de données.
 * \param delim le séparateur des composantes de données
 * \param vsize la dimension des données à récupérer
 * \param label récupérer le label de la donnée.
 */

double *get_vector(char *str, char *delim, int vsize, char **label)
{
    double *vect = (double *) calloc(vsize, sizeof *vect);
    char *save_ptr = NULL, *token = NULL, *str1 = NULL, *endptr = NULL;
    int ivec = 0;
    for (ivec = 0, str1 = str; ivec < vsize; ++ivec, str1 = NULL)
    {
        token = strtok_r(str1, delim, &save_ptr);
        if (token == NULL)
            break;
        vect[ivec] = strtod(token, &endptr);
    }
    str1 = NULL;
    token = strtok_r(str1, delim, &save_ptr);
    *label = strdup(token);
    return vect;
}


/**
 * \fn int hash_label(char *str)
 * \brief Cette fonction génère un entier correspondant au label
 * \param str a chaine de caractère qui correspond à une classe
 * \return un entier representant une classe 
 */

int hash_label(char *str)
{
    char *s = str;
    int n = 0, len = (int) strlen(str);
    for (int istr = 0; istr < len; ++istr)
    {
        n += *s;
        s++;
    }
    return n;
}


/**
 * \fn void detrmine_norm(dataset_t *db, int norm_type)
 * \breif Cette fonction détermine le type de norm pour les données.
 * \param db la base de données.
 * \param norm_type le type de norm
 */

void detrmine_norm(dataset_t *db, int norm_type)
{
    switch (norm_type)
    {
    case L1_NORM:
    case L1_CC:
        db->norm_func = l1_norm;
        break;
    case L2_NORM:
    case L2_CC:
        db->norm_func = l2_norm;
        break;
    default:
        break;
    }
}


/**
 * \fn dataset_t *dataset(data_property_t *properties)
 * \brief Cette fonction charge les données et les normaalise
 * \param properties constituent les parametres comme la dimension, la taille de la base et le fichier source des données.
 * \return retourne un objet de type dataset_t qui correspond à la base de données. 
 */
dataset_t *dataset(data_property_t *properties)
{
    dataset_t *db = (dataset_t *)malloc(sizeof *db);
    db->vsize = properties->vsize;
    db->db_size = properties->db_size;
    db->data = (struct data_t *) calloc(db->db_size, sizeof *db->data);
    struct data_t *data = db->data;
    FILE *db_file = fopen(properties->filename, "r");
    assert(db_file);
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int idata = 0, vsize = db->vsize, db_size = db->db_size, norm_type = properties->norm_type;
    detrmine_norm(db, norm_type);
    double (*norm)(double *, int) = db->norm_func;
    while ((nread = getline(&line, &len, db_file)) != -1)
    {
        if (line != NULL && idata < db_size)
        {
            data[idata].vector = get_vector(line, ",\n", db->vsize, &data[idata].label);
            data[idata].ilabel = iris_class(data[idata].label);
            normalize(data[idata].vector, vsize, norm);
            if (norm_type == L1_CC || norm_type == L2_CC)
                data[idata].vector = complement_coding(data[idata].vector, db->vsize);
            ++idata;
        }
    }
    free(line);
    line = NULL;
    fclose(db_file);
    return db;
}


/**
 * \fn void free_dataset(dataset_t *db)
 * \brief Libère la mémoire occupé par la base de données
 * \param db la base de données à libérer
 */

void free_dataset(dataset_t *db)
{
    for (int idata = 0; idata < db->db_size; ++idata)
    {
        free(db->data[idata].vector);
        db->data[idata].vector = NULL;
        free(db->data[idata].label);
        db->data[idata].label = NULL;
    }
    free(db->data);
    db->data = NULL;
    free(db);
    db = NULL;
}


/**
 * \fn void print_dataset(dataset_t *db)
 * \brief Cette fonction affiche à l'écran (stdout) la base de données.
 * \param la base de données à afficher
 */

void print_dataset(dataset_t *db)
{
    struct data_t *data = db->data;
    int vsize = db->vsize, db_size = db->db_size;
    for (int idata = 0; idata < db_size; ++idata)
    {
        print_vect(data[idata].vector, vsize, data[idata].label);
    }
}