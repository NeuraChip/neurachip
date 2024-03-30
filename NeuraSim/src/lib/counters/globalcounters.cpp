// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "globalcounters.h"
#include "../debugger/debugger.h"
#include "../config.h"
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

using chrono::high_resolution_clock;

using namespace std;

// Constructor
GlobalCounters::GlobalCounters(ArgParse *arg_parse)
{
	// Start cycle count from 1
	// To avoid confusion of first INTERVAL being INTERVAL+1
	this->set_configurable_parameters(arg_parse);
	this->cycle = 1;
	this->mem_req_id = -1;
	this->mem_resp_id = -1;
	this->core_id = -1;
	this->mem_id = -1;
	this->router_id = -1;
	this->tile_id = -1;
	this->conn_id = -1;
	this->uid = -1;
	this->dispatcher_id = -1;
	this->mc_id = -1;
	this->writer_id = -1;
	this->hash_req_id = -1;
	this->hash_resp_id = -1;
	this->mcps = 0;
	this->cps = 0;
	this->sim_time = 0;
	this->interval_sim_time = 0;
	this->frequency = this->FREQUENCY;
	// this->results_base_dir = "/home/ktb/git/NeuraChip/results_neurachip/";
	this->results_base_dir = "/home/ktb/git/NeuraChip/NeuraSim/results_neurachip/";
	this->interval_sim_progress = 0;

	this->exp_name = arg_parse->get_exp_name();
	this->test_stride = arg_parse->get_stride();
	this->results_dir = this->results_base_dir + this->exp_name;

	// Disabled when running through script
	// Check if the results directory exists
	// If it exists, delete it
	if (access(this->results_dir.c_str(), F_OK) != -1) {
		// cout << "Directory exists" << endl;
		// cout << "Deleting directory: " << this->results_dir << endl;
		// Use rm -rf
		string cmd = "rm -rf " + this->results_dir;
		system(cmd.c_str());
	} else {
		cout << "Directory does not exist" << endl;
	}

	// Create the results directory
	if (mkdir(this->results_dir.c_str(), 0777) == -1) {
		cout << "Error creating directory: " << this->results_dir << endl;
	} else {
		// cout << "Directory created: " << this->results_dir << endl;
		cout << "Directory created." << endl;
	}




	this->dram_data_file_name_0 = arg_parse->get_dataset_root() + "/" + "raw_data" + "/" + "dram_raw_data_0.csv";
	this->dram_data_file_name_1 = arg_parse->get_dataset_root() + "/" + "raw_data" + "/" + "dram_raw_data_1.csv";
	this->program_file = arg_parse->get_dataset_root() + "/" + "bin" + "/" + "spmm.csv";
	// cout << "Program File: " << this->program_file << endl;
	this->program_data_folder = arg_parse->get_dataset_root() + "/" + "matrix";
	// Create a new database with the experiment name
	this->mongo = new Mongo(arg_parse->get_exp_name());
	this->mongo->drop();
	this->component_name = "GlobalCounters";
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->mongo->get_component_collection(this->component_name);
	this->header_collection = this->mongo->get_component_collection(this->header_component_name);
	this->interval_id = 0;
	this->total_cores = this->NEURA_CORE_X_COUNT * this->NEURA_CORE_Y_COUNT;
	this->total_tiles = this->NEURA_TILE_X_COUNT * this->NEURA_TILE_Y_COUNT;
	this->total_mems = this->NEURA_MEM_X_COUNT * this->NEURA_MEM_Y_COUNT;
	this->total_mcs = this->MC_COUNT;
	this->total_writers = this->WRITER_COUNT;
	this->total_dispatchers = this->DISPATCHER_COUNT;
	this->total_routers = this->NEURA_ROUTER_X_COUNT * this->NEURA_ROUTER_Y_COUNT;
	this->core_id_array = new int[this->total_cores];
	for (int i = 0; i < this->total_cores; i++)
	{
		this->core_id_array[i] = -1;
	}
	this->tile_id_array = new int[this->total_tiles];
	for (int i = 0; i < this->total_tiles; i++)
	{
		this->tile_id_array[i] = -1;
	}
	this->mem_id_array = new int[this->total_mems];
	for (int i = 0; i < this->total_mems; i++)
	{
		this->mem_id_array[i] = -1;
	}
	this->mc_id_array = new int[this->total_mcs];
	for (int i = 0; i < this->total_mcs; i++)
	{
		this->mc_id_array[i] = -1;
	}
	this->writer_id_array = new int[this->total_writers];
	for (int i = 0; i < this->total_writers; i++)
	{
		this->writer_id_array[i] = -1;
	}
	this->dispatcher_id_array = new int[this->total_dispatchers];
	for (int i = 0; i < this->total_dispatchers; i++)
	{
		this->dispatcher_id_array[i] = -1;
	}
	this->router_id_array = new int[this->total_routers];
	for (int i = 0; i < this->total_routers; i++)
	{
		this->router_id_array[i] = -1;
	}

	this->test_arch_graph();
}



