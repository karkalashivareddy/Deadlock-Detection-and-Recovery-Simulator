#include "waitfor.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void){DlScenario s,snapshot;DlWaitForResult result;dl_fixture("two-process-deadlock",&s);snapshot=s;assert(dl_wait_for_cycle(&s,&result).ok);assert(result.has_cycle&&result.cycle_count==3&&result.cycle[0]==0&&result.cycle[1]==1&&result.cycle[2]==0);assert(!memcmp(&snapshot,&s,sizeof s));memset(&s,0,sizeof s);strcpy(s.id,"single-chain");s.process_count=2;s.resource_count=1;strcpy(s.processes[0],"P1");strcpy(s.processes[1],"P2");strcpy(s.resources[0],"R1");s.total[0]=1;s.allocation[0][0]=1;s.request[1][0]=1;assert(dl_wait_for_cycle(&s,&result).ok&&!result.has_cycle);s.available[0]=1;s.allocation[0][0]=0;assert(dl_wait_for_cycle(&s,&result).ok&&!result.has_cycle);dl_fixture("multi-instance",&s);assert(!dl_wait_for_cycle(&s,&result).ok);memset(&s,0,sizeof s);strcpy(s.id,"bad-request");s.process_count=1;s.resource_count=1;strcpy(s.processes[0],"P1");strcpy(s.resources[0],"R1");s.total[0]=1;s.available[0]=1;s.request[0][0]=2;assert(!dl_wait_for_cycle(&s,&result).ok);puts("C wait-for graph tests passed");return 0;}
