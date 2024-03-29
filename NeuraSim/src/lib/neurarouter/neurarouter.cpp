// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "neurarouter.h"
#include "../debugger/debugger.h"
#include "../config.h"
#include "../memreq/memrequest.h"
#include "../memresp/memresponse.h"
#include "../instx/instx.h"
#include "../hashreq/hashrequest.h"
#include "../functions.h"
// #include "../hashresp/hashresponse.h"

using namespace std;

NeuraRouter::NeuraRouter()
{
}

NeuraRouter::NeuraRouter(GlobalCounters *gcp,
						 long long x_cood,
						 long long y_cood)
{
	this->custom_constructor(gcp, x_cood, y_cood);
}

void NeuraRouter::custom_constructor(GlobalCounters *gcp,
									 long long x_cood,
									 long long y_cood)
{
	this->gcp = gcp;
	this->id = this->gcp->generate_router_id();
	this->uid = this->gcp->generate_uid();
	this->gcp->set_router_id_array(this->id, this->uid);
	this->port_count = this->gcp->NEURA_ROUTER_PORT_COUNT;
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
	this->x_cood = x_cood;
	this->y_cood = y_cood;
	this->last_port_used = 0;
	this->total_neura_cores = this->gcp->NEURA_CORE_X_COUNT * this->gcp->NEURA_CORE_Y_COUNT;
	this->total_neura_mems = this->gcp->NEURA_MEM_X_COUNT * this->gcp->NEURA_MEM_Y_COUNT;
	this->total_writers = this->gcp->WRITER_COUNT;
	this->total_memory_controllers = 1;
	this->total_destinations = this->total_neura_cores +
							   this->total_neura_mems +
							   this->total_writers +
							   this->total_memory_controllers;
	this->total_routers = this->gcp->NEURA_ROUTER_X_COUNT * this->gcp->NEURA_ROUTER_Y_COUNT;
	this->routing_matrix = this->create_routing_matrix();
	this->component_name = "NeuraRouter_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
	// this->connected_components = new vector<int>;

	this->vertex_name = "R_" + to_string(this->x_cood) + "_" + to_string(this->y_cood);
	this->gcp->add_vertex_to_arch_graph(this->vertex_name, this->uid);
}

NeuraRouter::~NeuraRouter()
{
	delete[] this->in_port_array;
	delete[] this->out_port_array;
}

queue<InstX *> *NeuraRouter::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

queue<InstX *> *NeuraRouter::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

bool NeuraRouter::tick()
{
	if (this->processing_counter > 0)
	{
		this->processing_counter--;
		return (this->return_routine(SIM_END_FALSE, COMPUTED));
	}

	for (int i = 1; i <= this->gcp->NEURA_ROUTER_PORT_COUNT; i++)
	{
		int next_port = (this->last_port_used + i) % this->gcp->NEURA_ROUTER_PORT_COUNT;

		// Check if there is any packet in the input port
		if (this->in_port_array[next_port].empty())
		{
			continue;
		}
#ifdef TRACE_INSTX
		cout << "NeuraRouter:" << this->vertex_name << " has packet on port: " << next_port << endl;
#endif
		InstX *packet = this->in_port_array[next_port].front();
		int dest_uid = packet->dest_unit_uid_ptr[0];
		int dest_port = this->get_dest_port(dest_uid);
		if (dest_port == -1)
		{
			cout << "NeuraRouter WARN: " << this->id << " has no destination port for packet on port: " << next_port << endl;
			continue;
		}
		bool move_success = this->attempt_move_packet(next_port, dest_port);
		if (move_success)
		{
			this->last_port_used = next_port;
#ifdef TRACE_INSTX
			string port_direction = "";
			switch (dest_port)
			{
			case RTR_TOP_PORT:
				port_direction = "NORTH";
				break;
			case RTR_RIGHT_PORT:
				port_direction = "EAST";
				break;
			case RTR_BOTTOM_PORT:
				port_direction = "SOUTH";
				break;
			case RTR_LEFT_PORT:
				port_direction = "WEST";
				break;
			case RTR_TOP_RIGHT_PORT:
				port_direction = "NORTH-EAST";
				break;
			case RTR_BOTTOM_RIGHT_PORT:
				port_direction = "SOUTH-EAST";
				break;
			case RTR_BOTTOM_LEFT_PORT:
				port_direction = "SOUTH-WEST";
				break;
			case RTR_TOP_LEFT_PORT:
				port_direction = "NORTH-WEST";
				break;
			case RTR_EXTRA_PORT:
				port_direction = "EXTRA";
				break;
			default:
				port_direction = "UNKNOWN";
				break;
			}

			cout << "NeuraRouter: " << this->vertex_name << " has sent packet to " << port_direction << " port" << endl;
#endif // TRACE_INSTX
			return (this->return_routine(SIM_END_FALSE, COMPUTED));
		}
	}

	// If some ports are not empty, then increment the stalled cycles
	for (int i = 0; i < this->gcp->NEURA_ROUTER_PORT_COUNT; i++)
	{
		if (!this->in_port_array[i].empty())
		{
			return (this->return_routine(SIM_END_FALSE, STALLED));
		}
	}
	// If all the ports are empty, then increment the idle cycles
	return (this->return_routine(SIM_END_TRUE, STALLED));
}

