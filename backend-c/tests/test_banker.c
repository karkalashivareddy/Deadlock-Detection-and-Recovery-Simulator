#include "banker.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void safe_fixture(DlBankerScenario*b){memset(b,0,sizeof *b);strcpy(b->id,"banker-safe");b->process_count=5;b->resource_count=3;const char*ps[]={"P0","P1","P2","P3","P4"};const char*rs[]={"A","B","C"};for(size_t i=0;i<5;i++)strcpy(b->processes[i],ps[i]);for(size_t j=0;j<3;j++)strcpy(b->resources[j],rs[j]);int total[]={10,5,7},available[]={3,3,2};memcpy(b->total,total,sizeof total);memcpy(b->available,available,sizeof available);int allocation[5][3]={{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};int maximum[5][3]={{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};for(size_t i=0;i<5;i++){memcpy(b->allocation[i],allocation[i],sizeof allocation[i]);memcpy(b->maximum[i],maximum[i],sizeof maximum[i]);}}
int main(void){DlBankerScenario b;DlBankerResult r;safe_fixture(&b);DlValidation v=dl_banker_safety(&b,&r);if(!v.ok||!r.safe||r.sequence_count!=5)fprintf(stderr,"Banker failure: ok=%d safe=%d seq=%zu %s totals=%d,%d,%d av=%d,%d,%d alloc=%d,%d,%d\n",v.ok,r.safe,r.sequence_count,v.message,b.total[0],b.total[1],b.total[2],b.available[0],b.available[1],b.available[2],b.allocation[0][0],b.allocation[0][1],b.allocation[0][2]);assert(v.ok&&r.safe&&r.sequence_count==5);const size_t expected[]={1,3,4,0,2};assert(!memcmp(expected,r.sequence,sizeof expected));assert(r.trace_count>=5);
 memset(&b,0,sizeof b);strcpy(b.id,"banker-unsafe");b.process_count=2;b.resource_count=1;strcpy(b.processes[0],"P0");strcpy(b.processes[1],"P1");strcpy(b.resources[0],"R");b.total[0]=2;b.allocation[0][0]=1;b.allocation[1][0]=1;b.maximum[0][0]=b.maximum[1][0]=2;v=dl_banker_safety(&b,&r);assert(v.ok&&!r.safe&&r.sequence_count==0&&r.unfinished_count==2);
 b.available[0]=1;b.total[0]=3;v=dl_banker_safety(&b,&r);assert(v.ok&&r.safe);
 b.maximum[0][0]=0;assert(!dl_banker_validate(&b).ok);
 memset(&b,0,sizeof b);strcpy(b.id,"empty");v=dl_banker_safety(&b,&r);assert(v.ok&&r.safe&&r.sequence_count==0);
 puts("C Banker's safety tests passed");return 0;}
