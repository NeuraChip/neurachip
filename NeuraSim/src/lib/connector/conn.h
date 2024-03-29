#ifndef CONN_H
#define CONN_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../config.h"
#include <queue>
#include "../memreq/memrequest.h"
#include "../instx/instx.h"

using namespace std;

// A single connections
class Conn {
	public:
		// Constructor
		// @param id: Connection ID
		Conn(GlobalCounters *gcp);
		~Conn();
		void print();
		int next_link_id;
		// void connect(TA unit_a, int port_a, TB unit_b, int port_b);
		queue<InstX *> *link_0_in_port;
		queue<InstX *> *link_0_out_port;
		queue<InstX *> *link_1_in_port;
		queue<InstX *> *link_1_out_port;
		bool tick();
		int get_next_link_id();
		void statistics();
		bool sanity_check();
		bool get_sim_end();
		void update_max_buffer_sizes();
		void measure_max_buffer_sizes();
		bool return_routine(bool sim_end, bool computed);
		void post_sim_metrics();
		void interval_metrics();
		string connected_vertices_names;
		void print_pressure_number(int pressure);
		void smart_stats();

	private:
		// Connection ID
		long long id;
		GlobalCounters *gcp;
		bool sim_end;
		bool max_buffer_sizes_computation;
		long long idle_cycles;
		long long interval_idle_cycles;
		long long busy_cycles;
		long long interval_busy_cycles;
		long long interval_id;
		// Max Buffer Sizes
		size_t link_0_in_port_max_buffer_size;
		size_t link_0_out_port_max_buffer_size;
		size_t link_1_in_port_max_buffer_size;
		size_t link_1_out_port_max_buffer_size;
		// Name of the component
		string component_name;
		// Name of the header component
		string header_component_name;
		// Collection
		mongocxx::collection collection;
		// Header Collection
		mongocxx::collection header_collection;
};


#endif // !CONN_H
