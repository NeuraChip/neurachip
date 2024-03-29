// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "neuracore.h"
#include "../debugger/debugger.h"
#include "../memreq/memrequest.h"
#include "../config.h"
#include "../hashreq/hashrequest.h"
#include "../memresp/memresponse.h"
#include "../functions.h"

using namespace std;

NeuraCore::NeuraCore(GlobalCounters *gcp,
					 int x_cood,
					 int y_cood,
					 int port_count)
{
	this->custom_constructor(gcp, x_cood, y_cood, port_count);
}

NeuraCore::NeuraCore()
{
}

void NeuraCore::custom_constructor(GlobalCounters *gcp,
								   int x_cood,
								   int y_cood,
								   int port_count)
{
	this->gcp = gcp;
	this->id = this->gcp->generate_core_id();
	this->uid = this->gcp->generate_uid();
	this->gcp->set_core_id_array(this->id, this->uid);
	this->x_cood = x_cood;
	this->y_cood = y_cood;
	// cout << "NeuraCore[" << this->x_cood << "][" << this->y_cood << "] ID: " << this->id << " UID: " << this->uid << endl;
	this->port_count = port_count;
	this->in_port_array = new queue<InstX *>[port_count];
	this->out_port_array = new queue<InstX *>[port_count];
	this->idle_cycles = 0;
	this->busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->sim_end = false;
	this->op_count = 100000000;
	this->current_op_index = 0;
	this->processing_counter = 0;
	this->last_processed_pipeline = -1;
	this->component_name = "NeuraCore_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);

	this->interval_id = 0;
	this->vertex_name = "C_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->gcp->add_vertex_to_arch_graph(vertex_name, this->uid);
	this->EC_processing = 0;
	this->EC_no_instx = 0;
	this->EC_instx_fetched = 0;
	this->EC_hashreq_processed = 0;
	this->EC_rf_full = 0;
	this->EC_rf_awaiting_mem_resp = 0;

#ifdef CACHE_MICROBENCHMARK
	this->test_cache_instx_total_count = 3;
	this->test_cache_current_instx = 0;
	this->test_address = 0;
	this->test_dispatched_instx_count = 0;
	this->test_cache_completed_instx = 0;
#endif // CACHE_MICROBENCHMARK
}

NeuraCore::~NeuraCore()
{
	delete[] this->in_port_array;
	delete[] this->out_port_array;
}

// Print NeuraCore Information
void NeuraCore::print()
{
	cout << "NeuraCore ID: " << this->id << endl;
	cout << "NeuraCore X Coordinate: " << this->x_cood << endl;
	cout << "NeuraCore Y Coordinate: " << this->y_cood << endl;
}

int NeuraCore::get_id()
{
	return this->id;
}

int NeuraCore::get_uid()
{
	return this->uid;
}

// Test Ports
bool NeuraCore::test_ports()
{
	// for (int i=0; i<this->port_count; i++) {
	// Create a new memory request
	if (this->op_count == 0)
	{
		return true;
	}
	MemRequest *mem_req = this->generate_mem_request(this->gcp,
													 this->uid,
													 -1,
													 MEM_REQ_READ,
													 0,
													 8,
													 4,
													 0xdeadbeef,
													 NULL);
	// cout << "Pushing MEM REQ to port 1" << endl;
	this->out_port_array[1].push(mem_req);
	// Create a new hash request
	HashRequest *hash_req = this->generate_hash_request(0,
														0xa,
														0xb,
														0xc,
														10,
														5);
	// cout << "Pushing HASH REQ to port 1" << endl;
	this->out_port_array[1].push(hash_req);
	this->op_count--;
	return false;
	// }
}

// Get a reference to an input port
// @param port_id: Port ID
// @return: Reference to input port
queue<InstX *> *NeuraCore::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

// Get a reference to an output port
// @param port_id: Port ID
// @return: Reference to output port
queue<InstX *> *NeuraCore::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

