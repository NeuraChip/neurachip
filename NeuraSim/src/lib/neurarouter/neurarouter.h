#ifndef NEURA_ROUTER_H
#define NEURA_ROUTER_H

#include <iostream>
#include "../debugger/debugger.h"
#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include <queue>
#include <string>

using namespace std;

class NeuraRouter {
	public:
		NeuraRouter();
		NeuraRouter(GlobalCounters *gcp, long long x_cood, long long y_cood);
		~NeuraRouter();

		bool tick();
		void print();
		void statistics();
		queue<InstX *> *get_in_port_ref(int);
		queue<InstX *> *get_out_port_ref(int);
		string get_vertex_name();
		bool sanity_check();
		int **create_routing_matrix();
		void custom_constructor(GlobalCounters *gcp, 
								long long x_cood, 
								long long y_cood);
		// Ports have to be public because 
		// they are accessed by Connector class`
		queue<InstX *> *in_port_array;
		queue<InstX *> *out_port_array;
		void print_conn_usage(int port_id);
		bool get_sim_end();
		void post_sim_metrics();
		void interval_metrics();
		bool return_routine(bool sim_end, bool computed);
		int get_id();
		int get_uid();
		string vertex_name;
		int get_dest_port(int dest_uid);
		vector<int> arch_graph_distances;
		void create_arch_graph_distances();
		vector<int> neighbors_desc;
		vector<int> neighbors_uid;
		vector<int> neighbors_uid_only_routers;
		// UID to port mapping
		map<int, int> uid_to_port_map;
		void smart_stats();
	private:
		GlobalCounters *gcp;
		int id;
		int uid;
		int *connected_components;
		int port_count;
		bool sim_end;
		int processing_counter;
		long long idle_cycles;
		long long interval_idle_cycles;
		long long busy_cycles;
		long long interval_busy_cycles;
		long long interval_id;
		int last_processed_port;
		long long x_cood;
		long long y_cood;
		int total_routers;
		// Routing Matrix
		int **routing_matrix;
		int total_neura_cores;
		int total_neura_mems;
		int total_writers;
		int total_memory_controllers;
		int total_destinations;
		// Name of the component
		string component_name;
		// Name of header component
		string header_component_name;
		// Collection
		mongocxx::collection collection;
		// Header Collection
		mongocxx::collection header_collection;
		bool attempt_move_packet(int port_in, int port_out);
		// Port used in last cycle
		int last_port_used;
		// int get_dest_port(int port_in);
		void create_network_graph();

		

};




#endif // !NEURA_ROUTER_H