// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "l2cache.h"

/*
class L2Cache {
	public:
		L2Cache();
		L2Cache(GlobalCounters *gcp, int nm_id);
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
*/

// Constructor
L2Cache::L2Cache(GlobalCounters *gcp, int nm_id)
{
	this->custom_constructor(gcp, nm_id);
}

// Constructor
L2Cache::L2Cache()
{
}

// Custom constructor
void L2Cache::custom_constructor(GlobalCounters *gcp)
{
	this->gcp = gcp;
	this->cacheline_count = 0;
}

// Destructor
L2Cache::~L2Cache()
{
}

bool L2Cache::sanity_check()
{
	// Sanity check
	return true;
}

