#pragma once

void ShowGraphWindow(int num_processes,int quantum,double fcfs_wait, double sjf_wait, double rr_wait, double fcfs_turn, double sjf_turn, double rr_turn);

void ShowBarGraph(const char* title, const char* info, double v0, double v1, double v2);

void ShowAllGraphsWindow(
	int num_processes,
	int quantum,
	double fcfs_wait, double sjf_wait, double rr_wait,
	double fcfs_turn, double sjf_turn, double rr_turn,
	double fcfs_resp, double sjf_resp, double rr_resp,
	int ctx_fcfs, int ctx_sjf, int ctx_rr);