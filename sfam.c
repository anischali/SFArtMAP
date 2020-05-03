#include "network.h"
#include "func.h"
#include <string.h>

void init_sfam_struct(sfam_t *sfam)
{
    sfam->init = init_prototypes;
    sfam->free_prototypes = free_proto;
    sfam->learn = sfam_learn;
    sfam->prototypes_stat = sfam_stat;
    sfam->make_clusters = make_clusters;
    sfam->print_prototypes = print_clusters;
}

double *add_uncomitted(int vsize, double value)
{
    double *weights = (double *)malloc(vsize * sizeof *weights);
    vec_set(weights, vsize, value);
    return weights;
}

void init_prototypes(sfam_t *map)
{
    map->prototype_size = map->params->prototype_size;
    map->prototype_index = 0;
    neuron_t *proto = (neuron_t *)malloc(map->prototype_size * sizeof *proto);
    proto[map->prototype_index].weights = add_uncomitted(map->params->vsize, 0.5);
    proto[map->prototype_index].label = -1;
    map->prototypes = proto;
    double *acts = (double *)malloc(map->prototype_size * sizeof *acts);
    vec_set(acts, map->prototype_size, 0.5);
    map->activities = acts;
}

void free_proto(sfam_t *map)
{
    int psize = map->prototype_index;
    neuron_t *protos = map->prototypes;
    for (int iproto = 0; iproto <= psize; ++iproto)
    {
        if (protos[iproto].weights != NULL)
        {
            free(protos[iproto].weights);
            protos[iproto].weights = NULL;
        }
    }
    if (map->clusters != NULL)
    {
        free(map->clusters->prototypes);
        free(map->clusters->labels);
        free(map->clusters->csizes);
        if (map->clusters->datas != NULL)
        {
            for (int idx = 0; idx < map->prototype_index; ++idx)
            {
                if (map->clusters->datas[idx] != NULL)
                {
                    free(map->clusters->datas[idx]);
                }
            }
        }
        free(map->clusters->datas);
        free(map->clusters);
        map->clusters = NULL;
    }
    free(protos);
    protos = NULL;
    free(map->activities);
    map->activities = NULL;
    map->init = NULL;
    map->learn = NULL;
    map->prototypes_stat = NULL;
    map->print_prototypes = NULL;
    map->free_prototypes = NULL;
}

void commit_class(neuron_t *proto, const struct data_t input, int vsize)
{
    for (int ivec = 0; ivec < vsize; ++ivec)
    {
        proto->weights[ivec] = input.vector[ivec];
    }
    proto->label = input.ilabel;
}

void sfam_learn(sfam_t *map, dataset_t *db)
{
    sfam_param_t *params = map->params;
    struct data_t *data = db->data;
    int nepoch = params->nepoch, ndata = db->db_size, vsize = params->vsize, pass = 0;
    double rho_base = params->rho_baseline, proto_norm = 0.0, *tmp = malloc(vsize * sizeof *tmp),
           epsilon = params->epsilon, *activities = map->activities, input_norm = 0.0,
           vect_norm = params->norm, alpha = params->alpha, value = 0.0, *weights = NULL, *input = NULL;
    neuron_t *prototypes = map->prototypes;
    int *index = make_index(ndata), winner = -1;
    for (int iepoch = 0; iepoch < nepoch; ++iepoch)
    {
        shuffle(index, ndata);
        for (int idata = 0; idata < ndata; ++idata)
        {
            map->rho = rho_base;
            input = data[index[idata]].vector;
            winner = -1;
            pass = 0;
            for (int iact = 0; iact < map->prototype_index; ++iact)
            {
                fuzzy_and(tmp, prototypes[iact].weights, input, vsize);
                input_norm = l1_norm(tmp, vsize);
                proto_norm = l1_norm(prototypes[iact].weights, vsize);
                activities[iact] = input_norm / (alpha + proto_norm);
            }
            activities[map->prototype_index] = 0.9;
            while (winner == -1 && pass == 0)
            {
                winner = vect_max_idx(activities, map->prototype_index + 1);
                weights = prototypes[winner].weights;
                if (prototypes[winner].label == -1)
                {
                    commit_class(&prototypes[winner], data[index[idata]], vsize);
                    map->prototype_index += (map->prototype_index < map->prototype_size) ? 1 : 0;
                    prototypes[map->prototype_index].weights = add_uncomitted(vsize, 0.9);
                    prototypes[map->prototype_index].label = -1;
                    winner = -1;
                    pass = 1;
                }
                else
                {
                    fuzzy_and(tmp, weights, input, vsize);
                    input_norm = l1_norm(tmp, vsize);
                    value = input_norm / vect_norm;
                    if (value >= map->rho)
                    {
                        if (prototypes[winner].label == data[index[idata]].ilabel)
                        {
                            for (int ivec = 0; ivec < vsize; ++ivec)
                            {
                                weights[ivec] = params->beta * (min_double(weights[ivec], input[ivec])) + (1.0 - params->beta) * weights[ivec];
                            }
                            pass = 1;
                        }
                        else
                        {
                            fuzzy_and(tmp, weights, input, vsize);
                            input_norm = l1_norm(tmp, vsize);
                            value = input_norm / vect_norm;
                            activities[winner] = -1.0;
                            winner = -1;
                            map->rho = value + epsilon;
                            if (map->rho > 1.0)
                            {
                                pass = 1;
                            }
                        }
                    }
                    else
                    {
                        activities[winner] = -1.0;
                        winner = -1;
                    }
                }
            }
        }
    }
    free(index);
    free(tmp);
    index = NULL;
    tmp = NULL;
}

