// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "memorycontroller.h"
#include "../debugger/debugger.h"
#include <unistd.h>
#include <sys/stat.h>
#include "../functions.h"

using namespace std;
using namespace dramsim3;
using namespace placeholders;

// Default constructor needed as NeuraTile initializes an array of MemoryController objects
// followed by a call to custom_constructor
MemoryController::MemoryController()
{
}

MemoryController::MemoryController(GlobalCounters *gcp, int x_cood)
{
	this->custom_constructor(gcp, x_cood);
}

void MemoryController::custom_constructor(GlobalCounters *gcp, int x_cood)
{
	this->gcp = gcp;
	this->x_cood = x_cood;
	this->id = this->gcp->generate_mc_id();
	this->uid = this->gcp->generate_uid();
	this->gcp->set_mc_id_array(this->id, this->uid);
	this->port_count = this->gcp->MC_PORT_COUNT;
	this->in_port_array = new queue<InstX *>[this->port_count];
	this->out_port_array = new queue<InstX *>[this->port_count];
	this->sim_end = false;
	this->processing_counter = 0;
	this->idle_cycles = 0;
	this->busy_cycles = 0;
	this->rejected_requests = 0;
	this->interval_idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_id = 0;
	this->interval_rejected_requests = 0;
	this->last_processed_port = 0;
	this->interval_avg_in_flight_requests = 0;
	this->mem_addresses_merged = 0;
	this->dram_data_file_name_0 = this->gcp->get_dram_data_file_name_0();
	this->dram_data_file_name_1 = this->gcp->get_dram_data_file_name_1();
	this->component_name = "MemoryController_" + to_string(this->id);
	this->dram_name = "DRAM_" + to_string(this->id);
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->dram_collection = this->gcp->mongo->get_component_collection(this->dram_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
	// const string config_file = "configs/DDR4_8Gb_x8_3200.ini";
	const string config_file = "/home/ktb/git/NeuraChip/NeuraSim/ktb_dram_configs/ktb_HBM2_8Gb_x128.ini";
	this->dram_output_dir_0 = this->gcp->get_results_dir() + "/" + "dram_results_0";
	this->dram_output_dir_1 = this->gcp->get_results_dir() + "/" + "dram_results_1";
	// const string output_dir = "ktb_1";
	// Make sure the output directory exists
	mkdir(this->dram_output_dir_0.c_str(), 0777);
	mkdir(this->dram_output_dir_1.c_str(), 0777);
	// cout << "Output Directory: " << this->dram_output_dir << endl;
	// this->hbm = new MemorySystem(	config_file,
	// 								output_dir,
	// 								bind(&MemoryController::ReadCallBack, this, _1),
	// 								bind(&MemoryController::WriteCallBack, this, _1)
	// 		  );
	this->hbm_0 = GetMemorySystem(config_file,
								  this->dram_output_dir_0,
								  bind(&MemoryController::ReadCallBack_0, this, _1),
								  bind(&MemoryController::WriteCallBack_0, this, _1));
	this->hbm_1 = GetMemorySystem(config_file,
								  this->dram_output_dir_1,
								  bind(&MemoryController::ReadCallBack_1, this, _1),
								  bind(&MemoryController::WriteCallBack_1, this, _1));
	this->hbm_array = new MemorySystem *[2];
	this->hbm_array[0] = this->hbm_0;
	this->hbm_array[1] = this->hbm_1;
	this->load_dram_data();

	this->vertex_name = "X_" + to_string(this->x_cood);
	this->gcp->add_vertex_to_arch_graph(this->vertex_name, this->uid);

	this->accessed_address_array = new long long[MC_MAX_ACCESSED_COUNT];
	this->accessed_at_cycle_array = new long long[MC_MAX_ACCESSED_COUNT];
	this->accessed_is_write_array = new bool[MC_MAX_ACCESSED_COUNT];
	this->rejected_accessed_address_array = new long long[MC_MAX_ACCESSED_COUNT];
	this->rejected_accessed_at_cycle_array = new long long[MC_MAX_ACCESSED_COUNT];
	this->rejected_accessed_port_number_array = new bool[MC_MAX_ACCESSED_COUNT];
	this->accessed_size = 0;
	this->rejected_accessed_size = 0;
	this->hbm_0_reqs = 0;
	this->hbm_1_reqs = 0;
	this->hbm_0_reads = 0;
	this->hbm_1_reads = 0;
	this->hbm_0_writes = 0;
	this->hbm_1_writes = 0;

	this->gcp->address_data_map_ptr = &this->address_data_map;

	this->port_stalls = new long long[this->port_count];

	for (int i = 0; i < this->port_count; i++)
	{
		this->port_stalls[i] = 0;
	}

	// this->test_address = 0;
	// this->test_count = 0;
	// this->test_complete_instx = 0;
	// this->test_total_instx = 100000;
	// this->test_rejected_count = 0;
}

void MemoryController::ReadCallBack_0(uint64_t addr)
{
#ifdef TRACE_INSTX
	cout << "ReadCallBack: " << addr << endl;
#endif
	// this->test_complete_instx++;
	// return;

	// std::cout << "ReadCallBack: " << addr << std::endl;
	vector<MemRequest *> mem_req_vec = this->address_mem_req_map[addr];
	// Send response to each of the memory requests
	for (auto mem_req : mem_req_vec)
	{
		// Send response to the memory request
		this->send_memory_response(mem_req);
	}
	// Delete the memory requests
	for (auto mem_req : mem_req_vec)
	{
		delete mem_req;
	}
	// Remove the memory request from the map
	this->address_mem_req_map.erase(addr);
	this->hbm_0_reads++;
	return;
}

void MemoryController::WriteCallBack_0(uint64_t addr)
{
#ifdef TRACE_INSTX
	cout << "WriteCallBack: " << addr << endl;
#endif
	// this->test_complete_instx++;
	// return;

	// std::cout << "WriteCallBack: " << addr << std::endl;
	vector<MemRequest *> mem_req_vec = this->address_mem_req_map[addr];
	// Write to address_data_map
	this->address_data_map[addr] = mem_req_vec[0]->get_payload();
	// Delete the memory requests
	for (auto mem_req : mem_req_vec)
	{
		delete mem_req;
	}
	// Remove the memory request from the map
	this->address_mem_req_map.erase(addr);
	this->hbm_0_writes++;
	return;
}

void MemoryController::ReadCallBack_1(uint64_t addr)
{
#ifdef TRACE_INSTX
	cout << "ReadCallBack: " << addr << endl;
#endif
	// this->test_complete_instx++;
	// return;

	// std::cout << "ReadCallBack: " << addr << std::endl;
	vector<MemRequest *> mem_req_vec = this->address_mem_req_map[addr];
	// Send response to each of the memory requests
	for (auto mem_req : mem_req_vec)
	{
		// Send response to the memory request
		this->send_memory_response(mem_req);
	}
	// Delete the memory requests
	for (auto mem_req : mem_req_vec)
	{
		delete mem_req;
	}
	// Remove the memory request from the map
	this->address_mem_req_map.erase(addr);
	this->hbm_1_reads++;
	return;
}

void MemoryController::WriteCallBack_1(uint64_t addr)
{
#ifdef TRACE_INSTX
	cout << "WriteCallBack: " << addr << endl;
#endif
	// this->test_complete_instx++;
	// return;

	// std::cout << "WriteCallBack: " << addr << std::endl;
	vector<MemRequest *> mem_req_vec = this->address_mem_req_map[addr];
	// Write to address_data_map
	this->address_data_map[addr] = mem_req_vec[0]->get_payload();
	// Delete the memory requests
	for (auto mem_req : mem_req_vec)
	{
		delete mem_req;
	}
	// Remove the memory request from the map
	this->address_mem_req_map.erase(addr);
	this->hbm_1_writes++;
	return;
}

MemoryController::~MemoryController()
{
	delete[] this->in_port_array;
	delete[] this->out_port_array;
}

int MemoryController::get_id()
{
	return this->id;
}

int MemoryController::get_uid()
{
	return this->uid;
}

queue<InstX *> *MemoryController::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

queue<InstX *> *MemoryController::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

bool MemoryController::tick()
{
	// Tick HBM
	// FIXME: Remove HBM tick from one memory controller.
	// Tick HBM separately in run_sim function
	// Because if you have multiple MCs, each one of them will tick the HBM
	// Also tick HBM multiple times in one cycle in run_sim for asynchronous HBM with higher frequency
	// for (int i = 0; i < 3; i++)
	// {
	// 	this->hbm->ClockTick();
	// }
	// // Tick the HBM one more time every 5 cycles
	// if (this->gcp->get_cycle() % 5 == 0) {
	// 	this->hbm->ClockTick();
	// }

	if (this->gcp->get_cycle() % 4 == 0)
	{
		this->hbm_0->ClockTick();
		this->hbm_1->ClockTick();
	}
	if (this->processing_counter > 0)
	{
		this->processing_counter--;
		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	}

	// if (this->test_complete_instx >= (this->test_total_instx - 1))
	// {
	// 	cout << "Test Complete" << endl;
	// 	cout << "Dispatched: " << this->test_count << endl;
	// 	cout << "Completed: " << this->test_complete_instx << endl;
	// 	cout << "Rejected: " << this->test_rejected_count << endl;

	// 	double tx_bytes = (double)(this->test_complete_instx * 64);
	// 	double tx_gbytes = tx_bytes / (1024 * 1024 * 1024);
	// 	double total_cycles = (double)this->gcp->get_cycle();

	// 	cout << "HBM Frequency: " << this->gcp->get_hbm_frequency() << endl;
	// 	double total_seconds = total_cycles / (double)this->gcp->get_frequency();
	// 	// Bandwidth in GB/s
	// 	double bw_gbps = (tx_gbytes) / total_seconds;
	// 	cout << "Stride: " << this->gcp->test_stride << endl;
	// 	cout << "Total GB: " << tx_gbytes << endl;
	// 	cout << "Total Seconds: " << total_seconds << endl;
	// 	cout << "Bandwidth: " << bw_gbps << " GB/s" << endl;
	// 	return (this->return_routine(SIM_END_TRUE, STALLED));
	// }

	// if (this->test_count % 10001 == 0)
	// {
	// 	cout << "Dispatched: " << this->test_count;
	// 	cout << " Completed: " << this->test_complete_instx;
	// 	cout << " Rejected: " << this->test_rejected_count;
	// 	cout << " Total: " << this->test_total_instx << endl;
	// }

	// // DRAM BW Test
	// if (this->test_count >= this->test_total_instx)
	// {
	// 	// cout << "Test MC dispatching complete" << endl;
	// 	return (this->return_routine(SIM_END_FALSE, STALLED));
	// }

	// /*
	// 	// if (this->test_complete_instx >= (this->test_total_instx - 1)) {
	// 	// 	cout << "FATAL ERROR!!!!!!!!!!!!!!" << endl;
	// 	// 	return (this->return_routine(SIM_END_FALSE, STALLED));
	// 	// }
	// */
	// // int TX_PER_CYCLE = (int)this->gcp->test_stride;
	// int TX_PER_CYCLE = 128;
	// for (int i = 0; i < TX_PER_CYCLE; i++)
	// {
	// 	// if (i == 3 || i == 6) {
	// 	// 	this->hbm->ClockTick();
	// 	// }
	// 	if (this->hbm->WillAcceptTransaction(this->test_address, false))
	// 	{
	// 		// cout << "Dispatching: " << this->test_address << endl;
	// 		this->hbm->AddTransaction(this->test_address, false);
	// 		long long STRIDE = (long long)this->gcp->test_stride;
	// 		this->test_address += (64 * (STRIDE));
	// 		// Generate a random address
	// 		// this->test_address = (((rand() % (1024 * 1024 * 1024)) >> 7) << 7);
	// 		this->test_count++;
	// 	}
	// 	else
	// 	{
	// 		this->test_rejected_count++;
	// 	}
	// 	// Tick 3 times within TX_PER_CYCLE
	// 	if (i % (TX_PER_CYCLE / 3) == 0)
	// 	{
	// 		this->hbm->ClockTick();
	// 	}
	// }
	// return (this->return_routine(SIM_END_FALSE, COMPUTED));

	// Tick all the ports
	bool port_ticked = false;
	bool tick_output;
	const int PORT_TICKS_PER_CYCLE = 8;
	const int HBM_TICK_INT = 3;
	for (int p_tick = 0; p_tick < PORT_TICKS_PER_CYCLE; p_tick++)
	{
		for (int i = 0; i < this->port_count; i++)
		{
			tick_output = this->tick_port(i);
			port_ticked = port_ticked || tick_output;
			if (tick_output == MC_PORT_EMPTY)
			{
				this->port_stalls[i]++;
			}
		}
		// Tick HBM 3 times within PORT_TICKS_PER_CYCLE
		if (p_tick % HBM_TICK_INT == 0)
		{
			this->hbm_0->ClockTick();
			this->hbm_1->ClockTick();
		}
	}
	if (port_ticked)
	{
		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	}
	else
	{
		return (this->return_routine(SIM_END_TRUE, STALLED));
	}

	// // Get next memory request
	// MemRequest *mem_req = this->get_next_memory_request();
	// if (mem_req == NULL)
	// {
	// 	// Check if address_mem_req_map is empty
	// 	if (this->address_mem_req_map.empty())
	// 	{
	// 		return (this->return_routine(SIM_END_TRUE, STALLED));
	// 	}
	// 	else
	// 	{
	// 		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	// 	}
	// }
	// else
	// {
	// 	this->update_mem_req_map_and_add_transaction(mem_req);
	// }

	cout << FATAL_ERROR << "MemoryController::tick() - Should not reach here" << endl;
	return (this->return_routine(SIM_END_TRUE, STALLED));
}

void MemoryController::update_mem_req_map_and_add_transaction(MemRequest *mem_req)
{
	// Process the memory request
	// Check if address is present in address_mem_req_map
	long long aligned_address = (mem_req->get_address() >> ALIGN_BIT_SHIFT) << ALIGN_BIT_SHIFT;
	// cout << "MC Address: " << mem_req->get_address() << " is_write: " << mem_req->get_is_write() << endl;
	// cout << "MC Aligned Address: " << aligned_address << endl;
	this->push_to_mc_accessed(aligned_address, this->gcp->get_cycle(), mem_req->get_is_write());
	if (this->address_mem_req_map.find(aligned_address) == this->address_mem_req_map.end())
	{
		// Create a new entry in address_mem_req_map
		vector<MemRequest *> *mem_req_vec;
		mem_req_vec = new vector<MemRequest *>();
		mem_req_vec->push_back(mem_req);
		this->address_mem_req_map.insert(pair<uint64_t, vector<MemRequest *>>(aligned_address, *mem_req_vec));
		// cout << "MC Address: " << mem_req->get_address() << " is_write: " << mem_req->get_is_write() << endl;
		this->hbm_array[this->address_to_index_converter(aligned_address)]->AddTransaction(aligned_address, mem_req->get_is_write());
		// return (this->return_routine(SIM_END_FALSE, COMPUTED));
		// return;
	}
	else
	{
		// Add the memory request to the existing entry in address_mem_req_map
		// cout << "MC Address merged: " << mem_req->get_address() << endl;
		this->mem_addresses_merged++;
		this->address_mem_req_map[aligned_address].push_back(mem_req);
		// return (this->return_routine(SIM_END_FALSE, COMPUTED));
		// return;
	}
	return;
}

void MemoryController::push_to_mc_accessed(long long address, long long cycle, bool is_write)
{
	if (this->accessed_size >= (MC_MAX_ACCESSED_COUNT))
	{
		cout << YELLOW_START << "WARN" << COLOR_RESET << "MemoryController::push_to_mc_accessed() - Accessed array full" << endl;
		cout << "Increase the value of MC_MAX_ACCESSED_COUNT in config" << endl;
		return;
		// exit(1);
	}
	this->accessed_address_array[this->accessed_size] = address;
	this->accessed_at_cycle_array[this->accessed_size] = cycle;
	this->accessed_is_write_array[this->accessed_size] = is_write;
	this->accessed_size++;
	return;
}

void MemoryController::push_to_rejected_mc_accessed(long long address, long long cycle, int port_number)
{
	if (this->rejected_accessed_size >= (MC_MAX_ACCESSED_COUNT - 10))
	{
		cout << YELLOW_START << "WARN" << COLOR_RESET << "MemoryController::push_to_rejected_mc_accessed() - Accessed array full" << endl;
		cout << "Increase the value of MC_MAX_ACCESSED_COUNT in config" << endl;
		return;
		// exit(1);
	}
	this->rejected_accessed_address_array[this->rejected_accessed_size] = address;
	this->rejected_accessed_at_cycle_array[this->rejected_accessed_size] = cycle;
	this->rejected_accessed_port_number_array[this->rejected_accessed_size] = (long)port_number;
	this->rejected_accessed_size++;
	return;
}

void MemoryController::print()
{
	cout << "MemoryController ID: " << this->id << endl;
}

void MemoryController::statistics()
{
	// Print statistics
	cout << "***************" << YELLOW_START "MemoryController ID: " << this->id << " UID: " << this->uid << COLOR_RESET << "***************" << endl;
	cout << RED_START << "Stalls" COLOR_RESET << ": " << this->idle_cycles << "\t";
	cout << GREEN_START << "Compute" << COLOR_RESET << ": " << this->busy_cycles << endl;
	cout << "Neighbors: " << this->neighbors_uid.size() << endl;
	cout << "Neighbors: ";
	for (auto neighbor_uid : this->neighbors_uid)
	{
		cout << this->gcp->uid_to_vertex_name_map[neighbor_uid] << " ";
	}
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

void MemoryController::smart_stats()
{
	// TODO: Pending implementation
	// cout << "MemoryController ID: " << this->id << endl;
	cout << "Mem addresses merged: " << this->mem_addresses_merged << endl;
	cout << "Generating Stall Profiles..." << endl;
}

bool MemoryController::sanity_check()
{
	// Check if all the ports are empty
	for (int i = 0; i < this->port_count; i++)
	{
		if (!this->in_port_array[i].empty())
		{
			cout << SANITY_CHECK << "MemoryController ID: ";
			cout << this->id << " Port ID: " << i;
			cout << " Input Port not empty" << endl;
			return false;
		}
		if (!this->out_port_array[i].empty())
		{
			cout << SANITY_CHECK << "MemoryController ID: ";
			cout << this->id << " Port ID: " << i;
			cout << " Output Port not empty" << endl;
			return false;
		}
	}
	return true;
}

bool MemoryController::get_sim_end()
{
	return this->sim_end;
}

void MemoryController::post_sim_metrics()
{
	this->write_accessed_to_file();
	cout << "MC Port Stalls" << endl;
	cout << "W0: " << this->port_stalls[0];
	cout << " W1: " << this->port_stalls[1];
	for (int i = 2; i < this->port_count; i++)
	{
		cout << " R" << i << ": " << this->port_stalls[i];
	}
	cout << endl;
	// cout << "Post Sim Metrics for MemoryController ID: " << this->id << endl;
	this->avg_in_flight_requests /= (double)this->gcp->get_cycle();
	// cout << "Writing to MongoDB" << endl;
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("busy_cycles", (long)this->busy_cycles),
			kvp("idle_cycles", (long)this->idle_cycles),
			kvp("stalls_writer_0", (long)this->port_stalls[0]),
			kvp("stalls_writer_1", (long)this->port_stalls[1]),
			kvp("stalls_ch_0", (long)this->port_stalls[2]),
			kvp("stalls_ch_1", (long)this->port_stalls[3]),
			kvp("stalls_ch_2", (long)this->port_stalls[4]),
			kvp("stalls_ch_3", (long)this->port_stalls[5]),
			kvp("stalls_ch_4", (long)this->port_stalls[6]),
			kvp("stalls_ch_5", (long)this->port_stalls[7]),
			kvp("stalls_ch_6", (long)this->port_stalls[8]),
			kvp("stalls_ch_7", (long)this->port_stalls[9]),
			kvp("hbm_0_reqs", (long)this->hbm_0_reqs),
			kvp("hbm_1_reqs", (long)this->hbm_1_reqs),
			kvp("hbm_0_reads", (long)this->hbm_0_reads),
			kvp("hbm_1_reads", (long)this->hbm_1_reads),
			kvp("hbm_0_writes", (long)this->hbm_0_writes),
			kvp("hbm_1_writes", (long)this->hbm_1_writes),

			kvp("avg_in_flight_requests", (double)this->avg_in_flight_requests),
			kvp("rejected_requests", (long)this->rejected_requests)));
	}
	this->print_json();
	return;
}