// int NeuraRouter::get_dest_port(int port_in) {
// 	// FIXME: Temporary implementation
// 	InstX *instx = this->in_port_array[port_in].front();
// 	// int dest_port = this->routing_matrix[port_in][instx->dest_id];
// 	// return dest_port;
// 	return 0;
// }

// @param dest_uid: Destination Unit UID
// @return: Port ID to send data on
// @description: Finds the best port to send data using the best routing algorithm
int NeuraRouter::get_dest_port(int dest_uid)
{
#ifdef TRACE_INSTX
	if (dest_uid == -1)
	{
		cout << FATAL_ERROR << " NeuraRouter: " << this->vertex_name << " has no destination port for packet [-1]" << endl;
	}
	else
	{
		cout << "NeuraRouter: " << this->vertex_name << " is routing packet to: " << this->gcp->uid_to_vertex_name_map[dest_uid] << endl;
	}
#endif

	// Check if the destination unit is a neighbor
	if (find(this->neighbors_uid.begin(), this->neighbors_uid.end(), dest_uid) != this->neighbors_uid.end())
	{
		return (this->uid_to_port_map[dest_uid]);
	}

	int min_distance_port = -1;

	// min_distance_port = packet_routing(dest_uid, this->gcp, &this->neighbors_uid, &this->uid_to_port_map);
	min_distance_port = packet_routing(dest_uid, this->gcp, &this->neighbors_uid_only_routers, &this->uid_to_port_map);
	return (min_distance_port);
}

bool NeuraRouter::attempt_move_packet(int port_in, int port_out)
{
	if (this->out_port_array[port_out].size() >= this->gcp->NEURA_ROUTER_BUFFER_SIZE)
	{
		return false;
	}
	InstX *instx = this->in_port_array[port_in].front();
	this->out_port_array[port_out].push(instx);
	this->in_port_array[port_in].pop();
	return true;
}

bool NeuraRouter::return_routine(bool sim_end, bool computed)
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

void NeuraRouter::print()
{
	cout << "NeuraRouter ID: " << this->id << endl;
}

void NeuraRouter::statistics()
{
	cout << "************** " << GREEN_START << "Router ID: " << this->id << COLOR_RESET << " UID: " << this->uid << " [" << this->x_cood << "]"
		 << "[" << this->y_cood << "]"
		 << " ********" << endl;
	cout << "Idle: " << this->idle_cycles;
	cout << " Busy: " << this->busy_cycles << endl;
	cout << "Neighbors: " << this->neighbors_uid.size() << endl;
	cout << "Neighbors: ";
	for (auto neighbor_uid : this->neighbors_uid)
	{
		cout << this->gcp->uid_to_vertex_name_map[neighbor_uid] << " ";
	}
	cout << endl;
	print_conn_usage(RTR_TOP_LEFT_PORT);
	cout << "\t\t";
	print_conn_usage(RTR_TOP_PORT);
	cout << "\t\t";
	print_conn_usage(RTR_TOP_RIGHT_PORT);
	cout << endl;
	print_conn_usage(RTR_LEFT_PORT);
	cout << "\t\t";
	cout << GREEN_START << "RTR" << COLOR_RESET;
	cout << "\t\t";
	print_conn_usage(RTR_RIGHT_PORT);
	cout << endl;
	print_conn_usage(RTR_BOTTOM_LEFT_PORT);
	cout << "\t\t";
	print_conn_usage(RTR_BOTTOM_PORT);
	cout << "\t\t";
	print_conn_usage(RTR_BOTTOM_RIGHT_PORT);
	cout << endl;
	cout << "*********************************" << endl;
}

