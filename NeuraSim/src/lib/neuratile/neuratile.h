#ifndef NEURA_TILE_H
#define NEURA_TILE_H

#include <iostream>
#include <queue>
#include "../counters/globalcounters.h"
#include "../neuracore/neuracore.h"
#include "../neuramem/neuramem.h"
#include "../neurarouter/neurarouter.h"
#include "../memreq/memrequest.h"
#include "../functions.h"
#include <vector>
#include "../connector/conn.h"
#include "../dispatcher/dispatcher.h"
#include "../mc/memorycontroller.h"
#include "../writer/writer.h"
#include "../mongo/mongo.h"

using namespace std;

// A single NeuraTile
class NeuraTile {
	public:
		// Constructor
		// @param id: NeuraTile ID
		// @param x_cood: NeuraTile X Coordinate
		// @param y_cood: NeuraTile Y Coordinate
		NeuraTile(GlobalCounters *gcp, int x_cood, int y_cood, int port_count);
		NeuraTile();
		void custom_constructor(GlobalCounters *gcp, int x, int y, int port_count);
		~NeuraTile();
		void print();
		bool tick();
		void statistics();
		bool sanity_check();
		// In Port array
		queue<InstX *> *in_port_array;
		// Out Port array
		queue<InstX *> *out_port_array;
		queue<InstX *> *get_in_port_ref(int port_id);
		queue<InstX *> *get_out_port_ref(int port_id);
		string get_vertex_name();
		bool get_sim_end();
		void print_sim_end();
		void enable_max_buffer_sizes_computation();
		void post_sim_metrics();
		void self_post_sim_metrics();
		void interval_metrics();
		void self_interval_metrics();
		void set_sim_start_time();
		bool return_routine(bool sim_end, bool computed);
		int get_id();
		// int get_uid();
		GlobalCounters *get_gcp();
		void connect_units(string unit1, int port1, string unit2, int port2);
		void generate_cgra_mesh_topology();
		void create_unit_components();
		void create_arch_graph_distances_for_all_units();
		void print_unit_distances();
		void smart_stats();
		string vertex_name;
	private:
		int id;
		// NOTE: Not allocating UID to neura tile
		// Because you dont have to send data to entire tile but spcific units on the tile.
		// So not adding Neura Tile to the graph.
		// To match the vertex descriptions to UIDs, not adding UID to NeuraTile
		// int uid;
		GlobalCounters *gcp;
		int x_cood;
		int y_cood;
		int core_x_count;
		int core_y_count;
		int mem_x_count;
		int mem_y_count;
		int mem_controller_count;
		int router_x_count;
		int router_y_count;
		int writer_count;
		int port_count;
		bool sim_end;
		NeuraCore **core_array;
		NeuraMem **mem_array;
		NeuraRouter **router_array;
		MemoryController *mem_controller_array;
		Writer *writer_array;
		vector<Conn *> conn_array;
		NeuraDispatcher dispatcher;
		string program_file;
		// Name of the component
		string component_name;
		// Name of the header component
		string header_component_name;
		// Collection
		mongocxx::collection collection;
		// Collection for the header
		mongocxx::collection header_collection;
		// Metrics
		long long idle_cycles;
		long long interval_idle_cycles;
		long long busy_cycles;
		long long interval_busy_cycles;
		long long interval_id;
		bool *component_sim_end_array;

};

#endif // !NEURA_TILE_H