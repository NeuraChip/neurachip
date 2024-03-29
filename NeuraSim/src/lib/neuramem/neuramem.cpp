// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "neuramem.h"
#include "../debugger/debugger.h"
#include "../counters/globalcounters.h"
#include "../config.h"
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include "../hashreq/hashrequest.h"
#include "../functions.h"

using namespace std;

NeuraMem::NeuraMem(GlobalCounters *gcp, long long x_cood, long long y_cood, long long size)
{
	this->custom_constructor(gcp, x_cood, y_cood, size);
}

NeuraMem::NeuraMem()
{
}

void NeuraMem::custom_constructor(GlobalCounters *gcp, long long x_cood, long long y_cood, long long size)
{
	this->gcp = gcp;
	this->id = this->gcp->generate_mem_id();
	this->uid = this->gcp->generate_uid();
	this->gcp->set_mem_id_array(this->id, this->uid);
	this->size = size;
	this->x_cood = x_cood;
	this->y_cood = y_cood;
	this->data = new uint8_t[size];
	this->port_count = this->gcp->NEURA_MEM_PORT_COUNT;
	this->in_port_array = new queue<InstX *>[this->port_count];
	this->out_port_array = new queue<InstX *>[this->port_count];
	this->sim_end = false;
	this->processing_counter = 0;
	this->idle_cycles = 0;
	this->interval_idle_cycles = 0;
	this->busy_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_id = 0;
	this->last_processed_port = 0;
	this->initialized = true;
	this->cache.custom_constructor(this->gcp, this->uid);
	this->component_name = "NeuraMem_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->header_component_name = "H_" + this->component_name;
	this->collisions = 0;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
	this->vertex_name = "M_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->gcp->add_vertex_to_arch_graph(this->vertex_name, this->uid);
	this->EC_processing = 0;
	this->EC_evicting_hash_req = 0;
	this->EC_no_instx_hashpads_empty = 0;
	this->EC_no_instx_hashpads_waiting = 0;
	this->EC_cache_hit = 0;
	this->EC_cache_miss = 0;
	this->EC_mem_resp = 0;
	this->EC_hash_hit = 0;
	this->EC_hash_miss = 0;
	this->last_writer_id = 1;
#ifdef CACHE_MICROBENCHMARK
	this->test_cache_hit_count = 0;
	this->test_cache_miss_count = 0;
#endif // CACHE_MICROBENCHMARK
}

NeuraMem::~NeuraMem()
{
	delete[] this->data;
	delete[] this->in_port_array;
	delete[] this->out_port_array;
	// Iterate over hashpad_map and delete all hashpads
	for (auto it = this->hashpad_map.begin(); it != this->hashpad_map.end(); it++)
	{
		delete it->second;
	}
}

void NeuraMem::print()
{
	cout << "NeuraMem ID: " << this->id << endl;
	cout << "NeuraMem Size: " << this->size << endl;
}

queue<InstX *> *NeuraMem::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

queue<InstX *> *NeuraMem::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

/*
  _____ ___ ____ _  __
 |_   _|_ _/ ___| |/ /
   | |  | | |   | ' /
   | |  | | |___| . \
   |_| |___\____|_|\_\

*/