void GlobalCounters::set_configurable_parameters(ArgParse *arg_parse)
{
	/*
	  ____ ___  _   _ _____ ___ ____
 	 / ___/ _ \| \ | |  ___|_ _/ ___|
	| |  | | | |  \| | |_   | | |  _
	| |__| |_| | |\  |  _|  | | |_| |
 	 \____\___/|_| \_|_|   |___\____|
	*/
	// Tile Config
	this->NEURA_TILE_SIZE = arg_parse->get_tile_size();
	this->NEURA_TILE_X_COUNT = 1;
	this->NEURA_TILE_Y_COUNT = 1;
	this->NEURA_TILE_PORT_COUNT = 4;

	// Core Config
	this->NEURA_CORE_X_COUNT = this->NEURA_TILE_SIZE;
	this->NEURA_CORE_Y_COUNT = this->NEURA_TILE_SIZE;

	// Last port is for Instructions from Dispatcher
	this->NEURA_CORE_PORT_COUNT = 5;
	this->NEURA_CORE_DATA_PORT_COUNT = 4;
	// Instruction Port Index for NeuraCores
	this->INSTX_PORT_INDEX = 4;
	// Core register file size
	// Number of Instructions that can be stored in register file
	// this->PIPELINE_COUNT = 16;
	// this->PIPELINE_COUNT = 64;
	this->PIPELINE_COUNT = arg_parse->get_pipeline_count();

	// Memory Config
	this->NEURA_MEM_X_COUNT = this->NEURA_TILE_SIZE;
	this->NEURA_MEM_Y_COUNT = this->NEURA_TILE_SIZE;
	this->NEURA_MEM_PORT_COUNT = 4;

	// Hashpad
	this->HASHPAD_COMPARATOR_COUNT = arg_parse->get_comparator_count();

	// Memory Controller Config
	this->MC_COUNT = 1;
	this->MC_PORT_COUNT = 10;

	// Router Config
	// this->NEURA_ROUTER_X_COUNT = 2 * NEURA_TILE_SIZE + 1;
	// this->NEURA_ROUTER_Y_COUNT = NEURA_TILE_SIZE + 1;
	this->NEURA_ROUTER_X_COUNT = 2 * NEURA_TILE_SIZE;
	this->NEURA_ROUTER_Y_COUNT = NEURA_TILE_SIZE;
	// Last port is for writer and memory controller
	this->NEURA_ROUTER_PORT_COUNT = 9;
	this->NEURA_ROUTER_PROCESSING_CYCLES = 0;
	this->NEURA_ROUTER_BUFFER_SIZE = 32;

	// Writer Config
	this->WRITER_COUNT = 2;
	this->WRITER_PORT_COUNT = 2;

	// Cache Config
	// this->CACHE_LINES_COUNT 4096

	this->CACHE_LINES_COUNT = arg_parse->get_cache_lines_count();

	// Timings
	this->DRAM_CONST_LATENCY = 300;
	this->IF_PROCESSING_CYCLES = 1;
	this->MULT_PROCESSING_CYCLES = 1;

	// Constant
	this->DISPATCH_INF_INSTX = -10; // Constant, do not comment

	// Dispatcher Config
	// this->DISPATCHER_X_COUNT = this->NEURA_CORE_X_COUNT;
	// this->DISPATCHER_Y_COUNT = this->NEURA_CORE_Y_COUNT;


	// Create 1 dispatcher and connect it to all cores
	this->DISPATCHER_COUNT = 1;
	// Dispatcher port count

	// Using 1 dispatcher per core
	this->DISPATCHER_PORT_COUNT = this->NEURA_CORE_X_COUNT * this->NEURA_CORE_Y_COUNT;

	// Configurable
	// this->DISPATCH_INSTX_COUNT = 1;
	// this->DISPATCH_INSTX_COUNT = 0;
	// Just Table 0
	// this->DISPATCH_INSTX_COUNT = 53;
	// this->DISPATCH_INSTX_COUNT = 118;
	// this->DISPATCH_INSTX_COUNT = 572; // 10 Rows
	// Entire Workload
	// this->DISPATCH_INSTX_COUNT = 192885;
	// Sample Instruction | Warning: This will never HALT

	this->DISPATCH_INSTX_COUNT = DISPATCH_INF_INSTX;

	this->INTERVAL = 1000;
	this->MIN_CYCLES = 0;
	this->HARD_STOP = 400000000;
	// this->HARD_STOP = 100;

	// this->INTERMEDIATE_STAT_PRINT = 10000;

	// Frequency 1.0 GHz
	this->FREQUENCY = 1000000000;
	this->frequency = this->FREQUENCY;
	this->HBM_FREQUENCY = 3200000000;
}