void MemoryController::write_accessed_to_file()
{
	// cout << "Writing to file" << endl;
	// Write mc_accessed to file
	string file_name = this->gcp->get_results_dir() + "/" + "mc_accessed_" + to_string(this->id) + ".csv";
	ofstream file;
	file.open(file_name);
	// Write header
	file << "address,cycle,is_write" << endl;
	for (int i = 0; i < this->accessed_size; i++)
	{
		file << this->accessed_address_array[i] << "," << this->accessed_at_cycle_array[i] << "," << this->accessed_is_write_array[i] << endl;
	}
	file.close();
	// Write rejected_mc_accessed to file
	string rejected_file_name = this->gcp->get_results_dir() + "/" + "rejected_mc_accessed_" + to_string(this->id) + ".csv";
	ofstream rejected_file;
	rejected_file.open(rejected_file_name);
	// Write header
	rejected_file << "address,cycle,port_number" << endl;
	for (int i = 0; i < this->rejected_accessed_size; i++)
	{
		rejected_file << this->rejected_accessed_address_array[i] << "," << this->rejected_accessed_at_cycle_array[i] << "," << this->rejected_accessed_port_number_array[i] << endl;
	}
	rejected_file.close();
	return;
}

void MemoryController::interval_metrics()
{
	this->interval_avg_in_flight_requests /= (double)this->gcp->INTERVAL;
	if (MONGO_FLAG)
	{
		this->collection.insert_one(make_document(
			kvp("name", "interval_metrics"),
			kvp("interval_id", (long)this->interval_id),
			kvp("interval_busy_cycles", (long)this->interval_busy_cycles),
			kvp("interval_idle_cycles", (long)this->interval_idle_cycles),
			kvp("interval_avg_in_flight_requests", (double)this->interval_avg_in_flight_requests),
			kvp("interval_rejected_requests", (long)this->interval_rejected_requests)));
	}
	this->interval_id++;
	this->interval_busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_avg_in_flight_requests = 0;
	return;
}

