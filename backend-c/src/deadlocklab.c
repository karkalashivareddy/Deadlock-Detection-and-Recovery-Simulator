#include "deadlocklab.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static DlValidation valid(void) { DlValidation v = { .ok = true, .message = "" }; return v; }
static DlValidation invalid(const char *message) { DlValidation v = { .ok = false }; snprintf(v.message, sizeof v.message, "%s", message); return v; }
static bool identifier(const char *s,size_t capacity){if(!s||!memchr(s,0,capacity)||!isalpha((unsigned char)s[0]))return false;for(size_t i=1;s[i];i++)if(!isalnum((unsigned char)s[i])&&s[i]!='_'&&s[i]!='-')return false;return true;}

DlValidation dl_validate(const DlScenario *s) {
    if (!s) return invalid("scenario is required");
    if (s->process_count > DL_MAX_PROCESSES || s->resource_count > DL_MAX_RESOURCES) return invalid("scenario exceeds configured process/resource limits");
    if(!memchr(s->id,0,sizeof s->id)||(s->id[0]&&!identifier(s->id,sizeof s->id)))return invalid("scenario identifier must use letters, digits, underscore, or hyphen");
    for (size_t i=0;i<s->process_count;i++) {
        if (!identifier(s->processes[i],sizeof s->processes[i])) return invalid("process identifiers must start with a letter and use only letters, digits, underscore, or hyphen");
        for (size_t k=0;k<i;k++) if (!strcmp(s->processes[k],s->processes[i])) return invalid("process identifiers must be unique");
        for (size_t j=0;j<s->resource_count;j++) if (s->allocation[i][j]<0 || s->request[i][j]<0) return invalid("allocation and request counts must be non-negative");
        if(s->terminated[i])for(size_t j=0;j<s->resource_count;j++)if(s->allocation[i][j]!=0||s->request[i][j]!=0)return invalid("terminated processes must have zero live allocation and request");
    }
    for (size_t j=0;j<s->resource_count;j++) {
        if (!identifier(s->resources[j],sizeof s->resources[j])) return invalid("resource identifiers must start with a letter and use only letters, digits, underscore, or hyphen");
        for (size_t k=0;k<j;k++) if (!strcmp(s->resources[k],s->resources[j])) return invalid("resource identifiers must be unique");
        if (s->total[j]<0 || s->available[j]<0) return invalid("resource counts must be non-negative");
        long long sum=s->available[j];
        for (size_t i=0;i<s->process_count;i++) if (!s->terminated[i]) sum+=s->allocation[i][j];
        if (sum!=s->total[j]) return invalid("resource accounting invariant failed");
    }
    return valid();
}

DlValidation dl_detect(const DlScenario *s, DlDetection *out) {
    DlValidation v=dl_validate(s); if(!v.ok)return v; if(!out)return invalid("detection result is required");
    memset(out,0,sizeof *out); int work[DL_MAX_RESOURCES]; bool finish[DL_MAX_PROCESSES]={false};
    for(size_t j=0;j<s->resource_count;j++)work[j]=s->available[j];
    for(size_t i=0;i<s->process_count;i++){finish[i]=s->terminated[i];bool zero=true;for(size_t j=0;j<s->resource_count;j++)if(s->allocation[i][j])zero=false;if(zero)finish[i]=true;}
    bool progress;
    do { progress=false;
        for(size_t i=0;i<s->process_count;i++) if(!finish[i]) {
            if(out->trace_count>=DL_MAX_PROCESSES*(DL_MAX_PROCESSES+1))return invalid("trace capacity exceeded");
            DlStep *step=&out->trace[out->trace_count++];step->process_index=i;step->eligible=true;
            for(size_t j=0;j<s->resource_count;j++){step->work_before[j]=work[j];step->component_fits[j]=s->request[i][j]<=work[j];if(!step->component_fits[j])step->eligible=false;}
            if(step->eligible){for(size_t j=0;j<s->resource_count;j++){step->released[j]=s->allocation[i][j];if(work[j]>INT_MAX-step->released[j])return invalid("resource arithmetic overflow");work[j]+=step->released[j];}finish[i]=true;progress=true;}
            for(size_t j=0;j<s->resource_count;j++)step->work_after[j]=work[j];
        }
    } while(progress);
    for(size_t j=0;j<s->resource_count;j++)out->final_work[j]=work[j];
    for(size_t i=0;i<s->process_count;i++)if(!finish[i])out->deadlocked[out->deadlocked_count++]=i;
    return valid();
}