bool NeuraMem::tick()
{
	if (this->processing_counter > 0)
	{
		this->processing_counter--;
		this->EC_processing++;
#ifndef SKIP_NEURAMEM_PROCESSING_COUNTER
		return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
#endif // SKIP_NEURAMEM_PROCESSING_COUNTER
	}
	// Check if eviction queue is not empty
	// Eviction consumes 1 cycle
	// TODO: Can be reduced by hardware support
	if (this->eviction_queue.size() > 0)
	{
		// Evict the first element
		HashRequest *hash_req = this->eviction_queue.front();
		// cout << "Evicting Table:" << hash_req->get_table_id() << " B Col ID: " << hash_req->get_b_col_id() << endl;
		this->eviction_queue.pop();
		this->eviction_routine(hash_req);
		this->EC_evicting_hash_req++;
		return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED)); // TODO: If hardware support is provided. You dont need to retuen here.
	}
	// Get the next request
	InstX *instx = this->get_next_request();
	if (instx == NULL)
	{ // No request to process
		if (this->are_hash_pads_empty() && this->is_cache_request_table_empty())
		{
			this->EC_no_instx_hashpads_empty++;
			return (this->return_routine(SIM_END_TRUE, MEM_STALLED));
		}
		else
		{
			this->EC_no_instx_hashpads_waiting++;
			return (this->return_routine(SIM_END_FALSE, MEM_STALLED));
		}
	}
	else
	{
		switch (instx->get_type())
		{
		// MemRequest
		case INSTX_TYPE_MEM_REQ:
		{
			MemRequest *mem_req = (MemRequest *)instx;
			// Check if the data is in cache
			if (this->cache.is_data_in_cache(mem_req->get_src_address()))
			{
#ifdef TRACE_INSTX
				cout << "Cache Hit: " << mem_req->get_src_address() << endl;
#endif
#ifdef CACHE_MICROBENCHMARK
				this->test_cache_hit_count++;
#endif // CACHE_MICROBENCHMARK
	   // Data is in cache
				this->cache.hit_routine(mem_req);
				// Generate MemResponse
				// Cache hit routine will update the payload in the same mem_req
				MemResponse *mem_resp = this->generate_mem_response(mem_req, mem_req->get_payload());
				// Push the response to the output port
				this->send_mem_resp(mem_resp);
				this->processing_counter += 1; // TODO: Check if this is required
				this->EC_cache_hit++;
				return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
			}
			else
			{
#ifdef TRACE_INSTX
				cout << "Cache Miss: " << mem_req->get_src_address() << endl;
#endif
#ifdef CACHE_MICROBENCHMARK
				this->test_cache_miss_count++;
#endif // CACHE_MICROBENCHMARK
	   // Data is not in cache
				MemRequest *outgoing_mem_req = this->cache.miss_routine(mem_req);
				if (outgoing_mem_req != NULL)
				{
					// Push the request to the output port
					this->send_mem_req(outgoing_mem_req);
					this->EC_cache_miss++;
					return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
				}
				else
				{
					return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
				}
			}
		}
		// MemResponse
		case INSTX_TYPE_MEM_RESP:
		{
			MemResponse *mem_resp = (MemResponse *)instx;
			// cout << "NeuraMem: MemResponse Received: " << mem_resp->get_src_address() << " with data: " << mem_resp->get_payload() << endl;
			this->cache.update_cache_table(mem_resp);
			this->reply_multiple_mem_reqs(mem_resp);
			this->EC_mem_resp++;
			return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
		}
		// HashRequest
		case INSTX_TYPE_HASH_REQ:
		{
			HashRequest *hash_req = (HashRequest *)instx;
			// cout << "Hash Request Received Table:" << hash_req->get_table_id() << " Col ID:" << hash_req->get_b_col_id() << " Count:" << hash_req->get_count() << endl;
			// Get col id
			long long col_id = hash_req->get_b_col_id();
			long long table_id = hash_req->get_table_id();
			if (col_id == -1)
			{
				cout << FATAL_ERROR << "HashRequest: B Col ID is -1" << endl;
			}
			if (table_id == -1)
			{
				cout << FATAL_ERROR << "HashRequest: Table ID is -1" << endl;
			}
			// If the table is not initialized, initialize it
			if (this->hashpad_map.find(table_id) == this->hashpad_map.end())
			{
				// cout << "Initializing HashPad for Table: " << table_id << endl;
				HashPad *hashpad = new HashPad();
				// FIXME: For now the size is 3000. Need to read it from CSV file
				hashpad->custom_constructor(300000, hash_req->get_table_id());
				this->hashpad_map[table_id] = hashpad;
			}
			HashPad *hashpad = this->hashpad_map[table_id];
			// Check if the data is in HashTable
			// And choose between two routines
			// Hit Routine and Miss Routine
			if (hashpad->is_data_in_hash_table(col_id))
			{ // Hit Routine
				// Data is in HashTable
				long long single_insert_collisions = hashpad->hit_routine(hash_req);
				if (hashpad->check_eviction_condition(col_id))
				{
					this->eviction_queue.push(hash_req);
				}
				// Number of cycles to do hash table walk (in addition to the initial 1 cycle)
				// HASHPAD_COMPARATOR_COUNT will compare those positions in parallel
				this->processing_counter += ((single_insert_collisions + 1) / this->gcp->HASHPAD_COMPARATOR_COUNT);
				this->collisions += single_insert_collisions;
				this->EC_hash_hit++;
				return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
			}
			else
			{
				// Data is not in HashTable
				long long single_insert_collisions = hashpad->miss_routine(hash_req);
				if (hashpad->check_eviction_condition(col_id))
				{
					this->eviction_queue.push(hash_req);
				}
				// Number of cycles to do hash table walk (in addition to the initial 1 cycle)
				// HASHPAD_COMPARATOR_COUNT will compare those positions in parallel
				this->processing_counter += ((single_insert_collisions + 1) / this->gcp->HASHPAD_COMPARATOR_COUNT);
				this->collisions += single_insert_collisions;
				this->EC_hash_miss++;
				return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
			}
			// Should not reach here
			cout << FATAL_ERROR << "NeuraMem: Should not reach here" << endl;
			return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
		}
		// HashResponse
		case INSTX_TYPE_HASH_RESP:
		{
			// TODO: Pending implementation
			// HashResponse *hash_resp = (HashResponse *)instx;
			/* code */
			cout << FATAL_ERROR << "NeuraMem: HashResponse not implemented" << endl;
			return (this->return_routine(SIM_END_FALSE, MEM_COMPUTED));
		}
		default:
		{
			cout << "ERROR: Invalid InstX type" << endl;
			exit(1);
		}
		}
	}
	cout << FATAL_ERROR << "NeuraMem: Should not reach here" << endl;
	// this->sim_end = this->process_mem_reqs(); //TODO: Pending implementation
	// this->sim_end = true;
	return this->sim_end;
}