void NeuraRouter::smart_stats()
{
	// TODO: Pending implementation
}

void NeuraRouter::print_conn_usage(int port_id)
{
	cout << "<" << this->in_port_array[port_id].size();
	cout << "/" << this->out_port_array[port_id].size() << ">";
}

bool NeuraRouter::sanity_check()
{
	return true;
}

// Create a routing matrix of size (total_destinations x (total_ports) )
/*

Ports 0 0 0 0
UID
0	  N E S W
1	  S N W E
2     E W N S
3     W E S N
4     N S E W
*/
int **NeuraRouter::create_routing_matrix()
{
	if (this->total_routers != this->gcp->NEURA_ROUTER_X_COUNT * this->gcp->NEURA_ROUTER_Y_COUNT)
	{
		cout << "NeuraRouter: " << this->id << " has a mismatch in the total routers" << endl;
		exit(1);
	}
	int **routing_matrix = new int *[this->total_routers];
	for (int i = 0; i < this->total_routers; i++)
	{
		routing_matrix[i] = new int[this->total_routers - 1];
	}
	// Initialize the routing matrix
	for (int i = 0; i < this->total_routers; i++)
	{
		for (int j = 0; j < this->total_routers - 1; j++)
		{
			routing_matrix[i][j] = -1;
		}
	}

	return routing_matrix;
}

bool NeuraRouter::get_sim_end()
{
	return this->sim_end;
}

void NeuraRouter::post_sim_metrics()
{
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("x_cood", (long)this->x_cood),
			kvp("y_cood", (long)this->y_cood),
			kvp("busy_cycles", (long)this->busy_cycles),
			kvp("idle_cycles", (long)this->idle_cycles)));
	}
	return;
}

void NeuraRouter::interval_metrics()
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

int NeuraRouter::get_id()
{
	return this->id;
}

int NeuraRouter::get_uid()
{
	return this->uid;
}

void NeuraRouter::create_arch_graph_distances()
{
	arch_distancing(this->gcp,
					this->arch_graph_distances,
					this->vertex_name,
					this->uid,
					this->neighbors_desc,
					this->neighbors_uid);
	// Replicate neighbors-uid into neighbors_uid_only_routers by removing the non-router neighbors
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
	// boost::dijkstra_shortest_paths(	this->gcp->arch_graph,
	// 								this->gcp->vertex_name_to_desc_map[this->vertex_name],
	// 								boost::distance_map(&this->arch_graph_distances[0]));

	// this->gcp->uid_to_arch_graph_distances_ptr_map[this->uid] = &this->arch_graph_distances;

	// // Source vertex descriptor
	// auto src_unit_vertex_desc = this->gcp->vertex_uid_to_desc_map[this->uid];

	// // Find the neighbors of this vertex
	// auto neighbors_iter_pair = boost::adjacent_vertices(src_unit_vertex_desc, this->gcp->arch_graph);
	// for (auto iter = neighbors_iter_pair.first; iter != neighbors_iter_pair.second; iter++) {
	// 	this->neighbors_desc.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// }

	// // Create neighbor_uid vector
	// for (auto iter = this->neighbors_desc.begin(); iter != this->neighbors_desc.end(); iter++) {
	// 	this->neighbors_uid.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
	// }
}

string NeuraRouter::get_vertex_name()
{
	return (this->vertex_name);
}