void GlobalCounters::test_arch_graph()
{
	cout << "Testing architecture graph generated \u2713" << endl;

	// Add vertices
	// this->add_vertex_to_arch_graph("C_0_0");
	// this->add_vertex_to_arch_graph("C_0_1");
	// this->add_vertex_to_arch_graph("C_1_0");
	// this->add_vertex_to_arch_graph("C_1_1");

	// this->add_edge_to_arch_graph("C_0_0", "C_0_1", BIDIRECTIONAL_EDGE);
	// this->add_edge_to_arch_graph("C_0_0", "C_1_0", BIDIRECTIONAL_EDGE);
	// this->add_edge_to_arch_graph("C_0_1", "C_1_1", BIDIRECTIONAL_EDGE);
	// this->add_edge_to_arch_graph("C_1_0", "C_1_1", BIDIRECTIONAL_EDGE);

	// vector<int> distances(boost::num_vertices(this->arch_graph));
	// boost::dijkstra_shortest_paths(this->arch_graph, this->inverse_vertex_map["C_0_0"] , boost::distance_map(&distances[0]));

	// std::cout << "Distances from vertex 0:" << std::endl;
	// for(int i = 0; i < distances.size(); ++i) {
	//     std::cout << i << ": " << distances[i] << std::endl;
	// }

	// cout << "Testing Arch Graph Complete" << endl;
}

void GlobalCounters::visualize_arch_graph()
{
	cout << "Visualizing architecture graph generated \u2713" << endl;
	ofstream file("./arch_graph.dot"); // Open a file to write the DOT representation
	// boost::write_graphviz(file, this->arch_graph); // Write the graph to the file
	boost::write_graphviz(file, this->arch_graph, boost::make_label_writer(this->vertex_desc_to_name_map));
	// Run the command "dot -Tpng graph.dot > graph.png"
	system("dot -Tpng arch_graph.dot > arch_graph.png");
}

void GlobalCounters::add_vertex_to_arch_graph(string vertex_name, int uid)
{
	VertexDescriptor v_desc = boost::add_vertex(this->arch_graph);
	// cout << "Adding vertex " << vertex_name << " with uid " << uid << " and desc " << v_desc << endl;
	this->vertex_desc_to_name_map[v_desc] = vertex_name;
	this->vertex_name_to_desc_map[vertex_name] = v_desc;
	this->vertex_uid_to_desc_map[uid] = v_desc;
	this->vertex_desc_to_uid_map[v_desc] = uid;
	this->uid_to_vertex_name_map[uid] = vertex_name;
}

// Just dont create UNIDIRECTIONAL edges
// Creates a UNIDIRECTIONAL edge from vertex_1 to vertex_2
// void GlobalCounters::add_edge_to_arch_graph(string vertex_1, string vertex_2, int edge_1_weight) {
// 	if (edge_1_weight == 0) {
// 		cout << FATAL_ERROR << "Edge weight cannot be 0" << endl;
// 	}
// 	VertexDescriptor v1 = this->inverse_vertex_map[vertex_1];
// 	VertexDescriptor v2 = this->inverse_vertex_map[vertex_2];
// 	boost::add_edge(v1, v2, EdgeWeightProperty(edge_1_weight), this->arch_graph);
// }