// Gets the next instruction from all ports OR NULL if no instruction is available
// @return: Pointer to the next instruction
// @return: NULL if no instruction is available
// @param: None
InstX *NeuraMem::get_next_request()
{
	for (int i = 0; i < this->port_count; i++)
	{
		int port_id = (this->last_processed_port + i) % this->port_count;
		if (this->in_port_array[port_id].size() > 0)
		{
			InstX *instx = this->in_port_array[port_id].front();
			this->in_port_array[port_id].pop();
			this->last_processed_port = port_id;
			// Instruction fetch by NeuraMem consumes 1 cycle
			this->processing_counter += 1;
			return instx;
		}
	}
	// No instruction available
	return NULL;
}

/* //TODO: Pending implementation
bool NeuraMem::process_mem_reqs() {
	bool sim_end = true;
	for (int i = 0; i < this->processing_array.size(); i++) {
		// Tick the mem_req
		MemRequest *mem_req = this->processing_array[i];
		if (mem_req->tick()) {
			// MemRequest is done
			// Remove it from processing_array
			this->processing_array.erase(this->processing_array.begin() + i);
			// Push the response
			this->out_port_array[mem_req->get_port_id()].push(mem_req);
			// Decrement i
			i--;
		}
		else {
			sim_end = false;
		}
	}
}
*/

MemRequest *NeuraMem::generate_mem_request(GlobalCounters *gcp,
										   int src_unit_uid,
										   int dest_unit_uid,
										   int req_type,
										   int src_address,
										   int dest_address,
										   int size,
										   int payload,
										   HashRequest *src_hash_req)
{
	MemRequest *mem_req = new MemRequest(gcp,
										 src_unit_uid,
										 dest_unit_uid,
										 req_type,
										 src_address,
										 dest_address,
										 size,
										 payload,
										 src_hash_req);
	return (mem_req);
}

int NeuraMem::get_mc_uid()
{
	// TODO: Pending implementation
	return this->gcp->get_mc_id_array()[0];
}

int NeuraMem::request_to_response_type(int mem_req_type)
{
	switch (mem_req_type)
	{
	case MEM_REQ_READ:
		return MEM_RESP_READ;
	case MEM_REQ_READ_NO_CACHE:
		return MEM_RESP_READ_NO_CACHE;
	case MEM_REQ_WRITE:
		return MEM_RESP_WRITE;
	case MEM_REQ_MOVE:
		return MEM_RESP_MOVE;
	case MEM_REQ_COPY:
		return MEM_RESP_COPY;
	case MEM_REQ_SCATTER:
		return MEM_RESP_SCATTER;
	case MEM_REQ_GATHER:
		return MEM_RESP_GATHER;
	case MEM_REQ_BCAST:
		return MEM_RESP_BCAST;
	case MEM_REQ_REDUCE:
		return MEM_RESP_REDUCE;
	default:
		return -1;
	}
}

