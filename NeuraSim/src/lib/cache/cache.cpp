// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "cache.h"
#include <iostream>
#include "../debugger/debugger.h"
#include "../config.h"
#include "../functions.h"

using namespace std;

// Constructor
Cache::Cache(GlobalCounters *gcp, int nm_uid)
{
	this->custom_constructor(gcp, nm_uid);
}

// Constructor
Cache::Cache()
{
}

// Custom constructor
void Cache::custom_constructor(GlobalCounters *gcp, int nm_uid)
{
	this->gcp = gcp;
	this->nm_uid = nm_uid;
}

// Destructor
Cache::~Cache()
{
}

bool Cache::sanity_check()
{
	// Sanity check
	return true;
}

int Cache::insert_in_request_table(MemRequest *request)
{
	// Create a new list if not present and insert
	if (this->request_table.find(request->get_src_address()) == this->request_table.end()) {
		list <MemRequest *> mem_req_list;
		mem_req_list.push_back(request);
		this->request_table[request->get_src_address()] = mem_req_list;
		return CACHE_REQUEST_TABLE_NEW_CREATED;
	}
	else {
		// Insert in list
		this->request_table[request->get_src_address()].push_back(request);
		return CACHE_REQUEST_TABLE_SAME_EXTENDED;
	}
}

void Cache::insert_in_cache_table(long long address, int payload)
{
	// cout << "Cache: Inserting addr: " << address << " payload: " << payload << endl;
	// If address is already present, return
	if (this->cache_table.find(address) != this->cache_table.end()) {
		cout << "WARNING: Tried to insert an address already present in cache table" << endl;
		return;
	}
	// If cache is full, evict a cacheline
	if (this->cache_table.size() >= this->gcp->CACHE_LINES_COUNT) {
		// Find the cacheline with the oldest last accessed cycle
		long long oldest_accessed_at_cycle = this->gcp->get_cycle();
		long long oldest_accessed_at_address = 0;
		for (auto it = this->cache_table.begin(); it != this->cache_table.end(); it++) {
			if (it->second->accessed_at_cycle < oldest_accessed_at_cycle) {
				oldest_accessed_at_cycle = it->second->accessed_at_cycle;
				oldest_accessed_at_address = it->first;
			}
		}
		// Remove from cache table
		this->remove_from_cache_table(oldest_accessed_at_address);
		// this->cache_table.erase(oldest_accessed_at_address);
	}
	// Dynamically allocate a new cacheline
	CacheLine *cache_line = new CacheLine(	payload,
											address,
											this->gcp->get_cycle());
	// Insert in cache table
	this->cache_table[address] = cache_line;
}

void Cache::remove_from_request_table(long long address)
{
	// Empty the list
	this->request_table[address].clear();
	// Remove from request table
	this->request_table.erase(address);
}

bool Cache::is_data_in_cache(long long address)
{
	// Check if data is in cache
	if (this->cache_table.find(address) != this->cache_table.end()) {
		return (true);
	}
	return (false);
}

void Cache::hit_routine(MemRequest *mem_req)
{
	// Get Cacheline
	CacheLine *cache_line = this->cache_table[mem_req->get_src_address()];
	// Update Last Accessed Cycle of Cacheline
	cache_line->accessed_at_cycle = this->gcp->get_cycle();
	mem_req->set_payload(cache_line->payload);
	// MemResponse will be sent by NeuraMem
}

MemRequest *Cache::miss_routine(MemRequest *incoming_mem_req)
{
	// Insert in request table
	int task_performed;
	task_performed = this->insert_in_request_table(incoming_mem_req);
	if (task_performed == CACHE_REQUEST_TABLE_NEW_CREATED) {
		// Send a memory request to memory controller
		MemRequest *outgoing_mem_req = this->generate_memory_request_to_mc(incoming_mem_req);
		// Send the memory request to memory controller is done by NeuraMem
		// this->send_memory_request_to_mc(outgoing_mem_req);
		// MemResponse will be sent by NeuraMem
		return (outgoing_mem_req);
	}
	else {
		// MemResponse will be sent by NeuraMem
		return (NULL);
	}
}

list <MemRequest *> Cache::get_mem_req_list(long long address)
{
	// Return the list of MemRequests waiting on the address
	return (this->request_table[address]);
}

void Cache::update_cache_table(MemResponse *mem_resp)
{
	// Insert in cache table
	this->insert_in_cache_table(mem_resp->get_src_address(), mem_resp->get_payload());
	// Send MemResponse to all MemRequests waiting on the address is done by NeuraMem
	return;
}

void Cache::remove_from_cache_table(long long address)
{
	// Delete the cacheline
	delete this->cache_table[address];
	// Remove from cache table
	this->cache_table.erase(address);
}

MemRequest *Cache::generate_memory_request_to_mc(MemRequest *incoming_mem_req)
{
	// Generate a memory request
	MemRequest *outgoing_mem_req = new MemRequest(	this->gcp,
											this->nm_uid,
											address_to_mc_id(this->gcp, incoming_mem_req->get_src_address()),
											MEM_REQ_READ,
											incoming_mem_req->get_src_address(),
											-1,
											4,
											-1,
											NULL);
	return (outgoing_mem_req);
}

bool Cache::is_request_table_empty()
{
	// Check if request table is empty
	if (this->request_table.empty()) {
		return (true);
	}
	return (false);
}