// Tick
// @return: True if simulation is over
bool NeuraCore::tick()
{
	// this->statistics();
	// Computing some instruction
	if (this->processing_counter > 0)
	{
		this->processing_counter--;
		this->EC_processing++;
		return (this->return_routine(false, CORE_COMPUTED));
	}

	// Cache Microbenchmark
#ifdef CACHE_MICROBENCHMARK
	// Only launch on Core 0 0
	if (this->x_cood == 0 && this->y_cood == 0)
	{
		const int neuramem_id = 0;
		this->test_nm_uid = this->gcp->get_mem_id_array()[neuramem_id];

		if (this->test_dispatched_instx_count >= this->test_cache_instx_total_count)
		{
			return this->return_routine(true, CORE_COMPUTED);
		}
		MemRequest *mem_req = this->generate_mem_request(this->gcp,
														 this->uid,
														 test_nm_uid,
														 MEM_REQ_READ,
														 this->test_address,
														 -1,
														 4,
														 -1,
														 NULL);
		mem_req->set_operand_id(HR_OPERAND_A_DATA_ADDR);
		this->send_mem_request(mem_req);
		this->test_address += 0;
		this->test_dispatched_instx_count++;
		return this->return_routine(false, CORE_COMPUTED);
	}

	return this->return_routine(true, CORE_STALLED);
	// Cache Microbenchmark
#endif // CACHE_MICROBENCHMARK

	// Process the pipelines
	if (this->register_file.empty())
	{ // Register File is empty
		// Get a new instruction from the instx port
		// Get a hash request from the instx port
		HashRequest *hash_req = (HashRequest *)this->get_next_instx();
		if (hash_req == NULL)
		{ // Instruction Queue is empty
			this->EC_no_instx++;
			return (this->return_routine(true, CORE_STALLED));
		}
		else
		{
// Instruction Queue is not empty
#ifdef TRACE_INSTX
			cout << "NeuraCore: " << this->id << " got a hash request id: " << hash_req->get_id() << endl;
#endif
			// cout << "NeuraCore: " << this->id << " got a hash request" << endl;
			// cout << "NeuraCore: " << this->id << " processing hash request" << endl;
			// Process the hash request
			// this->process_hash_request(hash_req);
			// cout << "NeuraCore: " << this->id << " processed hash request" << endl;
			// cout << "NeuraCore: " << this->id << " pushing hash request to register file" << endl;
			// Push the hash request to the register file
			this->register_file.push_back(hash_req);
			// cout << "NeuraCore: " << this->id << " pushed hash request to register file" << endl;
			// cout << "NeuraCore: " << this->id << " processing pipelines" << endl;
			// Process the pipelines
			// TODO: Do not process instructions in the same cycle as they are fetched
			// this->process_pipelines();
			// cout << "NeuraCore: " << this->id << " processed pipelines" << endl;
			this->EC_instx_fetched++;
			return (this->return_routine(false, CORE_COMPUTED));
		}
	}
	else
	{ // Register File is not empty
		bool work_done;
		work_done = this->process_pipelines();
		if (work_done)
		{ // Work is done
			this->EC_hashreq_processed++;
			return (this->return_routine(false, CORE_COMPUTED));
		}
		else
		{ // RF Not empty and Work is not done
			// Get a new instruction from the instx port
			if (this->is_register_file_available())
			{
				// Get a hash request from the instx port
				HashRequest *hash_req = (HashRequest *)this->get_next_instx();
				if (hash_req == NULL)
				{ // Instruction Queue is empty and RF is available
					bool rf_empty = this->register_file.empty();
					if (rf_empty)
					{ // RF is empty and Instruction Queue is empty
						this->EC_no_instx++;
						return (this->return_routine(true, CORE_STALLED));
					}
					else
					{ // RF is not empty and Instruction Queue is empty
						this->EC_rf_awaiting_mem_resp++;
						return (this->return_routine(false, CORE_STALLED));
					}
				}
				else
				{ // hash_req != NULL
					// Push to register file
					this->register_file.push_back(hash_req);
					// this->processing_counter += IF_PROCESSING_CYCLES; // TODO: Change this to ZERO
					this->EC_instx_fetched++;
					return (this->return_routine(false, CORE_COMPUTED));
				}
			}
			else
			{ // Register File is full and no instruction is being processed
				// Wait for the register file to be available
				this->EC_rf_full++;
				return (this->return_routine(false, CORE_STALLED));
			}
		}
	}
	cout << "NeuraCore::tick() - ERROR: Should not reach here" << endl;
}

bool NeuraCore::return_routine(bool sim_end, bool computed)
{
	if (computed)
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
	this->inc_reg_file_times();
	this->rf_responder();
	// this->print_register_file();
	return this->sim_end;
}

// Is register file available
// @return: True if register file is available
bool NeuraCore::is_register_file_available()
{
	if (this->register_file.size() >= this->gcp->PIPELINE_COUNT)
	{
		return false;
	}
	return true;
}

bool NeuraCore::process_pipelines()
{
	// Check if Pipelines contain any instx
	// If not, return false
	if (this->register_file.empty())
	{
		cout << FATAL_ERROR << "NeuraCore::process_pipelines() - ERROR: Register file is empty" << endl;
	}
	int current_pipeline = this->get_next_ready_pipeline();
	// If no pipeline is ready, return false
	if (current_pipeline == -1)
	{
		return false;
	}
	bool work_done;
	work_done = this->process_hash_request((HashRequest *)this->register_file[current_pipeline]);
	if (work_done)
	{
		return WORK_DONE;
	}
}