bool MemoryController::return_routine(bool sim_end, bool computed)
{
	this->sim_end = sim_end;
	this->avg_in_flight_requests += this->address_mem_req_map.size();
	this->interval_avg_in_flight_requests += this->address_mem_req_map.size();
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
	return this->sim_end;
}

// Gets the next instruction from all ports OR NULL if no instruction is available
// @return: Pointer to the next instruction
// @return: NULL if no instruction is available
// @param: None
MemRequest *MemoryController::get_next_memory_request()
{
	for (int i = 0; i < this->port_count; i++)
	{
		int port_id = (this->last_processed_port + i) % this->port_count;
		if (this->in_port_array[port_id].size() > 0)
		{
			InstX *instx = this->in_port_array[port_id].front();
			if (instx->get_type() != INSTX_TYPE_MEM_REQ)
			{
				cout << FATAL_ERROR << "MemoryController ID: " << this->id;
				cout << " Instruction Type: " << instx->get_type();
				cout << " is not a memory request" << endl;
				exit(1);
			}
			else
			{
				MemRequest *mem_req = (MemRequest *)instx;
				long long address = mem_req->get_address();
				if (this->hbm_array[this->address_to_index_converter(address)]->WillAcceptTransaction(address, mem_req->get_is_write()))
				{
					this->in_port_array[port_id].pop();
					this->last_processed_port = port_id;
					// Instruction fetch by NeuraMem consumes 1 cycle
					this->processing_counter += 1;
					return mem_req;
				}
				// cout << "MC rejected transaction: " << address << endl;
				this->rejected_requests++;
				this->interval_rejected_requests++;
			}
		}
	}
	// No instruction available
	return NULL;
}

