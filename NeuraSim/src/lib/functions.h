#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <cmath>
#include <fstream>
#include "connector/conn.h"
#include "counters/globalcounters.h"
#include "argparse/argparse.h"
#include <omp.h>

// #include "neuratile.h"

using namespace std;

class NeuraTile;

struct Input {
		long *number;
};

Input get_inputs (int, char**, Input, int printer=0);
void print_inputs(int, Input);
int run_sim(NeuraTile *neura_tile);

template <class TA, class TB>
Conn *create_link(GlobalCounters *, TA, int, TB, int);

template <typename TA, typename TB>
Conn *create_link(GlobalCounters *gcp, TA unit_a, int port_a, TB unit_b, int port_b) {
	Conn *conn;
	conn = new Conn(gcp);
	conn->link_0_in_port = unit_a->get_out_port_ref(port_a);
	conn->link_0_out_port = unit_b->get_in_port_ref(port_b);
	conn->link_1_in_port = unit_b->get_out_port_ref(port_b);
	conn->link_1_out_port = unit_a->get_in_port_ref(port_a);
	conn->connected_vertices_names = unit_a->get_vertex_name() + " - " + unit_b->get_vertex_name();
	return conn;
}

NeuraTile *sim_init(ArgParse *arg_parse);

void statistics (NeuraTile *neura_tile);

bool sanity_check (NeuraTile *neura_tile);

void sim_end_printer (bool b);

void post_sim(NeuraTile *neura_tile);

int address_to_mc_id(GlobalCounters *gcp, int address);

void sim_cleanup(NeuraTile *neura_tile);

void greetings();

void neuraviz_greetings();

void launch_visualizer_script(NeuraTile *neura_tile);

int packet_routing(int dest_uid, GlobalCounters *gcp, vector<int> *neighbors_uid, map<int, int> *uid_to_port_map);

void arch_distancing(	GlobalCounters *gcp,
						vector<int>& arch_graph_distances, 
						string vertex_name, 
						int uid, 
						vector<int>& neighbors_desc, 
						vector<int>& neighbors_uid);

void reset_results_dir_permissions(string experiment_name);

void host_notify(ArgParse *arg_parse);

#endif // FUNCTIONS_H