int NeuraCore::get_next_ready_pipeline()
{
	// Round robin scheduling of ready pipelines
	this->last_processed_pipeline++;
	int rf_size = this->register_file.size();
	for (int i = 0; i < rf_size; i++)
	{
		if (this->last_processed_pipeline >= this->gcp->PIPELINE_COUNT)
		{
			this->last_processed_pipeline = 0;
		}
		last_processed_pipeline = this->last_processed_pipeline % rf_size;
		if (((HashRequest *)this->register_file[this->last_processed_pipeline])->is_ready_to_process())
		{
			return this->last_processed_pipeline;
		}
		this->last_processed_pipeline++;
	}
	if (this->last_processed_pipeline >= this->gcp->PIPELINE_COUNT)
	{
		this->last_processed_pipeline = 0;
	}
	return -1;
}

// Process a hash request
// @param hash_req: Hash request
bool NeuraCore::process_hash_request(HashRequest *hash_req)
{
	if (hash_req->state == HR_NOT_STARTED)
	{
		// Send A Data Address load memory request to NeuraMem
		// cout << "Core: Sending A Data Address" << endl;
		MemRequest *mem_req = this->generate_mem_request(this->gcp,
														 this->uid,
														 this->get_nm_uid_from_address(hash_req->a_data_addr),
														 MEM_REQ_READ,
														 hash_req->a_data_addr,
														 -1,
														 4,
														 -1,
														 hash_req);
		mem_req->set_operand_id(HR_OPERAND_A_DATA_ADDR);
#ifndef BYPASS_NEURA_CORE_HBM_READS
		this->send_mem_request(mem_req);
#endif // BYPASS_NEURA_CORE_HBM_READS
		// Update the HashRequest state
		hash_req->state = HR_LOAD_A_DATA_ADDR;
		// hash_req->a_data_ready = false;
		// hash_req->b_data_ready = false;
		// hash_req->b_col_id_ready = false;
		// hash_req->all_ready = true;
		hash_req->ready_to_process = true;
		return WORK_DONE;
	}
	else if (hash_req->state == HR_LOAD_A_DATA_ADDR)
	{
		// Send B Data Address load memory request to NeuraMem
		// cout << "Core: Sending B Data Address" << endl;
		MemRequest *mem_req = this->generate_mem_request(this->gcp,
														 this->uid,
														 this->get_nm_uid_from_address(hash_req->b_data_addr),
														 MEM_REQ_READ,
														 hash_req->b_data_addr,
														 -1,
														 4,
														 -1,
														 hash_req);
		mem_req->set_operand_id(HR_OPERAND_B_DATA_ADDR);
#ifndef BYPASS_NEURA_CORE_HBM_READS
		this->send_mem_request(mem_req);
#endif // BYPASS_NEURA_CORE_HBM_READS
		// Update the HashRequest state
		hash_req->state = HR_LOAD_B_DATA_ADDR;
		// hash_req->all_ready = true;
		hash_req->ready_to_process = true;
		return WORK_DONE;
	}
	else if (hash_req->state == HR_LOAD_B_DATA_ADDR)
	{
		// Send B Col ID Address load memory request to NeuraMem
		// cout << "Core: Sending B Col ID Address" << endl;
		MemRequest *mem_req = this->generate_mem_request(this->gcp,
														 this->uid,
														 this->get_nm_uid_from_address(hash_req->b_col_id_addr),
														 MEM_REQ_READ,
														 hash_req->b_col_id_addr,
														 -1,
														 4,
														 -1,
														 hash_req);
		mem_req->set_operand_id(HR_OPERAND_B_COL_ID_ADDR);
#ifndef BYPASS_NEURA_CORE_HBM_READS
		this->send_mem_request(mem_req);
#endif // BYPASS_NEURA_CORE_HBM_READS

		// Update the HashRequest state
		hash_req->state = HR_LOAD_B_COL_ID_ADDR;
		// hash_req->all_ready = false;
		hash_req->ready_to_process = false;

#ifdef BYPASS_NEURA_CORE_HBM_READS
		hash_req->ready_to_process = true;
		hash_req->b_col_id = this->gcp->address_data_map_ptr->at(hash_req->b_col_id_addr);
		hash_req->set_a_data_ready();
		hash_req->set_b_data_ready();
		hash_req->set_b_col_id_ready();
#endif // BYPASS_NEURA_CORE_HBM_READS

		return WORK_DONE;
	}
	else if (hash_req->state == HR_LOAD_B_COL_ID_ADDR)
	{
		if (hash_req->is_all_ready())
		{
			// Multiply the A Data and B Data
			// cout << "Core: Multiplying A Data and B Data" << endl;
			hash_req->mult_result = hash_req->a_data * hash_req->b_data;
			hash_req->state = HR_MULTIPLY;
			this->processing_counter += this->gcp->MULT_PROCESSING_CYCLES;
			return WORK_DONE;
		}
		else
		{
			// cout << "Core: Not all operands are ready" << endl;
			return WORK_NOT_DONE;
		}
	}
	else if (hash_req->state == HR_MULTIPLY)
	{
		// Check if count is 1
		if (hash_req->get_count() <= 1)
		{
			// Generate HashResponse in the core itself
			// Hashing is not required in NeuraMem
			// Send to writeto the writer directly
			// cout << "Core: Generating HashResponse" << endl;
			HashResponse *hash_resp = this->generate_hash_response(hash_req);
			// Send the HashResponse to the writer
			this->send_hash_response(hash_resp);
			// Remove the HashRequest from the register file
			this->remove_hash_request(hash_req);
			return WORK_DONE;
		}
		else
		{
			// Map hash request to a NeuraMem
			this->map_hash_request_to_neuramem(hash_req);
			// Send the HashRequest to the NeuraMem
			this->send_hash_request(hash_req);
			// Update Time spent in Core Bins
			this->insert_rf_histogram(hash_req->reg_file_time);
			// Remove the HashRequest from the register file
			this->remove_hash_request(hash_req);
			return WORK_DONE;
		}
	}
	else
	{
		cout << FATAL_ERROR << "Core: Invalid HashRequest state" << endl;
		return WORK_DONE;
	}
}