MemResponse *NeuraMem::generate_mem_response(MemRequest *src_mem_req, int payload)
{

	MemResponse *mem_resp = new MemResponse(this->gcp,													 // GlobalCounters *gcp,
											this->id,													 // int src_unit_uid,
											src_mem_req->get_src_unit_uid(),							 // int dest_unit_uid,
											this->request_to_response_type(src_mem_req->get_req_type()), // int resp_type,
											src_mem_req->get_src_address(),								 // int src_address,
											src_mem_req->get_size(),									 // int size,
											payload,													 // int payload,
											src_mem_req->get_id(),										 // long long src_mem_req_id,
											src_mem_req,												 // MemRequest *src_mem_req,
											src_mem_req->get_src_hash_req());							 // HashRequest *src_hash_req

	mem_resp->set_operand_id(src_mem_req->get_operand_id());
	// cout << "NeuraMem::generate_mem_response() - payload: " << mem_resp->get_payload() << endl;
	return (mem_resp);
}

bool NeuraMem::sanity_check()
{
	if (this->initialized == false)
	{
		cout << "NeuraMem::sanity_check() - ERROR: Not initialized" << endl;
		return false;
	}
	// Check if all the ports are empty
	for (int i = 0; i < this->port_count; i++)
	{
		if (this->in_port_array[i].size() > 0)
		{
			cout << "NeuraMem::sanity_check() - ERROR: in_port_array[" << i << "] is not empty" << endl;
			return false;
		}
		if (this->out_port_array[i].size() > 0)
		{
			cout << "NeuraMem::sanity_check() - ERROR: out_port_array[" << i << "] is not empty" << endl;
			return false;
		}
	}
	return true;
}

bool NeuraMem::get_sim_end()
{
	return this->sim_end;
}

void NeuraMem::send_mem_resp(MemResponse *mem_resp)
{
// int dest_port_id = this->get_port_from_uid(mem_resp->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << "NeuraMem::send_mem_resp() - dest: " << this->gcp->uid_to_vertex_name_map[mem_resp->get_dest_unit_uid()] << endl;
#endif
	int dest_port_id = this->get_dest_port(mem_resp->get_dest_unit_uid());
	// cout << "NeuraMem::send_mem_resp() - dest_uid: " << mem_resp->get_dest_unit_uid() << " - dest_port_id: " << dest_port_id << endl;
	this->out_port_array[dest_port_id].push(mem_resp); // FIXME: Hardcoded port
}

