#ifndef DEADLOCKLAB_BANKER_H
#define DEADLOCKLAB_BANKER_H
#include "deadlocklab.h"

typedef struct {
    char id[DL_ID_LENGTH];
    size_t process_count, resource_count;
    char processes[DL_MAX_PROCESSES][DL_ID_LENGTH];
    char resources[DL_MAX_RESOURCES][DL_ID_LENGTH];
    int total[DL_MAX_RESOURCES], available[DL_MAX_RESOURCES];
    int allocation[DL_MAX_PROCESSES][DL_MAX_RESOURCES];
    int maximum[DL_MAX_PROCESSES][DL_MAX_RESOURCES];
} DlBankerScenario;

typedef struct {
    size_t process_index;
    bool eligible;
    int work_before[DL_MAX_RESOURCES];
    int need[DL_MAX_RESOURCES];
    int work_after[DL_MAX_RESOURCES];
} DlBankerStep;

typedef struct {
    bool safe;
    size_t sequence_count, trace_count, unfinished_count;
    size_t sequence[DL_MAX_PROCESSES];
    size_t unfinished[DL_MAX_PROCESSES];
    DlBankerStep trace[DL_MAX_PROCESSES * (DL_MAX_PROCESSES + 1)];
} DlBankerResult;

DlValidation dl_banker_validate(const DlBankerScenario *scenario);
DlValidation dl_banker_safety(const DlBankerScenario *scenario, DlBankerResult *result);

#endif
