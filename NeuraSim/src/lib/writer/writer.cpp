// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "writer.h"
#include "../debugger/debugger.h"
#include "../functions.h"

using namespace std;

// Constructor
Writer::Writer()
{
}

Writer::Writer(GlobalCounters *gcp, long long x_cood)
{
	this->custom_constructor(gcp, x_cood);
}

void Writer::custom_constructor(GlobalCounters *gcp,
								long long x_cood)
{
	this->gcp = gcp;
	this->id = this->gcp->generate_writer_id();
	this->uid = this->gcp->generate_uid();
	this->gcp->set_writer_id_array(this->id, this->uid);
	this->x_cood = x_cood;
	this->port_count = this->gcp->WRITER_PORT_COUNT;
	this->in_port_array = new queue<InstX *>[this->port_count];
	this->out_port_array = new queue<InstX *>[this->port_count];
	this->sim_end = false;
	this->processing_counter = 0;
	this->idle_cycles = 0;
	this->busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_id = 0;
	this->last_processed_port = 0;
	this->component_name = "Writer_" + to_string(this->x_cood);
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
	this->create_C_row_ptr_array();
	this->vertex_name = "W_" + to_string(this->x_cood);
	this->gcp->add_vertex_to_arch_graph(this->vertex_name, this->uid);
}

Writer::~Writer()
{
	delete[] this->in_port_array;
	delete[] this->out_port_array;
}

int Writer::get_id()
{
	return this->id;
}

int Writer::get_uid()
{
	return this->uid;
}

bool Writer::tick()
{
	if (this->processing_counter > 0)
	{
		this->processing_counter--;
		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	}

	// Get the next response
	HashResponse *hash_resp = this->get_next_response();
	if (hash_resp == NULL)
	{
		this->idle_cycles++;
		this->interval_idle_cycles++;
		return (this->return_routine(SIM_END_TRUE, STALLED));
	}
	else
	{
#ifndef WRITER_DISABLE_HBM_WRITE
		// cout << "Writer " << this->id << " got a response" << endl;
		long long write_address = this->generate_write_address(hash_resp);
		// cout << "Writer " << this->id << " generated write address: " << write_address << endl;
		MemRequest *write_req = this->generate_write_request(hash_resp, write_address);
		this->send_write_request(write_req);
#endif // WRITER_DISABLE_HBM_WRITE
		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	}

	return (this->return_routine(SIM_END_TRUE, STALLED));
}

void Writer::print()
{
	cout << "Writer ID: " << this->id << endl;
}

void Writer::statistics()
{
	// Print statistics
	cout << "***************" << GREEN_START "Writer ID: " << this->id << " UID: " << this->uid << COLOR_RESET << "***************" << endl;
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

void Writer::smart_stats()
{
	// TODO: Pending implementation
}

queue<InstX *> *Writer::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

queue<InstX *> *Writer::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

bool Writer::sanity_check()
{
	// Check if all the ports are empty
	for (int i = 0; i < this->port_count; i++)
	{
		if (!this->in_port_array[i].empty())
		{
			cout << SANITY_CHECK << "Writer ";
			cout << this->id << " has non-empty in port ";
			cout << i << endl;
			return false;
		}
		if (!this->out_port_array[i].empty())
		{
			cout << SANITY_CHECK << "Writer ";
			cout << this->id << " has non-empty out port ";
			cout << i << endl;
			return false;
		}
	}
	return true;
}

bool Writer::get_sim_end()
{
	return this->sim_end;
}

void Writer::post_sim_metrics()
{
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("busy_cycles", (long)this->busy_cycles),
			kvp("idle_cycles", (long)this->idle_cycles)));
	}
	return;
}

void Writer::interval_metrics()
{
	if (MONGO_FLAG)
	{
		this->collection.insert_one(make_document(
			kvp("name", "interval_metrics"),
			kvp("interval_id", (long)this->interval_id),
			kvp("interval_busy_cycles", (long)this->interval_busy_cycles),
			kvp("interval_idle_cycles", (long)this->interval_idle_cycles)));
	}
	this->interval_id++;
	this->interval_busy_cycles = 0;
	this->interval_idle_cycles = 0;
	return;
}

