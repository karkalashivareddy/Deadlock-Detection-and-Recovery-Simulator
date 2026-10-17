#include "waitfor.h"
#include <string.h>

typedef struct {const DlScenario *scenario;bool edges[DL_MAX_PROCESSES][DL_MAX_PROCESSES];unsigned char color[DL_MAX_PROCESSES];size_t stack[DL_MAX_PROCESSES];size_t depth;DlWaitForResult *result;} Search;

static bool visit(Search *search,size_t process){
 search->color[process]=1;search->stack[search->depth++]=process;
 for(size_t next=0;next<search->scenario->process_count;next++)if(search->edges[process][next]){
  if(search->color[next]==0){if(visit(search,next))return true;}
  else if(search->color[next]==1){size_t start=0;while(start<search->depth&&search->stack[start]!=next)start++;if(start==search->depth)return false;search->result->has_cycle=true;for(size_t i=start;i<search->depth;i++)search->result->cycle[search->result->cycle_count++]=search->stack[i];search->result->cycle[search->result->cycle_count++]=next;return true;}
 }
 search->depth--;search->color[process]=2;return false;
}

DlValidation dl_wait_for_cycle(const DlScenario *scenario,DlWaitForResult *result){
 if(!scenario||!result)return (DlValidation){.ok=false,.message="Scenario and result are required"};
 DlValidation valid=dl_validate(scenario);if(!valid.ok)return valid;
 memset(result,0,sizeof *result);
 for(size_t j=0;j<scenario->resource_count;j++)if(scenario->total[j]!=1)return (DlValidation){.ok=false,.message="Wait-for cycle analysis requires exactly one instance of every resource type"};
 for(size_t i=0;i<scenario->process_count;i++)for(size_t j=0;j<scenario->resource_count;j++)if(scenario->request[i][j]>1)return (DlValidation){.ok=false,.message="Single-instance requests must be zero or one"};
 Search search;memset(&search,0,sizeof search);search.scenario=scenario;search.result=result;
 for(size_t i=0;i<scenario->process_count;i++)if(!scenario->terminated[i])for(size_t j=0;j<scenario->resource_count;j++)if(scenario->request[i][j])for(size_t holder=0;holder<scenario->process_count;holder++)if(!scenario->terminated[holder]&&scenario->allocation[holder][j])search.edges[i][holder]=true;
 for(size_t i=0;i<scenario->process_count&&!result->has_cycle;i++)if(!scenario->terminated[i]&&search.color[i]==0)(void)visit(&search,i);
 return (DlValidation){.ok=true,.message="ok"};
}
