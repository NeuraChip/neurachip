#ifndef NEURA_DISPATCHER_H
#define NEURA_DISPATCHER_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../config.h"
#include "../instx/instx.h"
#include "../hashreq/hashrequest.h"
#include <queue>
#include "fstream"

using namespace std;

// A single NeuraDispatcher
class NeuraDispatcher
{
public:
    NeuraDispatcher();
    NeuraDispatcher(GlobalCounters *gcp, string program_file, int port_count);
    ~NeuraDispatcher();
    void custom_constructor(GlobalCounters *gcp, string program_file, int port_count);
    // Load program into NeuraDispatcher
    void load_program();
    void set_program_file(string program_file);
    bool tick();
    bool sanity_check();
    void statistics();
    fstream program_file_stream;
    long long hash_req_mapper(int b_col_id);
    // Ports connecting to each NeuraCore
    // Ports have to be public because they are accessed by Connector class
    queue<InstX *> *out_port_array;
    queue<InstX *> *in_port_array;
    queue<InstX *> *get_in_port_ref(int port_id);
    queue<InstX *> *get_out_port_ref(int port_id);
    string get_vertex_name();
    void send_hash_req(HashRequest *hr, int dest_core_id);
    void set_dispatch_count(long long dispatch_count);
    bool get_sim_end();
    void post_sim_metrics();
    void interval_metrics();
    bool return_routine(bool sim_end, bool computed);
    int get_id();
    int get_uid();
    string vertex_name;
    int get_dest_port(int dest_uid);
    vector<int> arch_graph_distances;
    void create_arch_graph_distances();
    vector<int> neighbors_desc;
    vector<int> neighbors_uid;
    // UID to port mapping
    map<int, int> uid_to_port_map;
    map<int, int> port_to_uid_map;
    void insert_uid_to_port_map_entry(int uid, int port_id);
    int last_sent_port_id;
    int total_cores;
    void smart_stats();
private:
    // NeuraDispatcher ID
    // Just one NeuraDispatcher per NeuraChip
    int id;
    // Unique ID (Across entire NeuraChip)
    int uid;
    // Port Count (Usually set to 1 since we have one NeuraDispatcher per NeuraCore)
    int port_count;
    GlobalCounters *gcp;
    // Number of dispatches per cycle
    // Ideally, this should be equal to the number of NeuraCores
    // Dispatcher does not have to go round robin across all NeuraCores
    int dispatches_per_cycle;
    // Total number of instructions to dispatch
    long long total_dispatch_count;
    // Number of instructions dispatched so far
    long long instx_dispatched;
    long long interval_instx_dispatched;
    long long interval_id;
    long long busy_cycles;
    long long interval_busy_cycles;
    long long idle_cycles;
    long long interval_idle_cycles;
    string program_file;
    // End Simulation flag
    bool sim_end;
    // Name of the component
	string component_name;
    // Name of the component header
    string header_component_name;
	// Collection
	mongocxx::collection collection;
    // Collection header
    mongocxx::collection header_collection;
    // Set sim progress
    void set_sim_progress();
};



#endif // NEURA_DISPATCHER_H