bool MemoryController::tick_port(int port_id)
{
	// Check if the port contains an instruction
	if (this->in_port_array[port_id].size() > 0)
	{
		InstX *instx = this->in_port_array[port_id].front();
		if (instx->get_type() != INSTX_TYPE_MEM_REQ)
		{
			cout << FATAL_ERROR << "MemoryController ID: " << this->id;
			cout << " Instruction Type: " << instx->get_type();
			cout << " is not a memory request" << endl;
			exit(1);
		}
		else
		{
			MemRequest *mem_req = (MemRequest *)instx;
			long long address = mem_req->get_address();
			long long aligned_address = (address >> ALIGN_BIT_SHIFT) << ALIGN_BIT_SHIFT;

			// Check if the MEM_REQ is a read request
			if (mem_req->get_is_read())
			{
				// Check if the aligned address is present in L2 Cache
				// if (this->l2_cache->is_present(aligned_address))
				// {
				// cout << "MC L2 Hit: " << aligned_address << endl;
				this->in_port_array[port_id].pop();
				this->last_processed_port = port_id;
				// Instruction fetch by NeuraMem consumes 1 cycle
				this->processing_counter += 1;

				// if (mem_req->get_src_hash_req() == 0)
				// {
				// 	mem_req->print();
				// }
				this->send_memory_response(mem_req);
				this->push_to_mc_accessed(aligned_address, this->gcp->get_cycle(), mem_req->get_is_write());

				// this->update_mem_req_map_and_add_transaction(mem_req);
				return MC_PORT_TICKED; // True
				// }
			}

#ifdef MC_DISABLE_WRITE
			if (mem_req->get_is_read() == false)
			{
				this->processing_counter += 1;
				this->push_to_mc_accessed(aligned_address, this->gcp->get_cycle(), mem_req->get_is_write());
				return MC_PORT_TICKED; // True
			}
#endif

			if (this->hbm_array[this->address_to_index_converter(aligned_address)]->WillAcceptTransaction(aligned_address, mem_req->get_is_write()))
			{
				this->in_port_array[port_id].pop();
				this->last_processed_port = port_id;
				// Instruction fetch by NeuraMem consumes 1 cycle
				this->processing_counter += 1;

				this->update_mem_req_map_and_add_transaction(mem_req);
				return MC_PORT_TICKED; // True
			}
			else
			{
				if (this->address_mem_req_map.find(aligned_address) != this->address_mem_req_map.end())
				{
					// Add the memory request to the existing entry in address_mem_req_map
					// cout << "MC Address merged: " << mem_req->get_address() << endl;
					this->mem_addresses_merged++;
					this->push_to_mc_accessed(aligned_address, this->gcp->get_cycle(), mem_req->get_is_write());
					this->address_mem_req_map[aligned_address].push_back(mem_req);
					// return (this->return_routine(SIM_END_FALSE, COMPUTED));
					// return;
					return MC_PORT_TICKED; // True
				}
				else
				{
					// cout << "MC rejected transaction: " << address << endl;
					this->push_to_rejected_mc_accessed(aligned_address, this->gcp->get_cycle(), port_id);

					this->rejected_requests++;
					this->interval_rejected_requests++;
					return MC_PORT_TICKED; // True
				}
			}
		}
	}
	// No instruction available
	return MC_PORT_EMPTY; // False
}

