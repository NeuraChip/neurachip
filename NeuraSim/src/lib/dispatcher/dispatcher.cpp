// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "dispatcher.h"
#include "../debugger/debugger.h"
#include "../hashreq/hashrequest.h"
#include <sstream>
#include <iostream>
#include "../functions.h"

using namespace std;

// Default constructor is needed because NeuraTile creates empty objects array with new keyword
// and then calls custom_constructor on each object
NeuraDispatcher::NeuraDispatcher()
{
}

NeuraDispatcher::NeuraDispatcher(GlobalCounters *gcp,
                                 string program_file,
                                 int port_count)
{
    this->custom_constructor(gcp, program_file, port_count);
}

void NeuraDispatcher::custom_constructor(GlobalCounters *gcp,
                                         string program_file,
                                         int port_count)
{
    this->program_file = program_file;
    this->gcp = gcp;
    this->port_count = port_count;
    this->id = gcp->generate_dispatcher_id();
    this->uid = gcp->generate_uid();
    this->gcp->set_dispatcher_id_array(this->id, this->uid);
    this->in_port_array = new queue<InstX *>[this->port_count];
    this->out_port_array = new queue<InstX *>[this->port_count];
    this->load_program();
    this->instx_dispatched = 0;
    this->total_dispatch_count = this->gcp->DISPATCH_INF_INSTX;
    this->sim_end = false;
    this->component_name = "NeuraDispatcher";
    this->header_component_name = "H_" + this->component_name;
    this->collection = gcp->mongo->get_component_collection(this->component_name);
    this->header_collection = gcp->mongo->get_component_collection(this->header_component_name);
    this->interval_instx_dispatched = 0;
    this->interval_id = 0;
    this->busy_cycles = 0;
    this->idle_cycles = 0;
    this->interval_busy_cycles = 0;
    this->interval_idle_cycles = 0;
    this->vertex_name = "D_0"; // + to_string(this->x_cood) + "_" + to_string(this->y_cood);
    this->gcp->add_vertex_to_arch_graph(this->vertex_name, this->uid);
    this->last_sent_port_id = -1;
    this->total_cores = this->gcp->get_total_cores();
}

NeuraDispatcher::~NeuraDispatcher()
{
    // Close program file stream
    this->program_file_stream.close();
}

int NeuraDispatcher::get_id()
{
    return this->id;
}

int NeuraDispatcher::get_uid()
{
    return this->uid;
}

void NeuraDispatcher::load_program()
{
    // Load program into NeuraDispatcher
    if (this->program_file != "")
    {
        // Open program file for reading
        this->program_file_stream.open(this->program_file, ios::in);
        // Check if file is open
        if (!this->program_file_stream.is_open())
        {
            cout << FATAL_ERROR << "NeuraDispatcher: Program file not found" << endl;
        }
    }
    else
    {
        cout << FATAL_ERROR << "NeuraDispatcher: Program file not set" << endl;
    }
}

void NeuraDispatcher::set_program_file(string program_file)
{
    this->program_file = program_file;
}

// NOTE: Will have to tick 16 times if you have 16 Cores
//
bool NeuraDispatcher::tick()
{
    // Tick number of times equal to number of cores
    // cout << "Dispatcher[" << this->id <<  "]: Ticking" << endl;
    bool dispatched_instruction = false;

    for (auto partial_tick = 0; partial_tick < this->total_cores; partial_tick++)
    {
        if (this->total_dispatch_count != this->gcp->DISPATCH_INF_INSTX)
        {
            if (this->instx_dispatched >= this->total_dispatch_count)
            {
                if (dispatched_instruction)
                {
                    this->sim_end = false;
                }
                else
                {
                    this->sim_end = true;
                }
                // this->sim_end = true;
                // FIXME: This is a hack to make sure the simulator never ends
                // this->sim_end = false;
                return this->sim_end;
            }
        }
        this->set_sim_progress();

        // Send multiple instructions to same NeuraCore

        for (auto multi_dispatch_iterator = 0; multi_dispatch_iterator < MULTI_DISPATCH_COUNT; multi_dispatch_iterator++)
        {
            // Read a single line from program file
            string line, word;
            getline(this->program_file_stream, line);
            stringstream prog_ss(line);
            // cout << "Line: " << line << endl;
            // Read each word in the line
            // Get Instruction Name
            getline(prog_ss, word, ',');
            // cout << "Name: " << word << endl;
            if (word != "MULT_HASH_ACC")
            {
                // cout << "Reached end of program: " << word << endl;
                return (this->return_routine(SIM_END_TRUE, STALLED));
            }
            // Get A_DATA_ADDR
            getline(prog_ss, word, ',');
            long long a_data_addr = stoll(word);
            // Get B_DATA_ADDR
            getline(prog_ss, word, ',');
            long long b_data_addr = stoll(word);
            // Get B_COL_ID_ADDR
            getline(prog_ss, word, ',');
            long long b_col_id_addr = stoll(word);
            // Get COUNT
            getline(prog_ss, word, ',');
            long long count = stoll(word);
            // Get TABLE_ID
            getline(prog_ss, word, ',');
            long long table_id = stoll(word);
            // Construct HashRequest
            int b_col_id = this->gcp->address_data_map_ptr->at(b_col_id_addr);
            long long dest_unit_uid = this->hash_req_mapper(b_col_id);
            HashRequest *hr = new HashRequest(this->gcp,
                                              this->uid,
                                              dest_unit_uid,
                                              a_data_addr,
                                              b_data_addr,
                                              b_col_id_addr,
                                              count,
                                              table_id);
            // Push HashRequest to NeuraCore
            this->send_hash_req(hr, dest_unit_uid);
            this->instx_dispatched++;
            this->interval_instx_dispatched++;
        }
        dispatched_instruction = true;

#ifdef TRACE_INSTX
        cout << "Dispatcher[" << this->id << "]: Dispatched HashRequest: " << hr->id << endl;
#endif
    }

    return (this->return_routine(SIM_END_FALSE, COMPUTED));
}

