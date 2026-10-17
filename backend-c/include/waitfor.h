#ifndef DEADLOCKLAB_WAITFOR_H
#define DEADLOCKLAB_WAITFOR_H
#include "deadlocklab.h"

typedef struct {bool has_cycle;size_t cycle_count;size_t cycle[DL_MAX_PROCESSES+1];} DlWaitForResult;
DlValidation dl_wait_for_cycle(const DlScenario *scenario,DlWaitForResult *result);

#endif