void NeuraMem::send_hash_req(HashRequest *hash_req)
{
// int dest_port_id = this->get_port_from_uid(hash_req->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << "NeuraMem::send_hash_req() - dest: " << this->gcp->uid_to_vertex_name_map[hash_req->get_dest_unit_uid()] << endl;
#endif
	int dest_port_id = this->get_dest_port(hash_req->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(hash_req); // FIXME: Hardcoded port
}

void NeuraMem::send_mem_req(MemRequest *mem_req)
{
// int dest_port_id = this->get_port_from_uid(mem_req->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << "NeuraMem::send_mem_req() - dest: " << this->gcp->uid_to_vertex_name_map[mem_req->get_dest_unit_uid()] << endl;
#endif
	int dest_port_id = this->get_dest_port(mem_req->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(mem_req); // FIXME: Hardcoded port
}

void NeuraMem::send_hash_resp(HashResponse *hash_resp)
{
// int dest_port_id = this->get_port_from_uid(hash_resp->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << "NeuraMem::send_hash_resp() - dest: " << this->gcp->uid_to_vertex_name_map[hash_resp->get_dest_unit_uid()] << endl;
#endif
	int dest_port_id = this->get_dest_port(hash_resp->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(hash_resp); // FIXME: Hardcoded port
}

void NeuraMem::hash_evicts()
{
	return;
}

void NeuraMem::eviction_routine(HashRequest *hash_req)
{
	// Generate a Hash Response
	HashResponse *hash_resp = this->generate_hash_response(hash_req);
	// Send the Hash Response
#ifndef DISABLE_NM_HASH_RESPONSES
	this->send_hash_resp(hash_resp);
#endif // DISABLE_NM_HASH_RESPONSES
	this->hashpad_map[hash_req->get_table_id()]->evict_hash_line(hash_req->get_b_col_id());
}

HashResponse *NeuraMem::generate_hash_response(HashRequest *hash_req)
{
	HashResponse *hash_resp = new HashResponse(this->gcp,													 // GlobalCounters *gcp,
											   this->id,													 // int src_unit_uid,
											   this->get_writer_uid_from_table_id(hash_req->get_table_id()), // int dest_unit_uid,
											   hash_req->get_table_id(),									 // long long table_id,
											   hash_req->get_b_col_id(),									 // long long b_col_id
											   hash_req->get_mult_result(),									 // int mult_result,
											   hash_req														 // HashRequest *src_hash_req,
	);
	return (hash_resp);
}

int NeuraMem::get_writer_uid_from_table_id(int table_id)
{
	// TODO: Pending implementation, need better hashing
	
	// int writer_id = table_id % this->gcp->get_total_writers();
	// Random ID

	int writer_id = rand() % this->gcp->get_total_writers();
	int writer_uid = this->gcp->get_writer_id_array()[writer_id];
	return writer_uid;

	// Round Robin
	/*
	if (this->last_writer_id == 0)
	{
		this->last_writer_id = 1;
		return (this->gcp->get_writer_id_array()[1]);
	}
	else
	{
		this->last_writer_id = 0;
		return (this->gcp->get_writer_id_array()[0]);
	}
	*/
}

bool NeuraMem::return_routine(bool sim_end, bool computed)
{
	if (computed == true)
	{
		this->busy_cycles++;
		this->interval_busy_cycles++;
	}
	else
	{
		this->idle_cycles++;
		this->interval_idle_cycles++;
	}
	this->sim_end = sim_end;
	return this->sim_end;
}

// int NeuraMem::get_port_from_uid(int uid)
// {
// 	const int core_uid = this->gcp->get_core_id_array()[0];
// 	const int mem_uid = this->gcp->get_mem_id_array()[0];
// 	const int router_uid = this->gcp->get_router_id_array()[0];
// 	const int dispatcher_uid = this->gcp->get_dispatcher_id_array()[0];
// 	const int writer_uid = this->gcp->get_writer_id_array()[0];
// 	const int mc_uid = this->gcp->get_mc_id_array()[0];
// 	if (uid == core_uid)
// 	{
// 		// cout << "Sending to Core" << endl;
// 		return 0;
// 	}
// 	else if (uid == mem_uid)
// 	{
// 		cout << FATAL_ERROR << "NeuraMem::get_port_from_uid()" << endl;
// 		return -1;
// 	}
// 	else if (uid == router_uid)
// 	{
// 		// cout << "Sending to Router" << endl;
// 		cout << "Pending Implementation" << endl;
// 		return -1;
// 	}
// 	else if (uid == dispatcher_uid)
// 	{
// 		cout << FATAL_ERROR << "NeuraMem::get_port_from_uid()" << endl;
// 		return -1;
// 	}
// 	else if (uid == writer_uid)
// 	{
// 		// cout << "Sending to Writer" << endl;
// 		return 2;
// 	}
// 	else if (uid == mc_uid)
// 	{
// 		// cout << "Sending to MC" << endl;
// 		return 1;
// 	}
// 	else
// 	{
// 		return -1;
// 	}
// }

void NeuraMem::reply_multiple_mem_reqs(MemResponse *incoming_mem_resp)
{
	// Get the list of MemRequests
	list<MemRequest *> mem_req_list = this->cache.get_mem_req_list(incoming_mem_resp->get_src_address());
	// Iterate through the list and reply to all the MemRequests
	for (list<MemRequest *>::iterator it = mem_req_list.begin(); it != mem_req_list.end(); it++)
	{
		MemRequest *mem_req = *it;
		// Generate a MemResponse
		string type;
		if (mem_req->type == MEM_REQ_READ)
		{
			type = "READ";
		}
		else if (mem_req->type == MEM_REQ_WRITE)
		{
			type = "WRITE";
		}
		else
		{
			type = "UNKNOWN";
		}
		// cout << "Generating a MemResponse to Mem Request" << type << " :" << mem_req->get_id() << " Src Address:" << mem_req->get_src_address() << " Dest Address:" << mem_req->get_dest_address() << endl;
		MemResponse *outgoing_mem_resp = this->generate_mem_response(mem_req, incoming_mem_resp->get_payload());
		// Send the MemResponse
		this->send_mem_resp(outgoing_mem_resp);
	}
	int processing_counter_to_add = (mem_req_list.size() - 1);
	if (processing_counter_to_add > 0)
	{
		this->processing_counter += processing_counter_to_add;
	}
	// Remove the MemRequests from the request table
	this->cache.remove_from_request_table(incoming_mem_resp->get_src_address());
	return;
}

void NeuraMem::post_sim_metrics()
{
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("x_cood", (long)this->x_cood),
			kvp("y_cood", (long)this->y_cood),
			kvp("busy_cycles", (long)this->busy_cycles),
			kvp("idle_cycles", (long)this->idle_cycles)));

		this->header_collection.insert_one(make_document(
			kvp("name", "exec_counter_profile"),
			kvp("x_cood", (long)this->x_cood),
			kvp("y_cood", (long)this->y_cood),
			kvp("processing", (long)this->EC_processing),
			kvp("evicting_hash_req", (long)this->EC_evicting_hash_req),
			kvp("no_instx_hashpads_empty", (long)this->EC_no_instx_hashpads_empty),
			kvp("no_instx_hashpads_waiting", (long)this->EC_no_instx_hashpads_waiting),
			kvp("cache_hit", (long)this->EC_cache_hit),
			kvp("cache_miss", (long)this->EC_cache_miss),
			kvp("mem_resp", (long)this->EC_mem_resp),
			kvp("hash_hit", (long)this->EC_hash_hit),
			kvp("hash_miss", (long)this->EC_hash_miss)));
	}
#ifdef CACHE_MICROBENCHMARK
	if (this->x_cood == 0 && this->y_cood == 0)
	{
		cout << "Cache Hit Count: " << this->test_cache_hit_count << endl;
		cout << "Cache Miss Count: " << this->test_cache_miss_count << endl;
	}
#endif // CACHE_MICROBENCHMARK
	return;
}