bool NeuraDispatcher::sanity_check()
{
    if (this->program_file == "")
    {
        cout << "NeuraDispatcher: Program file not set" << endl;
        return false;
    }

    return true;
}

long long NeuraDispatcher::hash_req_mapper(int b_col_id)
{
    // TODO: Implement hash request mapper
    // Map hash request to a NeuraCore
    // For now, just use round robin mapping
    // this->last_sent_port_id++;
    // if (this->last_sent_port_id >= this->port_count)
    // {
    //     this->last_sent_port_id = 0;
    // }
    // return this->last_sent_port_id;

    int core_id = b_col_id % this->total_cores;
    // int dest_port = this->gcp->get_core_id_array()[core_id];

    // Since core_id == Core's uid
    return core_id;
}

void NeuraDispatcher::send_hash_req(HashRequest *hr, int dest_core_uid)
{
    int dest_port = this->get_dest_port(dest_core_uid);
    // Send HashRequest to NeuraCore
    this->out_port_array[dest_port].push(hr);
}

// Get a reference to an input port
// @param port_id: Port ID
// @return: Reference to input port
queue<InstX *> *NeuraDispatcher::get_in_port_ref(int port_id)
{
    return &this->in_port_array[port_id];
}

// Get a reference to an output port
// @param port_id: Port ID
// @return: Reference to output port
queue<InstX *> *NeuraDispatcher::get_out_port_ref(int port_id)
{
    return &this->out_port_array[port_id];
}

void NeuraDispatcher::statistics()
{
    // Print statistics
    cout << "***************" << YELLOW_START "NeuraDispatcher ID: " << this->id << " UID: " << this->uid << COLOR_RESET << "***************" << endl;
    cout << RED_START << "Stalls" COLOR_RESET << ": " << this->idle_cycles << "\t";
    cout << GREEN_START << "Compute" << COLOR_RESET << ": " << this->busy_cycles << endl;
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

void NeuraDispatcher::smart_stats()
{
    // TODO: Print stats when dispatcher has not completed its job

    // if
    // this->statistics();
}

void NeuraDispatcher::set_dispatch_count(long long count)
{
    this->total_dispatch_count = count;
}

bool NeuraDispatcher::get_sim_end()
{
    return this->sim_end;
}

void NeuraDispatcher::post_sim_metrics()
{
    if (MONGO_FLAG)
    {
        this->header_collection.insert_one(make_document(
            kvp("name", "post_sim_metrics"),
            kvp("instx_dispatched", (long)this->instx_dispatched)));
        return;
    }
}

void NeuraDispatcher::interval_metrics()
{
    if (MONGO_FLAG)
    {
        this->collection.insert_one(make_document(
            kvp("name", "interval_metrics"),
            kvp("interval_id", (long)this->interval_id),
            kvp("interval_instx_dispatched", (long)this->interval_instx_dispatched),
            kvp("interval_busy_cycles", (long)this->interval_busy_cycles),
            kvp("interval_idle_cycles", (long)this->interval_idle_cycles)));
    }
    this->interval_instx_dispatched = 0;
    this->interval_busy_cycles = 0;
    this->interval_idle_cycles = 0;
    this->interval_id++;
}

bool NeuraDispatcher::return_routine(bool sim_end, bool computed)
{
    this->sim_end = sim_end;
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

void NeuraDispatcher::set_sim_progress()
{
    // FIXME: Fetch total instructions from binary
    int total_instx = 192885;
    gcp->set_sim_progress((int)(((float)this->instx_dispatched / (float)total_instx) * 100.0));
}

void NeuraDispatcher::insert_uid_to_port_map_entry(int uid, int port)
{
    this->uid_to_port_map[uid] = port;
}

int NeuraDispatcher::get_dest_port(int dest_uid)
{
    return this->uid_to_port_map[dest_uid];
}

void NeuraDispatcher::create_arch_graph_distances()
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
    //                                this->gcp->vertex_name_to_desc_map[this->vertex_name],
    //                                boost::distance_map(&this->arch_graph_distances[0]));

    // this->gcp->uid_to_arch_graph_distances_ptr_map[this->uid] = &this->arch_graph_distances;

    // // Source vertex descriptor
    // auto src_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[this->uid];

    // // Find the neighbors of this vertex
    // auto neighbors_iter_pair = boost::adjacent_vertices(src_unit_vertex_desc, this->gcp->arch_graph);
    // for (auto iter = neighbors_iter_pair.first; iter != neighbors_iter_pair.second; iter++)
    // {
    //     this->neighbors_desc.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
    // }

    // // Create neighbor_uid vector
    // for (auto iter = this->neighbors_desc.begin(); iter != this->neighbors_desc.end(); iter++)
    // {
    //     this->neighbors_uid.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
    // }
}

string NeuraDispatcher::get_vertex_name()
{
    return (this->vertex_name);
}