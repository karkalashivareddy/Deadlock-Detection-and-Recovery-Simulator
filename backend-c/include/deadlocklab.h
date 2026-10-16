#ifndef DEADLOCKLAB_H
#define DEADLOCKLAB_H
#include <stddef.h>
#include <stdbool.h>

#define DL_MAX_PROCESSES 32
#define DL_MAX_RESOURCES 16
#define DL_ID_LENGTH 64

typedef struct {
    char id[DL_ID_LENGTH];
    size_t process_count, resource_count;
    char processes[DL_MAX_PROCESSES][DL_ID_LENGTH];
    char resources[DL_MAX_RESOURCES][DL_ID_LENGTH];
    int total[DL_MAX_RESOURCES], available[DL_MAX_RESOURCES];
    int allocation[DL_MAX_PROCESSES][DL_MAX_RESOURCES];
    int request[DL_MAX_PROCESSES][DL_MAX_RESOURCES];
    bool preemptible[DL_MAX_RESOURCES];
    bool terminated[DL_MAX_PROCESSES];
} DlScenario;

typedef struct {
    size_t process_index;
    bool eligible;
    int work_before[DL_MAX_RESOURCES];
    bool component_fits[DL_MAX_RESOURCES];
    int released[DL_MAX_RESOURCES];
    int work_after[DL_MAX_RESOURCES];
} DlStep;

typedef struct {
    size_t deadlocked_count, trace_count;
    size_t deadlocked[DL_MAX_PROCESSES];
    int final_work[DL_MAX_RESOURCES];
    DlStep trace[DL_MAX_PROCESSES * (DL_MAX_PROCESSES + 1)];
} DlDetection;

typedef struct { bool ok; char message[192]; } DlValidation;

DlValidation dl_validate(const DlScenario *scenario);
DlValidation dl_detect(const DlScenario *scenario, DlDetection *result);
DlValidation dl_recover_terminate(const DlScenario *input, size_t victim, DlScenario *output);
DlValidation dl_recover_preempt(const DlScenario *input, size_t victim, size_t resource, DlScenario *output);
void dl_fixture(const char *id, DlScenario *out);
const char *dl_fixture_ids(size_t index);

#endif
