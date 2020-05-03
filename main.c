#include "dataset.h"
#include "network.h"
#include "func.h"
#include <time.h>
#include <stdlib.h>

static algorithms_t algos;


void init(algorithms_t *algorithms)
{
    algorithms->init = init_all;
    algorithms->free_all = free_algos;
    algorithms->time_compar = time_compar;
}




int main(int argc, char const *argv[])
{
    srand(time(NULL));
    init(&algos);
    algos.init(&algos);
    algos.time_compar(&algos);
    algos.som.tag(&algos.som, algos.databases[SOM_DB]);
    algos.som.map_stat(&algos.som, algos.databases[SOM_DB]); 
    algos.som.print_map(&algos.som);
    algos.sfam.prototypes_stat(&algos.sfam, algos.databases[SFAM_DB]);
    algos.sfam.make_clusters(&algos.sfam, algos.databases[SFAM_DB]);
    algos.sfam.print_prototypes(&algos.sfam, algos.databases[SFAM_DB]);
    algos.free_all(&algos);
    return 0;
}