// Remove HashRequest from the register file
// @param hash_req: HashRequest to remove
void NeuraCore::remove_hash_request(HashRequest *hash_req)
{
	// Remove the HashRequest from the register file
	for (size_t i = 0; i < this->register_file.size(); i++)
	{
		if (this->register_file[i] == hash_req)
		{
			this->register_file.erase(this->register_file.begin() + i);
			return;
		}
	}
}

// Map HashRequest to a NeuraMem
// @param hash_req: HashRequest to map
void NeuraCore::map_hash_request_to_neuramem(HashRequest *hash_req)
{
	// Map the HashRequest to a NeuraMem
	// TODO: Implement a better mapping algorithm
	// Use lower bits for hashing (Temporarily)
	// Divide by 4 because the address is 4 bytes addressing
	size_t nm_id = this->address_hashing(hash_req->get_b_col_id(), this->gcp->get_total_mems());
	// cout << "Core: Mapping HashRequest to NeuraMem " << hash_req->get_b_col_id() << "-->" << nm_id << endl;
	hash_req->set_dest_unit_uid((this->gcp->get_mem_id_array())[nm_id]);
}

int NeuraCore::get_nm_uid_from_b_col_id(int b_col_id)
{
	// TODO: Implement a better mapping algorithm
	size_t nm_id = this->address_hashing(b_col_id, this->gcp->get_total_mems());
	int nm_uid = this->gcp->get_mem_id_array()[nm_id];
	return nm_uid;
}

int NeuraCore::get_nm_uid_from_address(long long address)
{
	// TODO: Implement a better mapping algorithm
	// size_t nm_id = this->address_hashing(address, this->gcp->get_total_mems());
	size_t nm_id = (address / 4) % this->gcp->get_total_mems();
	int nm_uid = this->gcp->get_mem_id_array()[nm_id];
	return nm_uid;
}

size_t NeuraCore::address_hashing(int b_col_id, size_t total_units)
{
	// TODO: Check for correctness
	// int unit_id = (address / 4) % total_units;
	size_t unit_id = b_col_id % total_units;
	return unit_id;
}

// Send HashRequest to NeuraMem
// @param hash_req: HashRequest to send
// BUG: Fix this
void NeuraCore::send_hash_request(HashRequest *hash_req)
{
// Send the HashRequest to the NeuraMem
// cout << "Core: Sending HashRequest to NeuraMem" << endl;
// int dest_port_id = this->get_port_from_uid(hash_req->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << CYAN_START << "Core: Sending HashRequest to NeuraMem" << COLOR_RESET << endl;
#endif
	int dest_port_id = this->get_dest_port(hash_req->get_dest_unit_uid());
	// cout << CYAN_START << "Core: Sending HashRequest to port " << dest_port_id << COLOR_RESET << endl;
	this->out_port_array[dest_port_id].push(hash_req);
}

// Send HashResponse to Writer
// @param hash_resp: HashResponse to send
// BUG: Fix this
void NeuraCore::send_hash_response(HashResponse *hash_resp)
{
// Send the HashResponse to the Writer
// cout << "Core: Sending HashResponse to Writer" << endl;
// int dest_port_id = this->get_port_from_uid(hash_resp->get_dest_unit_uid());
#ifdef TRACE_INSTX
	cout << CYAN_START << "Core: Sending HashResponse to Writer" << COLOR_RESET << endl;
#endif
	int dest_port_id = this->get_dest_port(hash_resp->get_dest_unit_uid());
	// cout << CYAN_START << "Core: Sending HashResponse to port " << dest_port_id << COLOR_RESET << endl;
	this->out_port_array[dest_port_id].push(hash_resp);
}

