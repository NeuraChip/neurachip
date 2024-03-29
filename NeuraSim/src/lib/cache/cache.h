#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <map>
#include <list>
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include "../cacheline/cacheline.h"
#include "../counters/globalcounters.h"

using namespace std;

class Cache {
	public:
		Cache();
		Cache(GlobalCounters *gcp, int nm_id);
		~Cache();
		bool sanity_check();
		int insert_in_request_table(MemRequest *request);
		void insert_in_cache_table(long long address, int payload);
		void remove_from_request_table(long long address);
		void remove_from_cache_table(long long address);
		bool is_data_in_cache(long long address);
		void hit_routine(MemRequest *mem_req);
		MemRequest *miss_routine(MemRequest *mem_req);
		list <MemRequest *> get_mem_req_list(long long address);
		void update_cache_table(MemResponse *mem_resp);
		void custom_constructor(GlobalCounters *gcp, int id);
		MemRequest *generate_memory_request_to_mc(MemRequest *mem_req);
		bool is_request_table_empty();
	private:
	GlobalCounters *gcp;
	// Map of (Source Address) to (MemRequest *)
	// Useful to send MemResponse to ALL MemRequests waiting on the source address
	map <long long, list <MemRequest *>> request_table;
	// Map of (Address) to (Cacheline *)
	map <long long, CacheLine *> cache_table;
	int cache_size;
	// Parent NeuraMem ID
	long long nm_uid;
};

#endif // !CACHE_H