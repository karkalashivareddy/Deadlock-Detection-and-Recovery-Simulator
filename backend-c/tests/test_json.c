#include "json.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void rejects(const char *s){JsonDoc d={0};JsonError e;assert(!json_parse(&d,s,strlen(s),&e));assert(e.message[0]);json_free(&d);}
int main(void){
 JsonDoc d={0};JsonError e;const char *valid="{\"outer\":[{\"name\":\"A\\uD83D\\uDE80\",\"n\":12}],\"ok\":true}";assert(json_parse(&d,valid,strlen(valid),&e));size_t outer=json_object_get(&d,0,"outer");assert(json_array_size(&d,outer)==1);size_t item=json_array_get(&d,outer,0);char name[16];assert(json_string_copy(&d,json_object_get(&d,item,"name"),name,sizeof name));assert(!strcmp(name,"A\xF0\x9F\x9A\x80"));int n;assert(json_integer(&d,json_object_get(&d,item,"n"),&n)&&n==12);bool b;assert(json_boolean(&d,json_object_get(&d,0,"ok"),&b)&&b);json_free(&d);
 rejects("{\"a\":1,\"a\":2}");rejects("{\"a\":1,\"\\u0061\":2}");rejects("{\"a\":\"\\x\"}");rejects("{\"a\":\"\\uD800\"}");rejects("[1,]");rejects("{\"a\":1} trailing");rejects("01");
 const char *fraction="1.5";assert(json_parse(&d,fraction,strlen(fraction),&e));assert(!json_integer(&d,0,&n));json_free(&d);const char *negative="-1";assert(json_parse(&d,negative,strlen(negative),&e));assert(!json_integer(&d,0,&n));json_free(&d);const char *overflow="2147483648";assert(json_parse(&d,overflow,strlen(overflow),&e));assert(!json_integer(&d,0,&n));json_free(&d);
 puts("C JSON parser tests passed");return 0;
}
