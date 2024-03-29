#ifndef WRITER_H
#define WRITER_H

#include <iostream>

#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include "../hashresp/hashresponse.h"
#include "../config.h"
#include "../memreq/memrequest.h"
#include <queue>
#include "fstream"


using namespace std;

class Writer {
	public:
		Writer();
		Writer(GlobalCounters *gcp, long long x_cood);
		~Writer();
		void custom_constructor(GlobalCounters *gcp, 
								long long x_cood);
		bool tick();
		void print();
		void statistics();
		// Ports have to be public because they are accessed by Connector class
		queue<InstX *> *in_port_array;
		queue<InstX *> *out_port_array;
		queue<InstX *> *get_in_port_ref(int);
		queue<InstX *> *get_out_port_ref(int);
		string get_vertex_name();
		bool sanity_check();
		bool get_sim_end();
		void post_sim_metrics();
		void interval_metrics();
		bool return_routine(bool sim_end, bool computed);
		int get_id();
		int get_uid();
		HashResponse *get_next_response();
		MemRequest *generate_write_request(	HashResponse *hash_resp,
											long long write_address
										);
		long long generate_write_address(HashResponse *hash_resp);
		void send_write_request(MemRequest *write_req);
		void create_C_row_ptr_array();
		void close_C_files();
		void load_C_files();
		// int get_port_from_uid(int uid);
		string vertex_name;
		int get_dest_port(int dest_uid);
		vector<int> arch_graph_distances;
		void create_arch_graph_distances();
		vector<int> neighbors_desc;
		vector<int> neighbors_uid;
		// UID to port mapping
		map<int, int> uid_to_port_map;
		void smart_stats();
	private:
		GlobalCounters *gcp;
		int id;
		int uid;
		int port_count;
		bool sim_end;
		int processing_counter;
		long long idle_cycles;
		long long busy_cycles;
		long long interval_idle_cycles;
		long long interval_busy_cycles;
		long long interval_id;
		int last_processed_port;
		long long x_cood;
		long long y_cood;
		long long *C_row_ptr_array;
		long long C_row_ptr_array_size;
		long long C_data_base_address;
		long long *atomic_offsets_array;
		fstream C_row_ptr_file;
		fstream C_header_file;
		// Name of the component
		string component_name;
		// Name of the header component
		string header_component_name;
		// Collection
		mongocxx::collection collection;
		// Header Collection
		mongocxx::collection header_collection;
};






#endif // !WRITER_H