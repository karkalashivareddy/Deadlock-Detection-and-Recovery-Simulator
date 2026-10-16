#include "deadlocklab.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){DlScenario s,out;DlDetection d;DlValidation v;
 memset(&s,0,sizeof s);v=dl_detect(&s,&d);assert(v.ok&&d.deadlocked_count==0&&d.trace_count==0);
 dl_fixture("no-deadlock",&s);v=dl_detect(&s,&d);assert(v.ok&&d.deadlocked_count==0);
 dl_fixture("two-process-deadlock",&s);v=dl_detect(&s,&d);assert(v.ok&&d.deadlocked_count==2&&d.deadlocked[0]==0&&d.deadlocked[1]==1);
 dl_fixture("multi-instance",&s);v=dl_detect(&s,&d);assert(v.ok&&d.deadlocked_count==0);
 dl_fixture("zero-allocation",&s);v=dl_detect(&s,&d);assert(v.ok&&d.deadlocked_count==1&&d.deadlocked[0]==1);for(size_t i=0;i<d.trace_count;i++)assert(d.trace[i].process_index!=0);
 dl_fixture("two-process-deadlock",&s);v=dl_recover_terminate(&s,0,&s);assert(v.ok&&s.available[0]==1&&s.allocation[0][0]==0&&s.request[0][1]==0&&s.terminated[0]);assert(dl_detect(&s,&d).ok&&d.deadlocked_count==0);
 dl_fixture("two-process-deadlock",&s);v=dl_recover_preempt(&s,0,0,&out);assert(!v.ok);s.preemptible[0]=true;v=dl_recover_preempt(&s,0,0,&out);assert(v.ok&&out.available[0]==1&&out.request[0][1]==1&&out.allocation[0][0]==0);assert(dl_detect(&out,&d).ok&&d.deadlocked_count==0);
 s.available[0]=-1;assert(!dl_validate(&s).ok);dl_fixture("two-process-deadlock",&s);s.terminated[0]=true;assert(!dl_validate(&s).ok);
 puts("C deadlocklab tests passed");return 0;}
