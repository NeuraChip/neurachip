#ifndef GLOBAL_COUNTERS_H
#define GLOBAL_COUNTERS_H

#include <iostream>
#include "../mongo/mongo.h"
// #include "../neuracore/neuracore.h"
// #include "../neuramem/neuramem.h"
// #include "../neuratile/neuratile.h"
// #include "../mc/memorycontroller.h"
// #include "../dispatcher/dispatcher.h"
// #include "../writer/writer.h"
#include <chrono>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>
#include "../argparse/argparse.h"
// Include for running system function
#include <cstdlib>

using chrono::high_resolution_clock;

using namespace std;

typedef boost::property<boost::edge_weight_t, int> EdgeWeightProperty;
// typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::no_property, EdgeWeightProperty> Graph;
// typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::property<boost::vertex_name_t, std::string>> Graph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::property<boost::vertex_name_t, std::string>, EdgeWeightProperty> Graph;


typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;

/*
   ____ _       _           _  ____                  _                
  / ___| | ___ | |__   __ _| |/ ___|___  _   _ _ __ | |_ ___ _ __ ___ 
 | |  _| |/ _ \| '_ \ / _` | | |   / _ \| | | | '_ \| __/ _ \ '__/ __|
 | |_| | | (_) | |_) | (_| | | |__| (_) | |_| | | | | ||  __/ |  \__ \
  \____|_|\___/|_.__/ \__,_|_|\____\___/ \__,_|_| |_|\__\___|_|  |___/
                                                                      
*/

// Global Counters
class GlobalCounters {
	public:
		// Constructor
		GlobalCounters(ArgParse *arg_parse);
		~GlobalCounters();
		void print();
		long long generate_mem_req_id();
		long long generate_mem_resp_id();
		long long generate_hash_req_id();
		long long generate_hash_resp_id();
		int generate_core_id();
		int generate_tile_id();
		int generate_mem_id();
		int generate_router_id();
		int generate_conn_id();
		int generate_dispatcher_id();
		int generate_mc_id();
		int generate_writer_id();
		void tick();
		void statistics();
		int generate_uid();
		bool sanity_check();
		long long get_cycle();
		void post_sim_metrics();
		void set_sim_start_time();
		void interval_metrics();
		void set_core_id_array(int index, int uid);
		void set_tile_id_array(int index, int uid);
		void set_mem_id_array(int index, int uid);
		void set_mc_id_array(int index, int uid);
		void set_writer_id_array(int index, int uid);
		void set_dispatcher_id_array(int index, int uid);
		void set_router_id_array(int index, int uid);
		int *get_core_id_array();
		int *get_tile_id_array();
		int *get_mem_id_array();
		int *get_mc_id_array();
		int *get_writer_id_array();
		int *get_dispatcher_id_array();
		int *get_router_id_array();
		int get_total_cores();
		int get_total_tiles();
		int get_total_mems();
		int get_total_mcs();
		int get_total_routers();
		int get_total_writers();
		int get_total_dispatchers();
		string get_exp_name();
		Mongo *mongo;
		void print_sim_speed();
		void set_program_file(string program_file);
		string get_program_file();
		string get_program_data_folder();
		string get_dram_data_file_name_0();
		string get_dram_data_file_name_1();
		string get_results_dir();
		long long get_frequency();
		long long get_hbm_frequency();
		// TODO: Make ArgParse private
		ArgParse *arg_parse;
		int test_stride;
		void set_sim_progress(int progress);

		// Configurable parameters
		int NEURA_CORE_X_COUNT;
		int NEURA_CORE_Y_COUNT;

		// Last port is for Instructions from Dispatcher
		int NEURA_CORE_PORT_COUNT;
		int NEURA_CORE_DATA_PORT_COUNT;
		// Instruction Port Index for NeuraCores
		int INSTX_PORT_INDEX;
		// Core register file size
		// Number of Instructions that can be stored in register file
		// int PIPELINE_COUNT 4
		int PIPELINE_COUNT;

		// Memory Config
		int NEURA_MEM_X_COUNT;
		int NEURA_MEM_Y_COUNT;
		int NEURA_MEM_PORT_COUNT;
		
		// Hashpad
		int HASHPAD_COMPARATOR_COUNT;

		// Tile Config
		int NEURA_TILE_X_COUNT;
		int NEURA_TILE_Y_COUNT;
		int NEURA_TILE_SIZE;
		int NEURA_TILE_PORT_COUNT;

		// Memory Controller Config
		int MC_COUNT;
		int MC_PORT_COUNT;