// BUG: This function has to be deleted
// int NeuraCore::get_port_from_uid(int uid) {
// 	const int core_uid = this->gcp->get_core_id_array()[0];
// 	const int mem_uid = this->gcp->get_mem_id_array()[0];
// 	const int router_uid = this->gcp->get_router_id_array()[0];
// 	const int dispatcher_uid = this->gcp->get_dispatcher_id_array()[0];
// 	const int writer_uid = this->gcp->get_writer_id_array()[0];
// 	const int mc_uid = this->gcp->get_mc_id_array()[0];
// 	if (uid == core_uid) {
// 		cout << FATAL_ERROR << "Core sending data to core" << endl;
// 		return -1;
// 	}
// 	else if (uid == mem_uid) {
// 		return 0;
// 	}
// 	else if (uid == router_uid) {
// 		// cout << "Sending to Router" << endl;
// 		cout << "Pending Implementation" << endl;
// 		return -1;
// 	}
// 	else if (uid == dispatcher_uid) {
// 		return 4;
// 	}
// 	else if (uid == writer_uid) {
// 		return 2;
// 	}
// 	else if (uid == mc_uid) {
// 		cout << "Core not connected to Memory Controller" << endl;
// 		return -1;
// 	}
// 	else {
// 		cout << FATAL_ERROR << "Invalid UID: " << uid << endl;
// 		e_print(0);
// 		return -1;
// 	}
// }

void NeuraCore::rf_responder()
{
	// Check for any response from NeuraMem on any in ports
	for (int i = 0; i < this->gcp->NEURA_CORE_DATA_PORT_COUNT; i++)
	{
		if (this->in_port_array[i].empty())
		{
			continue;
		}
		// cout << "RF Responder: Received response from NeuraMem" << endl;
		// Get the response
		if (this->in_port_array[i].front()->get_type() != INSTX_TYPE_MEM_RESP)
		{
			cout << FATAL_ERROR << "Invalid InstX type received. Should be MemResponse" << endl;
			cout << "Received: " << this->in_port_array[i].front()->get_type() << " on in port " << i << " of NeuraCore " << this->vertex_name << endl;
			e_print(0);
			exit(1);
		}
		MemResponse *mem_resp = (MemResponse *)this->in_port_array[i].front();
		this->in_port_array[i].pop();

#ifdef TRACE_INSTX
		cout << "RF Responder: Received Mem Response ID: " << mem_resp->get_id() << endl;
#endif // TRACE_INSTX

#ifdef CACHE_MICROBENCHMARK
		this->test_cache_completed_instx++;

		// Dont process the response while microbenchmarking the cache.
		// Since you dont have assocuated hash request, you cannot process the response.
		return;
#endif // CACHE_MICROBENCHMARK

		// mem_resp->print();
		// Get the hash request
		HashRequest *hash_req = mem_resp->src_hash_req;
		if (hash_req == NULL)
		{
			cout << FATAL_ERROR << "NeuraCore: " << this->vertex_name << endl;
			cout << "HashRequest is NULL" << endl;
			mem_resp->print();
			exit(1);
		}
		// cout << "HashReq Pointer: " << hash_req << endl;
		int operand_id = mem_resp->get_operand_id();
		if (operand_id == HR_OPERAND_A_DATA_ADDR)
		{
			// cout << RED_START << "RF Responder: Received A Data Address | Payload: " << mem_resp->payload << COLOR_RESET << endl;
			hash_req->a_data = mem_resp->payload;
			hash_req->set_a_data_ready();
			delete mem_resp;
			// Only set one response per cycle
			// this->processing_counter += 1; // Does reading response from ports consume cycles?
			return;
		}
		else if (operand_id == HR_OPERAND_B_DATA_ADDR)
		{
			// cout << GREEN_START << "RF Responder: Received B Data Address | Payload: " << mem_resp->payload << COLOR_RESET << endl;
			hash_req->b_data = mem_resp->payload;
			hash_req->set_b_data_ready();
			delete mem_resp;
			// Only set one response per cycle
			// this->processing_counter += 1; // Does reading response from ports consume cycles?
			return;
		}
		else if (operand_id == HR_OPERAND_B_COL_ID_ADDR)
		{
			// cout << CYAN_START << "RF Responder: Received B Col ID Address | Payload: " << mem_resp->payload << COLOR_RESET << endl;
			hash_req->b_col_id = mem_resp->payload;
			hash_req->set_b_col_id_ready();
			hash_req->ready_to_process = true;
			delete mem_resp;
			// Only set one response per cycle
			// this->processing_counter += 1; // Does reading response from ports consume cycles?
			return;
		}
		else
		{
			cout << "ERROR: Unknown operand_id: " << operand_id << endl;
			cout << "NeuraCore: " << this->vertex_name << endl;
			cout << "HashRequest" << endl;
			hash_req->print();
			cout << "MemResponse" << endl;
			mem_resp->print();
			exit(1);
		}
	}
}