void NeuraMem::interval_metrics()
{
	if (MONGO_FLAG)
	{
		this->collection.insert_one(make_document(
			kvp("name", "interval_metrics"),
			kvp("interval_id", (long)this->interval_id),
			kvp("interval_busy_cycles", (long)this->interval_busy_cycles),
			kvp("interval_idle_cycles", (long)this->interval_idle_cycles)));
	}
	this->interval_busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_id++;
	return;
}

int NeuraMem::get_id()
{
	return this->id;
}

int NeuraMem::get_uid()
{
	return this->uid;
}

bool NeuraMem::are_hash_pads_empty()
{
	// Iterate over hashpad_map
	for (map<long long, HashPad *>::iterator it = this->hashpad_map.begin(); it != this->hashpad_map.end(); it++)
	{
		HashPad *hashpad = it->second;
		if (hashpad->is_empty == false)
		{
			return false;
		}
		// if (hashpad->is_hash_pad_empty() == false)
		// {
		// 	return false;
		// }
	}
	return true;
}

bool NeuraMem::is_cache_request_table_empty()
{
	return this->cache.is_request_table_empty();
}

// @param dest_uid: Destination Unit UID
// @return: Port ID to send data on
// @description: Finds the best port to send data using the best routing algorithm
int NeuraMem::get_dest_port(int dest_uid)
{
	int min_distance_port = -1;
	min_distance_port = packet_routing(dest_uid, this->gcp, &this->neighbors_uid, &this->uid_to_port_map);
	return (min_distance_port);

	// // Get the destination unit's vertex descriptor
	// auto dest_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[dest_uid];

	// // this->neighbors_uid is a vector of all the neighbors of this unit
	// vector<int> neighbor_distances_to_dest;
	// // Iterate over all the neighbors uids of this unit
	// for (auto neighbor_uid : this->neighbors_uid)
	// {
	// 	// Get the arch_graph_distances vector of the neighbor
	// 	auto neighbor_distances_ptr = this->gcp->uid_to_arch_graph_distances_ptr_map[neighbor_uid];
	// 	auto neighbor_distance_to_dest = (*neighbor_distances_ptr)[dest_unit_vertex_desc];
	// 	neighbor_distances_to_dest.push_back(neighbor_distance_to_dest);
	// }

	// // This is X-Y routing
	// // Find the port with the minimum distance to the destination
	// // TODO: Use neighbor_distances_to_dest to do adaptive routing
	// int min_distance = INT_MAX;
	// int min_distance_port = -1;
	// for (int i = 0; i < neighbor_distances_to_dest.size(); i++)
	// {
	// 	if (neighbor_distances_to_dest[i] < min_distance)
	// 	{
	// 		min_distance = neighbor_distances_to_dest[i];
	// 		min_distance_port = this->uid_to_port_map[this->neighbors_uid[i]];
	// 	}
	// }

	// return (min_distance_port);
}

