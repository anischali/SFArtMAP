#include "param.h"
#include "mjson.h"
#include "dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NFILES 4

static param_t params;

static const char *filenames[] = {
    "./conf/som.conf",
    "./conf/sfam.conf",
    "./conf/dataset_sfam.conf",
    "./conf/dataset_som.conf"};

static const struct json_attr_t som_attrs[] = {

    {"alpha", t_real, .addr.real = &(params.som_params.alpha)},
    {"ray", t_real, .addr.real = &(params.som_params.ray)},
    {"niter", t_integer, .addr.integer = &(params.som_params.niter)},
    {"init_type", t_integer, .addr.integer = &(params.som_params.init_type)},
    {"learning_type", t_integer, .addr.integer = &(params.som_params.learning_type)},
    {"map_height", t_integer, .addr.integer = &(params.som_params.map_height)},
    {"map_width", t_integer, .addr.integer = &(params.som_params.map_width)},
    {NULL}};

static const struct json_attr_t sfam_attrs[] = {
    {"alpha", t_real, .addr.real = &(params.sfam_params.alpha)},
    {"beta", t_real, .addr.real = &(params.sfam_params.beta)},
    {"epsilon", t_real, .addr.real = &(params.sfam_params.epsilon)},
    {"rho_baseline", t_real, .addr.real = &(params.sfam_params.rho_baseline)},
    {"prototype_size", t_integer, .addr.integer = &(params.sfam_params.prototype_size)},
    {"nepoch", t_integer, .addr.integer = &(params.sfam_params.nepoch)},
    {NULL}};

static const struct json_attr_t som_data_attr[] = {
    {"filename", t_string, .addr.string = params.som_data.filename, .len = SIZE_FILENAME},
    {"vsize", t_integer, .addr.integer = &params.som_data.vsize},
    {"db_size", t_integer, .addr.integer = &params.som_data.db_size},
    {"norm_type", t_integer, .addr.integer = &params.som_data.norm_type},
    {NULL}};

static const struct json_attr_t sfam_data_attr[] = {
    {"filename", t_string, .addr.string = params.sfam_data.filename, .len = SIZE_FILENAME},
    {"vsize", t_integer, .addr.integer = &params.sfam_data.vsize},
    {"db_size", t_integer, .addr.integer = &params.sfam_data.db_size},
    {"norm_type", t_integer, .addr.integer = &params.sfam_data.norm_type},
    {NULL}};

static const struct json_attr_t *attrs[] = {som_attrs, sfam_attrs, sfam_data_attr, som_data_attr};

static FILE *files[NFILES] = {NULL};

void open_files()
{
    for (int ifile = 0; ifile < NFILES; ++ifile)
    {
        files[ifile] = fopen(filenames[ifile], "r");
    }
}

void close_files()
{
    for (int ifile = 0; ifile < NFILES; ++ifile)
    {
        fclose(files[ifile]);
        files[ifile] = NULL;
    }
}



void read_configs()
{
    int c, count = 0, status = 0;
    bool begin[NFILES] = {false}, end[NFILES] = {false};
    char json[1024];
    for (int ifile = 0; ifile < NFILES; ++ifile)
    {
        count = 0;
        while (!feof(files[ifile]))
        {
            c = fgetc(files[ifile]);
            if (c != '{' && begin[ifile] == false)
                continue;
            else if (c == '\n' || c == '\t' || c == ' ')
            {
                continue;
            }
            begin[ifile] = true;
            json[count++] = (char) c;
            if (c == '}' && end[ifile] == false)
            {
                end[ifile] = true;
                json[count] = '\0';
                break;
            }
        }
        status = json_read_object(json, attrs[ifile], NULL);
        if (status != 0)
            puts(json_error_string(status));
    }
}

param_t *get_config()
{
    open_files();
    read_configs();
    params.som_params.vsize = params.som_data.vsize;
    if (params.sfam_data.norm_type == L1_CC || params.sfam_data.norm_type == L2_CC)
    {
        params.sfam_params.vsize = params.sfam_data.vsize * 2;
        params.sfam_params.norm = (double) params.sfam_data.vsize;
    }
    close_files();
    return &params;
}