#ifndef MEMORY_CONTROLLER_H
#define MEMORY_CONTROLLER_H

#include <iostream>
#include "../debugger/debugger.h"
#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include <queue>
#include "memory_system.h"
#include <functional>
#include <fstream>
#include <string>

using namespace std;
using namespace dramsim3;

class MemoryController {
	public:
		MemoryController();
		MemoryController(GlobalCounters *gcp, int x_cood);
		void custom_constructor(GlobalCounters *gcp, int x_cood);
		~MemoryController();
		bool tick();
		void print();
		void statistics();
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
		MemRequest *get_next_memory_request();
		// Ports have to be public because they are accessed by Connector class
		queue<InstX *> *in_port_array;
		queue<InstX *> *out_port_array;
		void ReadCallBack_0(uint64_t addr);
		void WriteCallBack_0(uint64_t addr);
		void ReadCallBack_1(uint64_t addr);
		void WriteCallBack_1(uint64_t addr);
		void send_memory_response(MemRequest *mem_req);
		MemResponse *generate_memory_response(MemRequest *mem_req);
		// int get_port_from_uid(int uid);
		void load_dram_data();
		void print_stats();
		void print_json();
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
		int address_to_index_converter(long long address);
		long long hbm_0_reqs;
		long long hbm_1_reqs;
		long long hbm_0_reads;
		long long hbm_1_reads;
		long long hbm_0_writes;
		long long hbm_1_writes;
		// Map of address to data
		map<uint64_t, int> address_data_map;
	private:
		GlobalCounters *gcp;
		int id;
		int uid;
		int x_cood;
		int port_count;
		bool sim_end;
		int processing_counter;
		long long idle_cycles;
		long long busy_cycles;
		long long mem_addresses_merged;
		double avg_in_flight_requests;
		double interval_avg_in_flight_requests;
		long long rejected_requests;
		long long interval_idle_cycles;
		long long interval_busy_cycles;
		long long interval_rejected_requests;
		long long interval_id;
		int last_processed_port;
		// MC Accessed
		long long *accessed_address_array;
		long long *accessed_at_cycle_array;
		bool *accessed_is_write_array;
		long long accessed_size;
		long long rejected_accessed_size;
		bool accessed_warn_flag;
		long long *rejected_accessed_address_array;
		long long *rejected_accessed_at_cycle_array;
		bool *rejected_accessed_port_number_array;
		
		// Name of the component
		string component_name;
		// Name of the header component
		string header_component_name;
		string dram_name;
		mongocxx::collection dram_collection;
		// Collection
		mongocxx::collection collection;
		// Header Collection
		mongocxx::collection header_collection;
		// Map of memory requests ID to memory requests *
		// map<int, MemRequest *> processing_mem_req_map;
		// Map of address to memory requests * vector
		map<uint64_t, vector<MemRequest *>> address_mem_req_map;
		MemorySystem *hbm_0;
		MemorySystem *hbm_1;
		MemorySystem **hbm_array;
		

		string dram_data_file_name_0;
		string dram_output_dir_0;
		string dram_data_file_name_1;
		string dram_output_dir_1;
		// long long test_address;
		// long long test_count;
		// long long test_total_instx;
		// long long test_complete_instx;
		// long long test_rejected_count;
		bool tick_port(int port_id);
		void update_mem_req_map_and_add_transaction(MemRequest *mem_req);
		void push_to_mc_accessed(long long address, long long cycle, bool is_write);
		void push_to_rejected_mc_accessed(long long address, long long cycle, int port_number);
		void write_accessed_to_file();
		long long *port_stalls;
		
};






#endif // !MEMORY_CONTROLLER_H