// Create a BIDIRECTIONAL edge from vertex_1 to vertex_2
void GlobalCounters::add_edge_to_arch_graph(string vertex_1, string vertex_2, int edge_1_weight, int edge_2_weight)
{
	if (edge_1_weight == 0 || edge_2_weight == 0)
	{
		cout << FATAL_ERROR << "Edge weight cannot be 0" << endl;
	}
	VertexDescriptor v1 = this->vertex_name_to_desc_map[vertex_1];
	VertexDescriptor v2 = this->vertex_name_to_desc_map[vertex_2];
	boost::add_edge(v1, v2, EdgeWeightProperty(edge_1_weight), this->arch_graph);
	boost::add_edge(v2, v1, EdgeWeightProperty(edge_2_weight), this->arch_graph);
}

GlobalCounters::~GlobalCounters()
{
	delete this->mongo;
	delete[] this->core_id_array;
	delete[] this->tile_id_array;
	delete[] this->mem_id_array;
	delete[] this->mc_id_array;
	delete[] this->writer_id_array;
	delete[] this->dispatcher_id_array;
	delete[] this->router_id_array;
}

void GlobalCounters::print()
{
	cout << "Tile ID: " << this->tile_id << endl;
	cout << "Core ID: " << this->core_id << endl;
	cout << "Memory ID: " << this->mem_id << endl;
	cout << "Router ID: " << this->router_id << endl;
	cout << "Memory Request ID: " << this->mem_req_id << endl;
	cout << "Memory Response ID: " << this->mem_resp_id << endl;
	cout << "Connection ID: " << this->conn_id << endl;
	cout << "UID: " << this->uid << endl;
	cout << "Dispatcher ID: " << this->dispatcher_id << endl;
}

long long GlobalCounters::generate_mem_req_id()
{
	this->mem_req_id++;
	return (this->mem_req_id);
}

long long GlobalCounters::generate_mem_resp_id()
{
	this->mem_resp_id++;
	return (this->mem_resp_id);
}

long long GlobalCounters::generate_hash_req_id()
{
	this->hash_req_id++;
	return (this->hash_req_id);
}

long long GlobalCounters::generate_hash_resp_id()
{
	this->hash_resp_id++;
	return (this->hash_resp_id);
}

int GlobalCounters::generate_core_id()
{
	this->core_id++;
	return (this->core_id);
}

int GlobalCounters::generate_tile_id()
{
	this->tile_id++;
	return (this->tile_id);
}

int GlobalCounters::generate_mem_id()
{
	this->mem_id++;
	return (this->mem_id);
}

int GlobalCounters::generate_router_id()
{
	this->router_id++;
	return (this->router_id);
}

int GlobalCounters::generate_conn_id()
{
	this->conn_id++;
	return (this->conn_id);
}

int GlobalCounters::generate_uid()
{
	this->uid++;
	return (this->uid);
}

int GlobalCounters::generate_dispatcher_id()
{
	this->dispatcher_id++;
	return (this->dispatcher_id);
}

int GlobalCounters::generate_mc_id()
{
	this->mc_id++;
	return (this->mc_id);
}

int GlobalCounters::generate_writer_id()
{
	this->writer_id++;
	return (this->writer_id);
}

void GlobalCounters::set_core_id_array(int index, int uid)
{
	this->core_id_array[index] = uid;
}

void GlobalCounters::set_tile_id_array(int index, int uid)
{
	this->tile_id_array[index] = uid;
}

void GlobalCounters::set_mem_id_array(int index, int uid)
{
	this->mem_id_array[index] = uid;
}

void GlobalCounters::set_mc_id_array(int index, int uid)
{
	this->mc_id_array[index] = uid;
}

void GlobalCounters::set_writer_id_array(int index, int uid)
{
	this->writer_id_array[index] = uid;
}

void GlobalCounters::set_dispatcher_id_array(int index, int uid)
{
	this->dispatcher_id_array[index] = uid;
}

void GlobalCounters::set_router_id_array(int index, int uid)
{
	this->router_id_array[index] = uid;
}

int *GlobalCounters::get_core_id_array()
{
	return (this->core_id_array);
}

int *GlobalCounters::get_tile_id_array()
{
	return (this->tile_id_array);
}

int *GlobalCounters::get_mem_id_array()
{
	return (this->mem_id_array);
}

