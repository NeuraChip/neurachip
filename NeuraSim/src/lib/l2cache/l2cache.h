#ifndef L2CACHE_H
#define L2CACHE_H

#include <iostream>
#include <map>
#include <list>
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include "../cacheline/cacheline.h"
#include "../counters/globalcounters.h"

using namespace std;

class L2Cache {
	public:
		L2Cache();
		L2Cache(GlobalCounters *gcp);
		~L2Cache();
		bool sanity_check();
		int insert_in_request_table(MemRequest *request);
		void insert_in_cache_table(long long address, int payload);
		void remove_from_request_table(long long address);
		void remove_from_cache_table(long long address);
		bool is_data_in_cache(long long address);
		void hit_routine(MemRequest *mem_req);
		MemRequest *miss_routine(MemRequest *mem_req);
		void custom_constructor(GlobalCounters *gcp, int id);
		void evict_cacheline();
	private:
	GlobalCounters *gcp;
	// Map of (Address) to (Cacheline *)
	map <long long, CacheLine *> cache_table;
	// Number of cachelines in the L2-Cache
	int cacheline_count;
};


#endif // !L2CACHE_H