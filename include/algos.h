#pragma once
#include "pclass.h"
#include "sort.h"
#include<limits.h>

struct Slice; // forward declaration

SchedResult fcfs(Process *p, int n);
SchedResult sjf(Process *p, int n);
SchedResult rr(Process *p, int n, int q);
SchedResult rr_ctx(Process *p, int n, int q, int *ctx_switches);
SchedResult rr_with_trace(Process *p, int n, int q, struct Slice **out, int *outCount);