int *GlobalCounters::get_mc_id_array()
{
	return (this->mc_id_array);
}

int *GlobalCounters::get_writer_id_array()
{
	return (this->writer_id_array);
}

int *GlobalCounters::get_dispatcher_id_array()
{
	return (this->dispatcher_id_array);
}

int *GlobalCounters::get_router_id_array()
{
	return (this->router_id_array);
}

int GlobalCounters::get_total_cores()
{
	return (this->total_cores);
}

int GlobalCounters::get_total_tiles()
{
	return (this->total_tiles);
}

int GlobalCounters::get_total_mems()
{
	return (this->total_mems);
}

int GlobalCounters::get_total_routers()
{
	return (this->total_routers);
}

int GlobalCounters::get_total_mcs()
{
	return (this->total_mcs);
}

int GlobalCounters::get_total_writers()
{
	return (this->total_writers);
}

int GlobalCounters::get_total_dispatchers()
{
	return (this->total_dispatchers);
}

void GlobalCounters::set_program_file(string program_file)
{
	this->program_file = program_file;
}

string GlobalCounters::get_program_file()
{
	return (this->program_file);
}

// Ticks the Cycle Counter
void GlobalCounters::tick()
{
	this->cycle++;
}

void GlobalCounters::statistics()
{
	cout << "********** Final Statistics **********" << endl;
	this->sim_time_printer();
	cout << "Total Cycles: " << CYAN_START << this->cycle << COLOR_RESET << endl;
	cout << "Execution Time: ";
	this->cycles_to_time_printer();
	cout << endl;
	cout << "**************************************" << endl;
}

void GlobalCounters::cycles_to_time_printer()
{
	// Cycles are in nanoseconds
	if (this->cycle < 1000)
	{
		cout << UNDERLINE_START;
		cout << this->cycle << " ns";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000 && this->cycle < 1000000)
	{
		cout << GREEN_START;
		cout << this->cycle / 1000.0 << " us";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000000 && this->cycle < 1000000000)
	{
		cout << YELLOW_START;
		cout << this->cycle / 1000000.0 << " ms";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000000000)
	{
		cout << RED_START;
		cout << this->cycle / 1000000000.0 << " s";
		cout << COLOR_RESET;
		return;
	}
	cout << FATAL_ERROR << "Should not reach here" << endl;
}

void GlobalCounters::sim_time_printer()
{
	// Sim time is in microseconds
	// Print Hours, minutes, seconds
	cout << "Sim Time Raw: " << this->sim_time << endl;
	// Hours
	int hours = this->sim_time / 3600000000;
	// Minutes
	int minutes = (this->sim_time - (hours * 3600000000)) / 60000000;
	// Seconds
	int seconds = (this->sim_time - (hours * 3600000000) - (minutes * 60000000)) / 1000000;

	cout << "Simulation Time: ";
	cout << RED_START << hours << COLOR_RESET << "h::";
	cout << YELLOW_START << minutes << COLOR_RESET << "m::";
	cout << GREEN_START << seconds << COLOR_RESET << "s" << endl;
}

void GlobalCounters::smart_stats()
{
	// TODO: Pending
}