void NeuraMem::create_arch_graph_distances()
{
	arch_distancing(this->gcp,
					this->arch_graph_distances,
					this->vertex_name,
					this->uid,
					this->neighbors_desc,
					this->neighbors_uid);
	// int total_vertices = boost::num_vertices(this->gcp->arch_graph);
	// this->arch_graph_distances.resize(total_vertices);
	// // cout << "Vertex name: " << this->vertex_name << endl;
	// boost::dijkstra_shortest_paths(this->gcp->arch_graph,
	// 							   this->gcp->vertex_name_to_desc_map[this->vertex_name],
	// 							   boost::distance_map(&this->arch_graph_distances[0]));

	// this->gcp->uid_to_arch_graph_distances_ptr_map[this->uid] = &this->arch_graph_distances;

	// // Source vertex descriptor
	// auto src_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[this->uid];

	// // Find the neighbors of this vertex
	// auto neighbors_iter_pair = boost::adjacent_vertices(src_unit_vertex_desc, this->gcp->arch_graph);
	// for (auto iter = neighbors_iter_pair.first; iter != neighbors_iter_pair.second; iter++)
	// {
	// 	this->neighbors_desc.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// }

	// // Create neighbor_uid vector
	// for (auto iter = this->neighbors_desc.begin(); iter != this->neighbors_desc.end(); iter++)
	// {
	// 	this->neighbors_uid.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// }
}

string NeuraMem::get_vertex_name()
{
	return (this->vertex_name);
}

void NeuraMem::statistics()
{
	cout << "************* " << RED_START << "NeuraMem" << COLOR_RESET << " *************" << endl;
	cout << "Mem[" << this->x_cood;
	cout << "][" << this->y_cood << "] ";
	cout << "ID: " << this->id;
	cout << " UID: " << this->uid << endl;
	cout << RED_START << "Stalls" COLOR_RESET << ": " << this->idle_cycles << "\t";
	cout << GREEN_START << "Compute" << COLOR_RESET << ": " << this->busy_cycles << endl;
	cout << "Virtual HashPads: " << YELLOW_START << this->hashpad_map.size() << COLOR_RESET << " ";
	cout << "HashPad Sizes: ";
	if (this->hashpad_map.size() == 0)
	{
		cout << "No HashPads" << endl;
	}
	else
	{
		// Iterate over hashpad_map and print size of each hashpad
		cout << "[";
		for (auto hashpad : this->hashpad_map)
		{
			HashPad *hashpad_ptr = hashpad.second;
			int current_hash_lines = hashpad_ptr->get_current_hash_lines();
			if (current_hash_lines > 0)
			{
				cout << RED_START << current_hash_lines << " " << COLOR_RESET;
			}
			else
			{
				cout << GREEN_START << current_hash_lines << " " << COLOR_RESET;
			}
		}
		cout << "]" << endl;
	}
	if (this->hashpad_map.size() > 0)
	{
		for (auto hashpad : this->hashpad_map)
		{
			HashPad *hashpad_ptr = hashpad.second;
			if (hashpad_ptr->get_current_hash_lines() > 0)
			{
				hashpad_ptr->statistics();
			}
		}
	}
	cout << "Neighbors: " << this->neighbors_uid.size() << endl;
	cout << "Neighbors: ";
	for (auto neighbor_uid : this->neighbors_uid)
	{
		cout << this->gcp->uid_to_vertex_name_map[neighbor_uid] << " ";
	}
	cout << endl;
	cout << "Port Pressure: ";
	for (int i = 0; i < this->port_count; i++)
	{
		cout << "[";
		cout << CYAN_START << this->in_port_array[i].size() << COLOR_RESET;
		cout << "/";
		cout << CYAN_START << this->out_port_array[i].size() << COLOR_RESET;
		cout << "]  ";
	}
	cout << endl;
	cout << "*************************************" << endl;
}

void NeuraMem::smart_stats()
{
	// TODO: Pending implementation
}