void NeuraCore::inc_reg_file_times()
{
	for (auto it : this->register_file)
	{
		HashRequest *hash_req = (HashRequest *)it;
		hash_req->inc_reg_file_time();
	}
}

void NeuraCore::print_register_file()
{
	cout << "************* " << YELLOW_START << "Register File" << COLOR_RESET << " *************" << endl;
	cout << "ID\tA Data\tB Data\tB Col ID\tMult Result\tState" << endl;
	set_TF_printer();
	for (auto it : this->register_file)
	{
		HashRequest *hash_req = (HashRequest *)it;
		cout << hash_req->id << "\t";
		cout << hash_req->a_data_ready << "\t";
		cout << hash_req->b_data_ready << "\t";
		cout << hash_req->b_col_id_ready << "\t\t";
		cout << hash_req->mult_result << "\t\t";
		cout << hash_req->state_in_text() << endl;
	}
	reset_TF_printer();
}

// Get the next instx
// @return: Next instx
InstX *NeuraCore::get_next_instx()
{
	// Get the next instx
	// If there is no instx, return NULL
	if (this->in_port_array[this->gcp->INSTX_PORT_INDEX].empty())
	{
		return NULL;
	}
	InstX *instx = this->in_port_array[this->gcp->INSTX_PORT_INDEX].front();
	this->in_port_array[this->gcp->INSTX_PORT_INDEX].pop();
	return instx;
}

// Print Statistics
void NeuraCore::statistics()
{
	cout << "************* " << YELLOW_START << "NeuraCore" << COLOR_RESET << " *************" << endl;
	cout << "Core[" << this->x_cood;
	cout << "][" << this->y_cood << "] ";
	cout << "ID: " << this->id;
	cout << " UID: " << this->uid << endl;
	cout << "Neighbors: " << this->neighbors_uid.size() << endl;
	cout << "Neighbors: ";
	for (auto it : this->neighbors_uid)
	{
		cout << this->gcp->uid_to_vertex_name_map[it] << " ";
	}
	cout << endl;
	cout << RED_START << "Stalls" COLOR_RESET << ": " << this->idle_cycles << "\t";
	cout << GREEN_START << "Compute" << COLOR_RESET << ": " << this->busy_cycles << endl;
	cout << "Port Pressure: ";
	for (int i = 0; i < this->port_count; i++)
	{
		if (i == this->gcp->INSTX_PORT_INDEX)
		{
			cout << "InstX [";
			cout << RED_START << this->in_port_array[i].size() << COLOR_RESET;
			cout << "/";
			cout << CYAN_START << this->out_port_array[i].size() << COLOR_RESET;
			cout << "]  ";
		}
		else
		{
			cout << "[";
			cout << CYAN_START << this->in_port_array[i].size() << COLOR_RESET;
			cout << "/";
			cout << CYAN_START << this->out_port_array[i].size() << COLOR_RESET;
			cout << "]  ";
		}
	}
	cout << endl;
	cout << "*************************************" << endl;
}

void NeuraCore::smart_stats()
{
	// TODO: Pending implementation
}