bool GlobalCounters::sanity_check()
{
	// Check if at least one tile is present
	if (this->tile_id == -2)
	{
		cout << SANITY_CHECK << "No Tiles Present" << endl;
		return (false);
	}
	// Check if at least one core is present
	if (this->core_id == -1)
	{
		cout << SANITY_CHECK << "No Cores Present" << endl;
		return (false);
	}
	// Check if at least one memory is present
	if (this->mem_id == -1)
	{
		cout << SANITY_CHECK << "No Memories Present" << endl;
		return (false);
	}
	// Check if at least one router is present
	if (this->router_id == -1)
	{
		cout << SANITY_CHECK << "No Routers Present" << endl;
		// return (false);
	}
	// Check if at least one connection is present
	if (this->conn_id == -1)
	{
		cout << SANITY_CHECK << "No Connections Present" << endl;
		// return (false);
	}
	// Check if at least one dispatcher is present
	if (this->dispatcher_id == -1)
	{
		cout << SANITY_CHECK << "No Dispatchers Present" << endl;
		// return (false);
	}

	// Check if core_id_array is populated
	for (int i = 0; i < this->total_cores; i++)
	{
		if (this->core_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "Core ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if tile_id_array is populated
	for (int i = 0; i < this->total_tiles; i++)
	{
		if (this->tile_id_array[i] == -2)
		{
			cout << SANITY_CHECK << "Tile ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if mem_id_array is populated
	for (int i = 0; i < this->total_mems; i++)
	{
		if (this->mem_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "Mem ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if mc_id_array is populated
	for (int i = 0; i < this->total_mcs; i++)
	{
		if (this->mc_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "MC ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if writer_id_array is populated
	for (int i = 0; i < this->total_writers; i++)
	{
		if (this->writer_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "Writer ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if dispatcher_id_array is populated
	for (int i = 0; i < this->total_dispatchers; i++)
	{
		if (this->dispatcher_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "Dispatcher ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Check if router_id_array is populated
	for (int i = 0; i < this->total_routers; i++)
	{
		if (this->router_id_array[i] == -1)
		{
			cout << SANITY_CHECK << "Router ID Array Not Populated" << endl;
			return (false);
		}
	}

	// Mongo Sanity Check
	if (!this->mongo->sanity_check())
	{
		cout << SANITY_CHECK << "Mongo Sanity Check Failed" << endl;
		return (false);
	}
	return (true);
}

long long GlobalCounters::get_cycle()
{
	return (this->cycle);
}

void GlobalCounters::post_sim_metrics()
{
	// End sim time is computed in the end_sim_time function within the run_sim function in main.cpp
	// this->end_time = high_resolution_clock::now();
	// this->sim_time = chrono::duration_cast<chrono::microseconds>(this->end_time - this->start_time).count();
	this->mcps = (double)this->cycle / (double)this->sim_time;
	this->cps = ((double)this->cycle * 1000000) / (double)this->sim_time;
	long peak_performance = 2 * (long)this->NEURA_TILE_SIZE;
	float on_chip_memory;
	long off_chip_memory = 128;
	long technology = 7;
	float chip_area;
	float spgemm_performance;
	float area_efficiency;
	spgemm_performance = (float)((this->hash_req_id + 1) * 2) / ((float)this->cycle); // * 2 because multiply and add operations
	if (this->NEURA_TILE_SIZE == 4) {
		on_chip_memory = 0.75;
		chip_area = 2.37;
	}
	if (this->NEURA_TILE_SIZE == 16) {
		on_chip_memory = 3;
		chip_area = 10.2;
	}
	if (this->NEURA_TILE_SIZE == 64) {
		on_chip_memory = 12;
		chip_area = 35.26;
	}
	area_efficiency = (float)spgemm_performance / chip_area;
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("exp_name", this->exp_name),
			kvp("tile_size", (long)this->NEURA_TILE_SIZE),
			kvp("peak_performance", (long)peak_performance),
			kvp("on_chip_memory", (double)on_chip_memory),
			kvp("off_chip_memory", (long)off_chip_memory),
			kvp("technology", (long)technology),
			kvp("chip_area", (double)chip_area),
			kvp("spgemm_performance", (double)spgemm_performance),
			kvp("area_efficiency", (double)area_efficiency),
			kvp("pipeline_count", (long)this->PIPELINE_COUNT),
			kvp("hashpad_comparator_count", (long)this->HASHPAD_COMPARATOR_COUNT),
			kvp("cycles", (long)this->cycle),
			kvp("sim_time", (double)this->sim_time),
			kvp("mcps", (double)this->mcps),
			kvp("cps", (double)this->cps),
			kvp("total_mem_reqs", (long)(this->mem_req_id + 1)),
			kvp("total_mem_resps", (long)(this->mem_resp_id + 1)),
			kvp("total_hash_reqs", (long)(this->hash_req_id + 1)),
			kvp("total_hash_resps", (long)(this->hash_resp_id + 1))));
	}

	// Create a folder in results directory called neuracore
	string core_results_dir = this->results_dir + "/neuracore";
	mkdir(core_results_dir.c_str(), 0777);
	// rf_histogram.csv
	string rf_histogram_file_name = core_results_dir + "/rf_histogram.csv";
	this->write_vector_to_csv(rf_histogram_file_name, this->rf_histogram);
}

void GlobalCounters::end_sim_time()
{
	this->end_time = high_resolution_clock::now();
	this->sim_time = chrono::duration_cast<chrono::microseconds>(this->end_time - this->start_time).count();
}

void GlobalCounters::interval_metrics()
{
	this->interval_end_time = high_resolution_clock::now();
	this->interval_sim_time = chrono::duration_cast<chrono::microseconds>(this->interval_end_time - this->interval_start_time).count();
	this->interval_start_time = high_resolution_clock::now();
	this->interval_mcps = (double)INTERVAL / (double)this->interval_sim_time;
	this->interval_cps = ((double)INTERVAL * 1000000) / (double)this->interval_sim_time;
	if (MONGO_FLAG)
	{
		this->collection.insert_one(make_document(
			kvp("name", "interval_metrics"),
			kvp("interval_id", (long)this->interval_id),
			kvp("interval_cycles", (long)INTERVAL),
			kvp("interval_sim_time", (double)this->interval_sim_time),
			kvp("interval_mcps", (double)this->interval_mcps),
			kvp("interval_cps", (double)this->interval_cps),
			kvp("interval_sim_progress", (double)this->interval_sim_progress)));
	}
	this->interval_id++;
}

void GlobalCounters::set_sim_start_time()
{
	this->start_time = high_resolution_clock::now();
	this->interval_start_time = high_resolution_clock::now();
}

void GlobalCounters::print_sim_speed()
{
	cout << "Sim Speed: " << GREEN_START << (int)(this->interval_cps) << COLOR_RESET << " Cycles Per Second    ";
	cout << "Cycles Completed: ";
	this->cycle_printer();
	cout << endl;
}

void GlobalCounters::cycle_printer()
{
	if (this->cycle < 1000)
	{
		cout << GREEN_START;
		cout << this->cycle;
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000 && this->cycle < 1000000)
	{
		cout << YELLOW_START;
		cout << int(this->cycle / 1000) << "K";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000000 && this->cycle < 1000000000)
	{
		cout << RED_START;
		cout << int(this->cycle / 1000000) << "M";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000000000 && this->cycle < 1000000000000)
	{
		cout << CYAN_START;
		cout << int(this->cycle / 1000000000) << "B";
		cout << COLOR_RESET;
		return;
	}
	if (this->cycle >= 1000000000000)
	{
		cout << RED_START;
		cout << int(this->cycle / 1000000000000) << "T";
		cout << COLOR_RESET;
		return;
	}
	cout << FATAL_ERROR << "Should not reach here: ";
	cout << this->cycle;
	return;
}

string GlobalCounters::get_exp_name()
{
	return (this->exp_name);
}

string GlobalCounters::get_program_data_folder()
{
	return (this->program_data_folder);
}

string GlobalCounters::get_dram_data_file_name_0()
{
	return (this->dram_data_file_name_0);
}

string GlobalCounters::get_dram_data_file_name_1()
{
	return (this->dram_data_file_name_1);
}



string GlobalCounters::get_results_dir()
{
	return (this->results_dir);
}

long long GlobalCounters::get_frequency()
{
	return (this->frequency);
}

long long GlobalCounters::get_hbm_frequency()
{
	return (this->HBM_FREQUENCY);
}

void GlobalCounters::set_sim_progress(int progress)
{
	this->interval_sim_progress = progress;
}

bool GlobalCounters::is_router(int uid)
{
	for (int i = 0; i < this->total_routers; i++)
	{
		if (this->router_id_array[i] == uid)
		{
			return (true);
		}
	}
	return (false);
}

long long GlobalCounters::get_clock_frequency()
{
	return (this->frequency);
}

void GlobalCounters::write_vector_to_csv(string file_name, vector<long long> data)
{
	ofstream myfile;
	myfile.open(file_name);
	for (int i = 0; i < data.size(); i++)
	{
		myfile << data[i] << endl;
	}
	myfile.close();
}

void GlobalCounters::write_vector_to_csv(string file_name, vector<long> data)
{
	ofstream myfile;
	myfile.open(file_name);
	for (int i = 0; i < data.size(); i++)
	{
		myfile << data[i] << endl;
	}
	myfile.close();
}

void GlobalCounters::write_vector_to_csv(string file_name, vector<int> data)
{
	ofstream myfile;
	myfile.open(file_name);
	for (int i = 0; i < data.size(); i++)
	{
		myfile << data[i] << endl;
	}
	myfile.close();
}