void sfam_stat(sfam_t *map, dataset_t *db)
{
    sfam_param_t *params = map->params;
    struct data_t *data = db->data;
    neuron_t *prototypes = map->prototypes;
    int *index = make_index(db->db_size), winner = -1, vsize = params->vsize, result = 0;
    shuffle(index, db->db_size);
    double *input, input_norm, proto_norm, *activities = map->activities,
                                           *tmp = malloc(vsize * sizeof *tmp);
    map->clusters = malloc(sizeof *map->clusters);
    map->clusters->psize = map->prototype_index;
    map->clusters->csizes = calloc(map->prototype_index, sizeof *map->clusters->csizes);
    map->clusters->prototypes = malloc(map->prototype_index * sizeof *map->clusters->prototypes);
    map->clusters->labels = malloc(map->prototype_index * sizeof *map->clusters->labels);
    map->clusters->datas = malloc(map->prototype_index * sizeof *map->clusters->datas);
    for (int idata = 0; idata < db->db_size; ++idata)
    {
        input = data[index[idata]].vector;
        winner = -1;
        for (int iact = 0; iact < map->prototype_index; ++iact)
        {
            fuzzy_and(tmp, prototypes[iact].weights, input, vsize);
            input_norm = l1_norm(tmp, vsize);
            proto_norm = l1_norm(prototypes[iact].weights, vsize);
            activities[iact] = input_norm / (params->alpha + proto_norm);
        }
        winner = vect_max_idx(activities, map->prototype_index + 1);
        if (prototypes[winner].label == data[index[idata]].ilabel)
        {
            map->clusters->csizes[winner]++;
            ++result;
        }
    }
    map->stat = result;
    free(index);
    free(tmp);
    index = NULL;
    tmp = NULL;
}

void make_clusters(sfam_t *map, dataset_t *db)
{
    sfam_param_t *params = map->params;
    struct data_t *data = db->data;
    neuron_t *prototypes = map->prototypes;
    int *index = make_index(db->db_size), winner = -1, vsize = params->vsize;
    shuffle(index, db->db_size);
    double *input, input_norm, proto_norm, *activities = map->activities,
                                           *tmp = malloc(vsize * sizeof *tmp);
    int *counts = calloc(map->prototype_index, sizeof *counts);
    for (int idx = 0; idx < map->prototype_index; ++idx)
    {
        map->clusters->datas[idx] = malloc((map->clusters->csizes[idx]) * sizeof *map->clusters->datas[idx]);
        map->clusters->labels[idx] = map->prototypes[idx].label;
        map->clusters->prototypes[idx] = map->prototypes[idx].weights;
    }
    for (int idata = 0; idata < db->db_size; ++idata)
    {
        input = data[index[idata]].vector;
        winner = -1;
        for (int iact = 0; iact < map->prototype_index; ++iact)
        {
            fuzzy_and(tmp, prototypes[iact].weights, input, vsize);
            input_norm = l1_norm(tmp, vsize);
            proto_norm = l1_norm(prototypes[iact].weights, vsize);
            activities[iact] = input_norm / (params->alpha + proto_norm);
        }
        winner = vect_max_idx(activities, map->prototype_index);
        if (counts[winner] < map->clusters->csizes[winner])
        {
            map->clusters->datas[winner][counts[winner]] = index[idata];
            counts[winner]++;
        }
    }
    free(index);
    free(tmp);
    index = NULL;
    tmp = NULL;
    free(counts);
    counts = NULL;
}

void print_clusters(sfam_t *map, dataset_t *db)
{
    const char *color[] = {GREEN_PRINT, RED_PRINT, BLUE_PRINT};
    double percent = 100.0 - (100.0 * map->stat / (double)db->db_size);
    int vsize = map->params->vsize;
    for (int iclust = 0; iclust < map->prototype_index; ++iclust)
    {
        fprintf(stdout, "The subclass contains %d vectors with prototype vector of ",  map->clusters->csizes[iclust]);
        print_vect(map->clusters->prototypes[iclust], vsize, db->data[map->clusters->datas[iclust][0]].label);
        fprintf(stdout, "\n");
        for (int idata = 0; idata < map->clusters->csizes[iclust]; ++idata)
        {
            int idx = map->clusters->datas[iclust][idata];
            fprintf(stdout, "\t\t");
            print_vect(db->data[idx].vector, vsize, db->data[idx].label);
        }
        fprintf(stdout, "\n\n");
    }
    fprintf(stdout, "Number of classes is: %d\n", map->prototype_index);
    fprintf(stdout, "Error percentage: %.2f\n", percent);
    fprintf(stdout, "Number of epoches are: %d\n", map->params->nepoch);
}