void MemoryController::send_memory_response(MemRequest *mem_req)
{
	// Generate a memory response
	MemResponse *mem_resp = this->generate_memory_response(mem_req);

	// TODO: Analyze the impact of this
	// mem_resp->set_operand_id(mem_req->get_operand_id());

// Send the memory response to the destination unit
#ifdef TRACE_INSTX
	cout << "MC sending response with destination: " << this->gcp->uid_to_vertex_name_map[mem_resp->get_dest_unit_uid()] << endl;
#endif

	if (mem_resp->get_src_unit_uid() == 0 && mem_resp->get_dest_unit_uid() == 0)
	{
		cout << FATAL_ERROR << "MemoryController::send_memory_response() - src_unit_uid and dest_unit_uid cannot be 0" << endl;
		mem_req->print();
		mem_resp->print();
		exit(1);
	}

	int dest_port_id = this->get_dest_port(mem_resp->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(mem_resp);
	// Instruction send by MemoryController consumes 1 cycle
	this->processing_counter += 1;
	return;
}

MemResponse *MemoryController::generate_memory_response(MemRequest *mem_req)
{
	/*
			MemResponse(	GlobalCounters *gcp,
						int src_unit_uid,
						int dest_unit_uid,
						int resp_type,
						int src_address,
						int size,
						int payload,
						long long src_mem_req_id,
						MemRequest *src_mem_req,
						HashRequest *src_hash_req);
	*/
	int response_type = mem_req->get_is_write() ? MEM_RESP_WRITE : MEM_RESP_READ;
	MemResponse *mem_resp = new MemResponse(this->gcp,
											this->id,
											mem_req->get_src_unit_uid(),
											response_type,
											mem_req->get_address(),
											mem_req->get_size(),
											this->address_data_map[mem_req->get_address()],
											mem_req->get_id(),
											mem_req,
											NULL);
	mem_resp->type = INSTX_TYPE_MEM_RESP;
	// cout << "MC generated response to address: " << mem_req->get_address() << " with data: " << this->address_data_map[mem_req->get_address()] << endl;
	return mem_resp;
}

// int MemoryController::get_port_from_uid(int uid)
// {
// 	// Sending everything to port 0 for now
// 	return 0; // TODO: Implement this
// }

void MemoryController::load_dram_data()
{
	// cout << "Loading DRAM data from file: " << this->dram_data_file_name << endl;
	// Load the data from the DRAM data file
	fstream dram_data_file;
	dram_data_file.open(this->dram_data_file_name_0, ios::in);
	if (!dram_data_file.is_open())
	{
		cout << FATAL_ERROR << "Could not open DRAM data file: " << this->dram_data_file_name_0 << endl;
		exit(1);
	}
	// Read a single line from program file
	string line, word;
	while (getline(dram_data_file, line))
	{
		if (line == "")
		{
			continue;
		}
		stringstream s(line);
		uint64_t address;
		int data;
		getline(s, word, ',');
		address = stoll(word);
		getline(s, word, ',');
		data = stoi(word);
		// cout << "Address: " << address << " Data: " << data << endl;
		this->address_data_map[address] = data;
	}
	dram_data_file.close();
	return;
}

void MemoryController::print_stats()
{
	cout << "MemoryController ID: " << this->id << endl;
	cout << "Total Busy Cycles: " << this->busy_cycles << endl;
	cout << "Total Idle Cycles: " << this->idle_cycles << endl;
	cout << "Total Rejected Requests: " << this->rejected_requests << endl;
	cout << "Total Average In Flight Requests: " << (double)this->avg_in_flight_requests / (double)this->interval_id << endl;
	cout << "DRAM Stats: " << endl;
	return;
}

void MemoryController::print_json()
{
	// cout << "PrintStats called" << endl;
	this->hbm_0->PrintStats();
	this->hbm_1->PrintStats();

	// BUG: You cannot simply upload the json file to mongodb.
	/*
	cout << "json_file_name" << endl;
	string json_file_name = this->dram_output_dir + "/dramsim3.json";
	// Read json file
	cout << "ifstream json_file" << endl;
	ifstream json_file(json_file_name);
	cout << "json_str" << endl;
	string json_str((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
	// Push the json file to mongodb
	cout << "json_doc" << endl;
	bsoncxx::document::value json_doc = bsoncxx::from_json(json_str);
	cout << "Inserting json doc into mongodb" << endl;
	this->dram_collection.insert_one(json_doc.view());
	cout << "Inserted json doc into mongodb" << endl;
	// Close the json file
	json_file.close();
	*/
	return;
}

// @param dest_uid: Destination Unit UID
// @return: Port ID to send data on
// @description: Finds the best port to send data using the best routing algorithm
int MemoryController::get_dest_port(int dest_uid)
{
	int min_distance_port = -1;
	min_distance_port = packet_routing(dest_uid, this->gcp, &this->neighbors_uid_only_routers, &this->uid_to_port_map);
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

void MemoryController::create_arch_graph_distances()
{
	arch_distancing(this->gcp,
					this->arch_graph_distances,
					this->vertex_name,
					this->uid,
					this->neighbors_desc,
					this->neighbors_uid);
	for (auto neighbor_uid : this->neighbors_uid)
	{
		if (this->gcp->is_router(neighbor_uid))
		{
			this->neighbors_uid_only_routers.push_back(neighbor_uid);
		}
	}
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

string MemoryController::get_vertex_name()
{
	return (this->vertex_name);
}

int MemoryController::address_to_index_converter(long long address)
{
	// Index is either 0 or 1 depending on the aligned address
	int index = (address >> ALIGN_BIT_SHIFT) & 1; // Index is just the last bit of the aligned address
	if (index == 0)
	{
		this->hbm_0_reqs++;
	}
	else
	{
		this->hbm_1_reqs++;
	}
	return index;
}