		// Router Config
		int NEURA_ROUTER_X_COUNT;
		int NEURA_ROUTER_Y_COUNT;
		int NEURA_ROUTER_PORT_COUNT;
		int NEURA_ROUTER_PROCESSING_CYCLES;
		size_t NEURA_ROUTER_BUFFER_SIZE;
	
		// Writer Config
		int WRITER_COUNT;
		int WRITER_PORT_COUNT;

		// Cache Config
		// int CACHE_LINES_COUNT 4096
		int CACHE_LINES_COUNT;
		int CACHE_REQUEST_TABLE_SAME_EXTENDED;
		int CACHE_REQUEST_TABLE_NEW_CREATED;


		// Timings
		int DRAM_CONST_LATENCY;
		int IF_PROCESSING_CYCLES;
		int MULT_PROCESSING_CYCLES;

		// Dispatcher Config
		// int DISPATCHER_X_COUNT;
		// int DISPATCHER_Y_COUNT;
		int DISPATCHER_COUNT;
		// Dispatcher port count
		int DISPATCHER_PORT_COUNT;
		// Constant
		int DISPATCH_INF_INSTX;
		// Configurable
		// Just Table 0
		// int DISPATCH_INSTX_COUNT 53
		int DISPATCH_INSTX_COUNT;
		// Entire Workload
		// int DISPATCH_INSTX_COUNT 192885
		// Sample Instruction | Warning: This will never HALT


		// int DISPATCH_INSTX_COUNT DISPATCH_INF_INSTX

		int INTERVAL;
		int MIN_CYCLES;
		int HARD_STOP;

		// int INTERMEDIATE_STAT_PRINT;

		// Frequency 1.0 GHz
		long long FREQUENCY;
		long long HBM_FREQUENCY;
		Graph arch_graph;
		boost::property_map<Graph, boost::vertex_name_t>::type vertex_desc_to_name_map = get(boost::vertex_name, this->arch_graph);
		// Inverse Vertex Map
		map<string, VertexDescriptor> vertex_name_to_desc_map;
		// UID to Vertex Map
		map<int, VertexDescriptor> vertex_uid_to_desc_map;
		void add_vertex_to_arch_graph(string vertex_name, int uid);
		// void add_edge_to_arch_graph(string vertex_1, string vertex_2, int edge_1_weight);
		void add_edge_to_arch_graph(string vertex_1, string vertex_2, int edge_1_weight, int edge_2_weight);
		void visualize_arch_graph();
		map<int, vector<int>*> uid_to_arch_graph_distances_ptr_map;
		map<VertexDescriptor, int> vertex_desc_to_uid_map;
		map<int, string> uid_to_vertex_name_map;
		void smart_stats();
		bool is_router(int uid);
		void cycle_printer();
		void cycles_to_time_printer();
		void sim_time_printer();
		void end_sim_time();
		long long get_clock_frequency();
		// Histogram Collection for reg_file_time of HashRequests in Cores
		vector<int> rf_histogram;
		void write_vector_to_csv(string file_name, vector<long long> data);
		void write_vector_to_csv(string file_name, vector<long> data);
		void write_vector_to_csv(string file_name, vector<int> data);
		map<uint64_t, int> *address_data_map_ptr;
	private:
		// ArgParse *arg_parse;
		string program_file;
		string program_data_folder;
		int uid;
		int core_id;
		long long mem_req_id;
		long long mem_resp_id;
		long long hash_req_id;
		long long hash_resp_id;
		int tile_id;
		int mem_id;
		int router_id;
		long long cycle;
		int conn_id;
		int dispatcher_id;
		int mc_id;
		int writer_id;
		int *core_id_array;
		int *tile_id_array;
		int *mem_id_array;
		int *mc_id_array;
		int *writer_id_array;
		int *dispatcher_id_array;
		int *router_id_array;
		int total_cores;
		int total_tiles;
		int total_mems;
		int total_mcs;
		int total_routers;
		int total_writers;
		int total_dispatchers;
		// Frequency in Hz
		long long frequency;
		// Million Cycles Per Second
		float mcps;
		float interval_mcps;
		// Instructions Per Second
		float cps;
		float interval_cps;
		long long interval_id;
		high_resolution_clock::time_point start_time;
		high_resolution_clock::time_point interval_start_time;
		high_resolution_clock::time_point end_time;
		high_resolution_clock::time_point interval_end_time;
		double sim_time;
		double interval_sim_time;
		string component_name;
		string header_component_name;
		mongocxx::collection collection;
		mongocxx::collection header_collection;
		string exp_name;
		string dram_data_file_name_0;
		string dram_data_file_name_1;
		string results_base_dir;
		string results_dir;
		int interval_sim_progress;
		void set_configurable_parameters(ArgParse *arg_parse);
		void test_arch_graph();
		
};

#endif // !GLOBAL

