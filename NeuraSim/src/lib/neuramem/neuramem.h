#ifndef NEURA_MEM_H
#define NEURA_MEM_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../config.h"
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include "../hashreq/hashrequest.h"
#include "../hashresp/hashresponse.h"
#include <queue>
#include <vector>
#include "../hashpad/hashpad.h"
#include "../cache/cache.h"
#include <map>
#include "../hashpad/hashpad.h"
#include "../mongo/mongo.h"
#include <string>

using namespace std;


// A single NeuraMem
class NeuraMem {
	public:
		// Constructor
		// @param id: NeuraMem ID
		NeuraMem(	GlobalCounters *gcp,
					long long x_cood,
					long long  y_cood,
					long long size);
		NeuraMem();
		~NeuraMem();
		void custom_constructor(	GlobalCounters *gcp,
									long long x_cood,
									long long y_cood,
									long long size);
		void print();
		queue<InstX *> *get_in_port_ref(int port_id);
		queue<InstX *> *get_out_port_ref(int port_id);
		bool tick();
		InstX *get_next_request();
		bool process_mem_reqs();
		MemRequest *generate_mem_request(	GlobalCounters *gcp,
											int src_unit_uid,
											int dest_unit_uid,
											int req_type,
											int src_address,
											int dest_address,
											int size,
											int payload,
											HashRequest *src_hash_req);
											
		MemResponse *generate_mem_response(MemRequest *src_mem_req,
											int payload);

		HashResponse *generate_hash_response(HashRequest *src_hash_req);

		// Ports have to be public because they are accessed by Connector class
		queue<InstX *> *in_port_array;
		queue<InstX *> *out_port_array;
		string get_vertex_name();
		bool sanity_check();
		int request_to_response_type(int mem_req_type);
		bool get_sim_end();
		void send_mem_resp(MemResponse *mem_resp);
		void send_hash_req(HashRequest *hash_req);
		void send_hash_resp(HashResponse *hash_resp);
		void send_mem_req(MemRequest *mem_req);
		void hash_evicts();
		void eviction_routine(HashRequest *hash_req);
		bool return_routine(bool sim_end, bool computed);
		// int get_port_from_uid(int dest_uid);
		void reply_multiple_mem_reqs(MemResponse *mem_resp);
		void post_sim_metrics();
		void interval_metrics();
		int get_uid();
		int get_id();
		bool are_hash_pads_empty();
		bool is_cache_request_table_empty();
		int get_writer_uid_from_table_id(int table_id);
		int get_mc_uid();
		string vertex_name;
		int get_dest_port(int dest_uid);
		vector<int> arch_graph_distances;
		void create_arch_graph_distances();
		vector<int> neighbors_desc;
		vector<int> neighbors_uid;
		// UID to port mapping
		map<int, int> uid_to_port_map;
		void statistics();
		void smart_stats();
#ifdef CACHE_MICROBENCHMARK
		long long test_cache_hit_count;
		long long test_cache_miss_count;
#endif // CACHE_MICROBENCHMARK
	private:
		// NeuraMem ID
		long long id;
		int uid;
		long long x_cood;
		long long y_cood;
		GlobalCounters *gcp;
		long long size;
		uint8_t *data;
		long long port_count;
		bool sim_end;
		long long processing_counter;
		long long idle_cycles;
		long long interval_busy_cycles;
		long long busy_cycles;
		long long interval_idle_cycles;
		long long interval_id;
		int last_processed_port;
		vector<InstX *> processing_array;
		// HashPad **hashpad_array;
		Cache cache;
		bool initialized;
		// Map of (Table ID) to (HashPad *)
		map <long long, HashPad *> hashpad_map;
		// Queue of HashRequests that are waiting to be evicted
		queue <HashRequest *> eviction_queue;
		string component_name;
		string header_component_name;
		long long collisions;
		// Collection
		mongocxx::collection collection;
		// Collection for header
		mongocxx::collection header_collection;
		// Exec Counter Profile
		long EC_processing;
		long EC_evicting_hash_req;
		long EC_no_instx_hashpads_empty;
		long EC_no_instx_hashpads_waiting;
		long EC_cache_hit;
		long EC_cache_miss;
		long EC_mem_resp;
		long EC_hash_hit;
		long EC_hash_miss;
		int last_writer_id;

};

#endif // !NEURA_MEM_H