// Send a memory request to a port
// @param mem_req: Memory request
// @param port_id: Port ID
void NeuraCore::send_mem_request(MemRequest *mem_req)
{
// Add it to the map
// this->mem_req_map[mem_req->id] = mem_req;
// cout << "NeuraCore: Sending Mem Request addr: " << mem_req->get_src_address() << endl;
// Print neighbors_uid
// cout << "Core Neighbors UID: ";
// for (auto it : this->neighbors_uid) {
// 	cout << it << " ";
// }
// Print size of neighbors_uid vector
// cout << "Neighbors UID Size: " << this->neighbors_uid.size() << endl;
#ifdef TRACE_INSTX
	cout << "NeuraCore: Sending Mem Request addr: " << mem_req->get_src_address() << endl;
#endif
	int dest_port_id = this->get_dest_port(mem_req->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(mem_req);
}

// Receive a memory request from a port
// @param port_id: Port ID
// @return: Memory request
InstX *NeuraCore::receive_instx(int port_id)
{
	InstX *instx = this->in_port_array[port_id].front();
	this->in_port_array[port_id].pop();
	return instx;
}

MemRequest *NeuraCore::generate_mem_request(GlobalCounters *gcp,
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

// Generate Hash Request
HashRequest *NeuraCore::generate_hash_request(int dest_unit_uid,
											  long long a_data_addr,
											  long long b_data_addr,
											  long long b_col_id_addr,
											  long long count,
											  long long table_id)
{
	HashRequest *hash_req = new HashRequest(this->gcp,
											this->uid,
											dest_unit_uid,
											a_data_addr,
											b_data_addr,
											b_col_id_addr,
											count,
											table_id);
	return (hash_req);
}

// Cores dont need to generate memory responses
// MemRequest *NeuraCore::generate_mem_response(	GlobalCounters *gcp,
// 												int src_unit_uid,
// 												MemRequest *src_mem_req) {
// 	MemRequest *mem_req = new MemRequest(	gcp,
// 											src_unit_uid,
// 											src_mem_req->get_id(),
// 											MEM_REQ_RESP,
// 											-1,
// 											-1,
// 											-1,
// 											-1,
// 											src_mem_req->get_src_unit_uid());
// 	mem_req->type = INSTX_TYPE_MEM_RESP;
// 	return (mem_req);
// }

bool NeuraCore::sanity_check()
{
	// Check if all ports are empty
	for (int i = 0; i < this->port_count; i++)
	{
		if (this->in_port_array[i].size() != 0)
		{
			cout << "NeuraCore[" << this->x_cood << "][" << this->y_cood << "] ";
			cout << "Port[" << i << "] is not empty" << endl;
			return false;
		}
		if (this->out_port_array[i].size() != 0)
		{
			cout << "NeuraCore[" << this->x_cood << "][" << this->y_cood << "] ";
			cout << "Port[" << i << "] is not empty" << endl;
			return false;
		}
	}
	// Make sure arch_graph_distances vector is not empty
	if (this->arch_graph_distances.size() == 0)
	{
		cout << "NeuraCore[" << this->x_cood << "][" << this->y_cood << "] ";
		cout << "arch_graph_distances vector is empty" << endl;
		return false;
	}
	return true;
}

bool NeuraCore::get_sim_end()
{
	return this->sim_end;
}

void NeuraCore::post_sim_metrics()
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
			kvp("no_instx", (long)this->EC_no_instx),
			kvp("instx_fetched", (long)this->EC_instx_fetched),
			kvp("hashreq_processed", (long)this->EC_hashreq_processed),
			kvp("rf_full", (long)this->EC_rf_full),
			kvp("rf_awaiting_mem_resp", (long)this->EC_rf_awaiting_mem_resp)));

	}

#ifdef CACHE_MICROBENCHMARK
	if (this->x_cood == 0 && this->y_cood == 0)
	{
		cout << "Test Cache Completed InstX: " << this->test_cache_completed_instx << endl;
		double total_bytes = (double)(this->test_cache_completed_instx * 4);
		double total_gb = total_bytes / (1024 * 1024 * 1024);
		double total_cycles = (double)this->gcp->get_cycle();
		double total_seconds = (double)total_cycles / (double)this->gcp->get_clock_frequency();
		double total_gb_per_sec = (double)total_gb / total_seconds;
		cout << "Total GB: " << total_gb << endl;
		cout << "Total Seconds: " << total_seconds << endl;
		cout << "Total GB/s: " << total_gb_per_sec << endl;
	}
#endif // CACHE_MICROBENCHMARK

	return;
}

void NeuraCore::interval_metrics()
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

HashResponse *NeuraCore::generate_hash_response(HashRequest *hash_req)
{
	HashResponse *hash_resp = new HashResponse(this->gcp,													 // GlobalCounters *gcp,
											   this->uid,													 // int src_unit_uid,
											   this->get_writer_uid_from_table_id(hash_req->get_table_id()), // int dest_unit_uid,
											   hash_req->get_table_id(),									 // long long table_id,
											   hash_req->get_b_col_id(),									 // long long b_col_id
											   hash_req->get_mult_result(),									 // int mult_result,
											   hash_req														 // HashRequest *src_hash_req,
	);
	return (hash_resp);
}

int NeuraCore::get_writer_uid_from_table_id(long long table_id)
{
	// TODO: Pending implementation
	return (this->gcp->get_writer_id_array()[0]);
}

