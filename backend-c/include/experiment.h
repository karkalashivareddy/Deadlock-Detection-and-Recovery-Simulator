#ifndef DEADLOCKLAB_EXPERIMENT_H
#define DEADLOCKLAB_EXPERIMENT_H
#include "deadlocklab.h"
#include <stdint.h>

DlValidation dl_experiment_generate(uint32_t seed,size_t process_count,size_t resource_count,DlScenario *out);

#endif