bool Writer::return_routine(bool sim_end, bool computed)
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

// Gets the next instruction from all ports OR NULL if no instruction is available
// @return: Pointer to the next instruction
// @return: NULL if no instruction is available
// @param: None
HashResponse *Writer::get_next_response()
{
	for (int i = 0; i < this->port_count; i++)
	{
		int port_id = (this->last_processed_port + i) % this->port_count;
		if (this->in_port_array[port_id].size() > 0)
		{
			InstX *instx = this->in_port_array[port_id].front();
			this->in_port_array[port_id].pop();
			if (instx->get_type() != INSTX_TYPE_HASH_RESP)
			{
				cout << FATAL_ERROR << "Writer::get_next_response() received ";
				cout << "non-hash-response instruction" << endl;
				cout << "Instruction Type: " << instx->get_type() << endl;
				exit(1);
			}
			this->last_processed_port = port_id;
			// Instruction fetch by Writer consumes 1 cycle
			this->processing_counter += 1;
			// Convert to HashResponse
			HashResponse *hash_resp = (HashResponse *)instx;
			return hash_resp;
		}
	}
	// No instruction available
	return NULL;
}

void Writer::create_C_row_ptr_array()
{
	this->load_C_files();

	// Read a single line from program file
	string line, word;

	while (this->C_header_file.good())
	{
		getline(this->C_header_file, line);
		if (line == "")
		{
			continue;
		}
		// Create a string stream from line
		stringstream s(line);
		getline(s, word, ',');
		if (word == "x_dim")
		{
			getline(s, word, ',');
			this->C_row_ptr_array_size = (stoll(word) + 1); // +1 because row ptr array has one more element than number of rows
		}
		if (word == "data_address")
		{
			getline(s, word, ',');
			this->C_data_base_address = stoll(word);
		}
	}
	// Print C Row Ptr Array Size
	// cout << "C Row Ptr Array Size: " << this->C_row_ptr_array_size << endl;
	// cout << "C Data Base Address: " << this->C_data_base_address << endl;

	// Allocate memory for C_row_ptr_array
	this->C_row_ptr_array = new long long[this->C_row_ptr_array_size];
	// Allocate memory for atomic_offsets_array
	this->atomic_offsets_array = new long long[this->C_row_ptr_array_size];
	// Initialize atomic_offsets_array
	for (int i = 0; i < this->C_row_ptr_array_size; i++)
	{
		this->atomic_offsets_array[i] = 0;
	}

	// Read a single line from program file
	int line_number = 0;
	while (this->C_row_ptr_file.good())
	{

		getline(this->C_row_ptr_file, line);
		// Create a string stream from line
		stringstream s(line);
		getline(s, word, '\n');
		if (word == "")
		{
			continue;
		}
		if (line_number == this->C_row_ptr_array_size)
		{
			cout << "WARNING: C Row Ptr Array consists more rows than X dimension of C matrix: " << line_number << endl;
			// break;
		}

		uint64_t row_ptr = stoll(word);
		this->C_row_ptr_array[line_number] = row_ptr;
		line_number++;
	}

	// Print first all elements of C_row_ptr_array
	// cout << "C_row_ptr_array: ";
	// for (int i = 0; i < this->C_row_ptr_array_size; i++) {
	// 	cout << this->C_row_ptr_array[i] << " ";
	// }
	// cout << endl;

	this->close_C_files();
}