// @param dest_uid: Destination Unit UID
// @return: Port ID to send data on
// @description: Finds the best port to send data using the best routing algorithm
int NeuraCore::get_dest_port(int dest_uid)
{

	int min_distance_port = -1;

	min_distance_port = packet_routing(dest_uid, this->gcp, &this->neighbors_uid, &this->uid_to_port_map);
	return (min_distance_port);

	/*
		// Get the destination unit's vertex descriptor
		cout << "Dest UID: " << dest_uid << endl;
		auto dest_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[dest_uid];

		cout << "Dest Unit Vertex Desc: " << dest_unit_vertex_desc << endl;

		// this->neighbors_uid is a vector of all the neighbors of this unit

		cout << "Neighbors UID: ";
		for (auto neighbor_uid : this->neighbors_uid) {
			cout << neighbor_uid << " ";
		}
		cout << endl;

		vector<int> neighbor_distances_to_dest;
		// Iterate over all the neighbors uids of this unit
		for (auto neighbor_uid : this->neighbors_uid) {
			// Get the arch_graph_distances vector of the neighbor
			auto neighbor_distances_ptr = this->gcp->uid_to_arch_graph_distances_ptr_map[neighbor_uid];
			auto neighbor_distance_to_dest = (*neighbor_distances_ptr)[dest_unit_vertex_desc];
			neighbor_distances_to_dest.push_back(neighbor_distance_to_dest);
		}

		cout << "Neighbor Distances to Dest: ";
		for (auto neighbor_distance_to_dest : neighbor_distances_to_dest) {
			cout << neighbor_distance_to_dest << " ";
		}
		cout << endl;

		// This is X-Y routing
		// Find the port with the minimum distance to the destination
		// TODO: Use neighbor_distances_to_dest to do adaptive routing
		int min_distance = INT_MAX;
		int min_distance_port = -1;
		for (int i=0; i<neighbor_distances_to_dest.size(); i++) {
			if (neighbor_distances_to_dest[i] < min_distance) {
				min_distance = neighbor_distances_to_dest[i];
				min_distance_port = this->uid_to_port_map[this->neighbors_uid[i]];
			}
		}

		cout << "Min Distance: " << min_distance << endl;
		cout << "Min Distance Port: " << min_distance_port << endl;


		return (min_distance_port);
	*/
}

void NeuraCore::create_arch_graph_distances()
{
	// cout << "Before arch_graph_distances size: " << this->arch_graph_distances.size() << endl;
	arch_distancing(this->gcp,
					this->arch_graph_distances,
					this->vertex_name,
					this->uid,
					this->neighbors_desc,
					this->neighbors_uid);
	// cout << "After arch_graph_distances size: " << this->arch_graph_distances.size() << endl;
	// int total_vertices = boost::num_vertices(this->gcp->arch_graph);
	// this->arch_graph_distances.resize(total_vertices);
	// // cout << "Vertex name: " << this->vertex_name << endl;
	// boost::dijkstra_shortest_paths(	this->gcp->arch_graph,
	// 								this->gcp->vertex_name_to_desc_map[this->vertex_name],
	// 								boost::distance_map(&this->arch_graph_distances[0]));

	// // Print arch_graph_distances
	// cout << "Total Vertices: " << total_vertices << endl;
	// cout << "Arch Graph Distances: ";
	// for (auto distance : this->arch_graph_distances) {
	// 	cout << distance << " ";
	// }
	// cout << endl;

	// this->gcp->uid_to_arch_graph_distances_ptr_map[this->uid] = &this->arch_graph_distances;

	// // Source vertex descriptor
	// auto src_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[this->uid];

	// // Find the neighbors of this vertex
	// auto neighbors_iter_pair = boost::adjacent_vertices(src_unit_vertex_desc, this->gcp->arch_graph);
	// for (auto iter = neighbors_iter_pair.first; iter != neighbors_iter_pair.second; iter++) {
	// 	// this->neighbors_desc.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// 	this->neighbors_desc.push_back(*iter);
	// }

	// // Create neighbor_uid vector
	// cout << "Creating Core: " << this->vertex_name << " Neighbors: ";
	// for (auto iter = this->neighbors_desc.begin(); iter != this->neighbors_desc.end(); iter++) {
	// 	this->neighbors_uid.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// 	cout << this->gcp->vertex_desc_to_uid_map[*iter] << " ";
	// }
	// cout << endl;

	// if (this->id == 0) {
	// 	// Print vertex_desc_to_uid_map
	// 	cout << "Vertex Desc to UID Map: ";
	// 	for (auto iter = this->gcp->vertex_desc_to_uid_map.begin(); iter != this->gcp->vertex_desc_to_uid_map.end(); iter++) {
	// 		cout << iter->first << ": " << iter->second << endl;
	// 	}
	// }
}

string NeuraCore::get_vertex_name()
{
	return (this->vertex_name);
}

void NeuraCore::insert_rf_histogram(int count)
{
	int index = count / RF_HISTOGRAM_BIN_SIZE;
	if (index >= this->gcp->rf_histogram.size())
	{
		this->gcp->rf_histogram.resize(index + 1);
	}
	this->gcp->rf_histogram[index]++;


}