DlValidation dl_recover_terminate(const DlScenario *s,size_t victim,DlScenario *out){DlValidation v=dl_validate(s);if(!v.ok)return v;if(!out||victim>=s->process_count||s->terminated[victim])return invalid("victim must be an active process");DlScenario candidate=*s;for(size_t j=0;j<s->resource_count;j++){if(candidate.available[j]>INT_MAX-candidate.allocation[victim][j])return invalid("resource arithmetic overflow");candidate.available[j]+=candidate.allocation[victim][j];candidate.allocation[victim][j]=0;candidate.request[victim][j]=0;}candidate.terminated[victim]=true;v=dl_validate(&candidate);if(!v.ok)return v;*out=candidate;return valid();}

static DlValidation checkpoint_compatible(const DlScenario*s,const DlScenario*c){DlValidation v=dl_validate(s);if(!v.ok)return v;v=dl_validate(c);if(!v.ok)return invalid("checkpoint state is invalid");if(strcmp(s->id,c->id)||s->process_count!=c->process_count||s->resource_count!=c->resource_count)return invalid("checkpoint belongs to a different scenario shape");for(size_t i=0;i<s->process_count;i++)if(strcmp(s->processes[i],c->processes[i]))return invalid("checkpoint process identifiers do not match current scenario");for(size_t j=0;j<s->resource_count;j++)if(strcmp(s->resources[j],c->resources[j])||s->total[j]!=c->total[j]||s->preemptible[j]!=c->preemptible[j])return invalid("checkpoint resource definition does not match current scenario");return valid();}
DlValidation dl_recover_rollback(const DlScenario*s,const DlScenario*c,DlScenario*out){DlValidation v=checkpoint_compatible(s,c);if(!v.ok)return v;if(!out)return invalid("recovery output is required");DlScenario candidate=*c;v=dl_validate(&candidate);if(!v.ok)return v;*out=candidate;return valid();}
DlValidation dl_recover_preempt_from_checkpoint(const DlScenario*s,const DlScenario*c,size_t victim,size_t resource,DlScenario*out){DlValidation v=checkpoint_compatible(s,c);if(!v.ok)return v;if(!out||victim>=s->process_count||resource>=s->resource_count||s->terminated[victim]||c->terminated[victim])return invalid("victim and resource must identify an active process/resource in both states");if(!s->preemptible[resource])return invalid("resource is not configured as preemptible");if(s->allocation[victim][resource]<=c->allocation[victim][resource])return invalid("checkpoint does not release any held instances of the selected resource");DlScenario candidate=*s;for(size_t j=0;j<s->resource_count;j++){candidate.allocation[victim][j]=c->allocation[victim][j];candidate.request[victim][j]=c->request[victim][j];}for(size_t j=0;j<s->resource_count;j++){long long used=0;for(size_t i=0;i<s->process_count;i++)if(!candidate.terminated[i])used+=candidate.allocation[i][j];if(used>s->total[j])return invalid("checkpoint rollback exceeds the total resource instances");candidate.available[j]=s->total[j]-(int)used;}v=dl_validate(&candidate);if(!v.ok)return v;*out=candidate;return valid();}
DlValidation dl_recover_preempt(const DlScenario*s,size_t victim,size_t resource,DlScenario*out){(void)s;(void)victim;(void)resource;(void)out;return invalid("resource preemption requires an explicit compatible checkpoint");}

const char *dl_fixture_ids(size_t i){static const char *ids[]={"no-deadlock","two-process-deadlock","multi-instance","zero-allocation","recovery-case"};return i<5?ids[i]:NULL;}
void dl_fixture(const char *id,DlScenario *s){memset(s,0,sizeof *s);snprintf(s->id,sizeof s->id,"%s",id);s->process_count=2;s->resource_count=2;strcpy(s->processes[0],"P1");strcpy(s->processes[1],"P2");strcpy(s->resources[0],"R1");strcpy(s->resources[1],"R2");
 if(!strcmp(id,"no-deadlock")){s->total[0]=2;s->total[1]=1;s->available[0]=1;s->allocation[0][0]=1;s->allocation[1][1]=1;s->request[0][0]=1;s->preemptible[0]=s->preemptible[1]=true;}
 else if(!strcmp(id,"multi-instance")){s->process_count=3;s->resource_count=1;strcpy(s->processes[2],"P3");s->total[0]=3;strcpy(s->resources[0],"R1");for(size_t i=0;i<3;i++)s->allocation[i][0]=1;s->request[0][0]=s->request[1][0]=1;s->preemptible[0]=true;}
 else if(!strcmp(id,"zero-allocation")){s->resource_count=1;strcpy(s->resources[0],"R1");s->total[0]=1;s->allocation[1][0]=1;s->request[0][0]=s->request[1][0]=1;s->preemptible[0]=true;}
 else {s->total[0]=s->total[1]=1;s->allocation[0][0]=s->allocation[1][1]=1;s->request[0][1]=s->request[1][0]=1;}
}