void Writer::load_C_files()
{
	// Load C_row_ptr_file
	if (this->gcp->get_program_data_folder() == "")
	{
		cout << FATAL_ERROR << "Writer::load_C_row_ptr_file() ";
		cout << "program_data_folder is not set" << endl;
		// exit(1);
	}
	string C_row_ptr_file = this->gcp->get_program_data_folder() + "/mat_C/row_ptr.txt";
	this->C_row_ptr_file.open(C_row_ptr_file, ios::in);
	// Check if file is open
	if (!this->C_row_ptr_file.is_open())
	{
		cout << FATAL_ERROR << "Writer::load_C_row_ptr_file() ";
		cout << "could not open file " << C_row_ptr_file << endl;
		// exit(1);
	}
	// Load C_header_file
	if (this->gcp->get_program_data_folder() == "")
	{
		cout << FATAL_ERROR << "Writer::load_C_header_file() ";
		cout << "program_data_folder is not set" << endl;
		// exit(1);
	}
	string C_header_file = this->gcp->get_program_data_folder() + "/mat_C/header.csv";
	this->C_header_file.open(C_header_file, ios::in);
	// Check if file is open
	if (!this->C_header_file.is_open())
	{
		cout << FATAL_ERROR << "Writer::load_C_header_file() ";
		cout << "could not open file " << C_header_file << endl;
		// exit(1);
	}
}

void Writer::close_C_files()
{
	this->C_row_ptr_file.close();
	this->C_header_file.close();
}

long long Writer::generate_write_address(HashResponse *hash_resp)
{
	long long write_address;
	// Get row number
	long long row_number = hash_resp->get_table_id();
	write_address = this->C_data_base_address +
					(this->C_row_ptr_array[row_number] * 4) +
					(this->atomic_offsets_array[row_number] * 4);
	// Increment atomic offset
	this->atomic_offsets_array[row_number] += 1;
	return write_address;
}

MemRequest *Writer::generate_write_request(HashResponse *hash_resp,
										   long long write_address)
{
	// MC UID
	int mc_uid = this->gcp->get_mc_id_array()[0];
	// Create a new MemRequest
	MemRequest *mem_req = new MemRequest(this->gcp,
										 this->id,
										 mc_uid,
										 MEM_REQ_WRITE,
										 -1,
										 write_address,
										 4,
										 hash_resp->get_mult_result(),
										 hash_resp->get_src_hash_req());
	return mem_req;
}

void Writer::send_write_request(MemRequest *write_req)
{
// Send write request to MC
// this->out_port_array[this->get_port_from_uid(write_req->get_dest_unit_uid())].push(write_req);
#ifdef TRACE_INSTX
	cout << "Writer sending write request: " << this->gcp->uid_to_vertex_name_map[write_req->get_dest_unit_uid()] << endl;
#endif
	int dest_port_id = this->get_dest_port(write_req->get_dest_unit_uid());
	this->out_port_array[dest_port_id].push(write_req);
}

// int Writer::get_port_from_uid(int uid)
// {
// 	// FIXME: This is a temporary solution
// 	const int core_uid = this->gcp->get_core_id_array()[0];
// 	const int mem_uid = this->gcp->get_mem_id_array()[0];
// 	const int router_uid = this->gcp->get_router_id_array()[0];
// 	const int dispatcher_uid = this->gcp->get_dispatcher_id_array()[0];
// 	const int writer_uid = this->gcp->get_writer_id_array()[0];
// 	const int mc_uid = this->gcp->get_mc_id_array()[0];
// 	if (uid == core_uid)
// 	{
// 		cout << FATAL_ERROR << "Core sending data to core" << endl;
// 		return -1;
// 	}
// 	else if (uid == mem_uid)
// 	{
// 		return 0;
// 	}
// 	else if (uid == router_uid)
// 	{
// 		// cout << "Sending to Router" << endl;
// 		cout << "Pending Implementation" << endl;
// 		return -1;
// 	}
// 	else if (uid == dispatcher_uid)
// 	{
// 		cout << "Pending Implementation" << endl;
// 		return -1;
// 	}
// 	else if (uid == writer_uid)
// 	{
// 		cout << FATAL_ERROR << "Writer sending data to Writer" << endl;
// 		return -1;
// 	}
// 	else if (uid == mc_uid)
// 	{
// 		return 1;
// 	}
// 	else
// 	{
// 		cout << FATAL_ERROR << "Invalid UID: " << uid << endl;
// 		return -1;
// 	}
// }

// @param dest_uid: Destination Unit UID
// @return: Port ID to send data on
// @description: Finds the best port to send data using the best routing algorithm
int Writer::get_dest_port(int dest_uid)
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

void Writer::create_arch_graph_distances()
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

string Writer::get_vertex_name()
{
	return (this->vertex_name);
}