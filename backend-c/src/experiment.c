#include "experiment.h"
#include <stdio.h>
#include <string.h>

static uint32_t next_random(uint32_t *state){uint32_t x=*state;x^=x<<13;x^=x>>17;x^=x<<5;*state=x;return x;}

DlValidation dl_experiment_generate(uint32_t seed,size_t process_count,size_t resource_count,DlScenario *out){
 if(!out||process_count<1||process_count>12||resource_count<1||resource_count>6)return (DlValidation){.ok=false,.message="Experiment bounds are 1-12 processes and 1-6 resources"};
 DlScenario generated;memset(&generated,0,sizeof generated);generated.process_count=process_count;generated.resource_count=resource_count;
 (void)snprintf(generated.id,sizeof generated.id,"experiment-%u",seed);
 uint32_t state=seed^UINT32_C(0xA341316C);if(state==0)state=UINT32_C(0x6D2B79F5);
 for(size_t i=0;i<process_count;i++)(void)snprintf(generated.processes[i],DL_ID_LENGTH,"P%zu",i+1);
 for(size_t j=0;j<resource_count;j++){
  (void)snprintf(generated.resources[j],DL_ID_LENGTH,"R%zu",j+1);
  generated.total[j]=1+(int)(next_random(&state)%3u);
  generated.available[j]=generated.total[j];
  generated.preemptible[j]=(next_random(&state)&1u)!=0;
  for(int instance=0;instance<generated.total[j];instance++)if((next_random(&state)&3u)!=0){size_t owner=next_random(&state)%process_count;generated.allocation[owner][j]++;generated.available[j]--;}
  for(size_t i=0;i<process_count;i++)generated.request[i][j]=(int)(next_random(&state)%((uint32_t)generated.total[j]+1u));
 }
 DlValidation valid=dl_validate(&generated);if(!valid.ok)return valid;*out=generated;return (DlValidation){.ok=true,.message="ok"};
}
