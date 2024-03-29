#ifndef NEURA_CORE_H
#define NEURA_CORE_H

#include <iostream>
#include <queue>
#include "../counters/globalcounters.h"
#include "../memreq/memrequest.h"
#include "../hashreq/hashrequest.h"
#include "../hashresp/hashresponse.h"
#include <map>
#include <string>

using namespace std;

/*
  _   _                       ____               
 | \ | | ___ _   _ _ __ __ _ / ___|___  _ __ ___ 
 |  \| |/ _ \ | | | '__/ _` | |   / _ \| '__/ _ \
 | |\  |  __/ |_| | | | (_| | |__| (_) | | |  __/
 |_| \_|\___|\__,_|_|  \__,_|\____\___/|_|  \___|
                                                 
*/

// A single NeuraCore consisting of a systolic array of processing elements
class NeuraCore {
	public:
		// Constructor
		// @param id: NeuraCore ID
		// @param x_cood: NeuraCore X Coordinate
		// @param y_cood: NeuraCore Y Coordinate
		NeuraCore(GlobalCounters *gcp, int x_cood, int y_cood, int port_count);
		~NeuraCore();
		// Default Constructor
		NeuraCore();
		// Custom Constructor
		void custom_constructor(GlobalCounters *gcp, int x_cood, int y_cood, int port_count);
		void print();
		bool test_ports();
		bool tick();
		queue<InstX *> *get_in_port_ref(int port_id);
		queue<InstX *> *get_out_port_ref(int port_id);
		string get_vertex_name();
		void statistics();
		void inc_reg_file_times();
		void send_mem_request(MemRequest *mem_request);
		InstX *receive_instx(int port_id);
		MemRequest *generate_mem_request(	GlobalCounters *gcp,
											int src_unit_uid,
											int dest_unit_uid,
											int req_type,
											int src_address,
											int dest_address,
											int size,
											int payload,
											HashRequest *src_hash_req);
		MemRequest *generate_mem_response(	GlobalCounters *gcp,
											MemRequest *src_mem_req);
		HashRequest *generate_hash_request(	int dest_unit_uid,
                        					long long a_data_addr,
                        					long long b_data_addr,
                        					long long b_col_id_addr,
                        					long long count,
                        					long long table_id);

		// Ports have to be public because they are accessed by Connector class

		// In Port array
		queue<InstX *> *in_port_array;
		// Out Port array
		queue<InstX *> *out_port_array;
		// Register File
		vector<InstX *> register_file;
		// Sanity Check
		bool sanity_check();
		bool get_sim_end();
		void remove_hash_request(HashRequest *hash_request);
		bool return_routine(bool sim_end, bool computed);
		void print_register_file();
		void post_sim_metrics();
		void interval_metrics();
		int get_uid();
		int get_id();
		void send_hash_request(HashRequest *hash_request);
		// int get_port_from_uid(int uid);
		void map_hash_request_to_neuramem(HashRequest *hash_req);
		int get_nm_uid_from_b_col_id(int b_col_id);
		int get_nm_uid_from_address(long long address);
		HashResponse *generate_hash_response(HashRequest *hash_req);
		int get_writer_uid_from_table_id(long long table_id);
		void send_hash_response(HashResponse *hash_resp);
		string vertex_name;
		// Create an unordered map of string as key and vector of int as value
		// map<string, vector<int>> routing_map;
		int get_dest_port(int dest_uid);
		vector<int> arch_graph_distances;
		void create_arch_graph_distances();
		vector<int> neighbors_desc;
		vector<int> neighbors_uid;
		// UID to port mapping
		map<int, int> uid_to_port_map;
		void smart_stats();
#ifdef CACHE_MICROBENCHMARK
		long long test_cache_instx_total_count;
		long long test_cache_current_instx;
		long long test_cache_completed_instx;
		long long test_address;
		long long test_dispatched_instx_count;
		int test_nm_uid;
#endif // CACHE_MICROBENCHMARK
		
	private:
		// Pointer to GlobalCounters
		GlobalCounters *gcp;
		// X Coordinate of core
		long long x_cood;
		// Y Coordinate of core
		long long y_cood;
		// Number of ports on core
		long long port_count;
		// Cycles that the core was stalled
		long long idle_cycles;
		// Cycles that the core was stalled during the interval
		long long interval_idle_cycles;
		// Cycles that the core was computing
		long long busy_cycles;
		// Cycles that the core was computing during the interval
		long long interval_busy_cycles;
		// Interval ID
		long long interval_id;
		// Flag to indicate end of simulation
		bool sim_end;
		// Total number of operations
		long long op_count;
		// Index of current operation being performed
		long long current_op_index;
		// Map of Instructions
		// map<long long, InstX*> instx_map;
		// Map of Memory Requests
		// map<long long, MemRequest*> mem_req_map;
		// Counter to delay the processing of nect instruction
		long long processing_counter;
		// UID
		int uid;
		// NeuraCore ID
		// Accessed by Dispatcher, hence WAS public
		int id;
		// Get next instx
		InstX *get_next_instx();
		// Is register_file_available
		bool is_register_file_available();
		// Last processed pipeline
		int last_processed_pipeline;
		bool process_pipelines();
		int get_next_ready_pipeline();
		bool process_hash_request(HashRequest *hash_request);
		void rf_responder();
		// Name of the component
		string component_name;
		// Header Component Name
		string header_component_name;
		// Collection
		mongocxx::collection collection;
		// Header Collection
		mongocxx::collection header_collection;
		// Hashing Algorithm Used
		size_t address_hashing(int b_col_id, size_t total_units);
		// Exec Counter Profile Collection
		long EC_processing;
		long EC_no_instx;
		long EC_instx_fetched;
		long EC_hashreq_processed;
		long EC_rf_full;
		long EC_rf_awaiting_mem_resp;
		void insert_rf_histogram(int count);
};









#endif // !NEURA_CORE_H
