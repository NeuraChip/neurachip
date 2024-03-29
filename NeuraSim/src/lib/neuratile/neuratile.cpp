// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "neuratile.h"
#include "../neuracore/neuracore.h"
#include "../debugger/debugger.h"
#include "../config.h"
#include "../neuramem/neuramem.h"
#include "../functions.h"
#include "../connector/conn.h"
#include "../dispatcher/dispatcher.h"
#include "../neurarouter/neurarouter.h"
#include "../mc/memorycontroller.h"
#include "../writer/writer.h"

using namespace std;

// Constructor
// @param id: NeuraTile ID
// @param gcp: Global Counters Pointer
// @param x: X Coordinate
// @param y: Y Coordinate
// @param port_count: Number of Ports
NeuraTile::NeuraTile(GlobalCounters *gcp, int x, int y, int port_count)
{
	this->custom_constructor(gcp, x, y, port_count);
}

NeuraTile::NeuraTile()
{
}

// Custom Constructor
// @param id: NeuraTile ID
// @param gcp: Global Counters Pointer
// @param x: X Coordinate
// @param y: Y Coordinate
// @param port_count: Number of Ports
void NeuraTile::custom_constructor(GlobalCounters *gcp, int x, int y, int port_count)
{
	// cout << "NeuraTile::custom_constructor()" << endl;
	this->gcp = gcp;
	this->id = this->gcp->generate_tile_id();
	// this->uid = this->gcp->generate_uid();
	// this->gcp->set_tile_id_array(this->id, this->uid);
	this->gcp->set_tile_id_array(this->id, -1);
	this->x_cood = x;
	this->y_cood = y;
	this->core_x_count = this->gcp->NEURA_CORE_X_COUNT;
	this->core_y_count = this->gcp->NEURA_CORE_Y_COUNT;
	this->mem_x_count = this->gcp->NEURA_MEM_X_COUNT;
	this->mem_y_count = this->gcp->NEURA_MEM_Y_COUNT;
	this->mem_controller_count = this->gcp->MC_COUNT;
	this->router_x_count = this->gcp->NEURA_ROUTER_X_COUNT;
	this->router_y_count = this->gcp->NEURA_ROUTER_Y_COUNT;
	this->port_count = this->gcp->NEURA_TILE_PORT_COUNT;
	this->writer_count = this->gcp->WRITER_COUNT;
	this->in_port_array = new queue<InstX *>[port_count];
	this->out_port_array = new queue<InstX *>[port_count];
	this->sim_end = false;
	this->program_file = this->gcp->get_program_file();
	this->busy_cycles = 0;
	this->idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_id = 0;
	this->component_name = "NeuraTile";
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
	this->vertex_name = "T_" + to_string(this->id);
	this->create_unit_components();
	this->component_sim_end_array = new bool[COMPONENT_COUNT];

	// Connect 1 core to 1 memory Port 0 <-> Port 0
	// this->conn_array.push_back(create_link(this->gcp, &this->core_array[0][0], 0, &this->mem_array[0][0], 0));
	// Connect 1 core to 1 router Port 1 <-> Port 1
	// this->conn_array.push_back(create_link(this->gcp, &this->core_array[0][0], 1, &this->router_array[0][0], 1));

	// Connect dispatcher to cores
	// FIXME: Implementation pending!
	// for (auto i = 0; i < this->core_x_count; i++)
	// {
	// 	for (auto j = 0; j < this->core_y_count; j++)
	// 	{
	// 		this->conn_array.push_back(create_link(this->gcp, this->dispatcher, this->core_array[i][j].get_id(), &this->core_array[i][j], this->gcp->INSTX_PORT_INDEX));
	// 	}
	// }

	this->generate_cgra_mesh_topology();

	// Connect Units
	// FIXME: Implementation pending!
	// this->connect_units("C_0_0", 0, "M_0_0", 1);

	// Connect NeuraMem Port 1 ro Memory Controller Port 0
	// this->conn_array.push_back(create_link(this->gcp, &this->mem_array[0][0], 1, this->mem_controller, 0));

	// Connect NeuraMem Port 2 to Writer Port 0
	// this->conn_array.push_back(create_link(this->gcp, &this->mem_array[0][0], 2, &this->writer_array[0], 0));

	// Connect Writer Port 1 to Memory Controller Port 1
	// this->conn_array.push_back(create_link(this->gcp, &this->writer_array[0], 1, this->mem_controller, 1));

	// Connect Core Port 2 to Writer Port 2
	// this->conn_array.push_back(create_link(this->gcp, &this->core_array[0][0], 2, &this->writer_array[0], 2));

	this->create_arch_graph_distances_for_all_units();

	// this->gcp->visualize_arch_graph();

	// this->print_unit_distances();

	// This has to be called after all connections are created
	this->enable_max_buffer_sizes_computation();
}

void NeuraTile::create_unit_components()
{
	// Create a 2D core array
	this->core_array = new NeuraCore *[this->core_x_count];
	for (int i = 0; i < this->core_x_count; i++)
	{
		this->core_array[i] = new NeuraCore[this->core_y_count];
		for (int j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].custom_constructor(this->gcp, i, j, this->gcp->NEURA_CORE_PORT_COUNT);
		}
	}

	// Create 2D array of memory
	this->mem_array = new NeuraMem *[this->mem_x_count];
	for (int i = 0; i < this->mem_x_count; i++)
	{
		this->mem_array[i] = new NeuraMem[this->mem_y_count];
		for (int j = 0; j < this->mem_y_count; j++)
		{
			this->mem_array[i][j].custom_constructor(this->gcp, i, j, this->gcp->NEURA_MEM_PORT_COUNT);
		}
	}

	// Create 2D array of Routers
	this->router_array = new NeuraRouter *[this->router_x_count];
	for (int i = 0; i < this->router_x_count; i++)
	{
		this->router_array[i] = new NeuraRouter[this->router_y_count];
		for (int j = 0; j < this->router_y_count; j++)
		{
			this->router_array[i][j].custom_constructor(this->gcp, i, j);
		}
	}

	// Create 1D array of Writers
	this->writer_array = new Writer[this->writer_count];
	for (int i = 0; i < this->writer_count; i++)
	{
		this->writer_array[i].custom_constructor(this->gcp, i);
	}

	// Create 1D Memory Controller array
	this->mem_controller_array = new MemoryController[this->mem_controller_count];
	// this->mem_controller = new MemoryController(this->gcp);
	for (int i = 0; i < this->mem_controller_count; i++)
	{
		this->mem_controller_array[i].custom_constructor(this->gcp, i);
	}

	// Create a dispatcher // TODO: Create a 2D array of Dispatchers
	// this->dispatcher_array = new NeuraDispatcher *[this->core_x_count];
	// for (int i = 0; i < this->core_x_count; i++)
	// {
	// 	this->dispatcher_array[i] = new NeuraDispatcher[this->core_y_count];
	// 	for (int j = 0; j < this->core_y_count; j++)
	// 	{
	// 		// FIXME: Cannot use the same program file for all dispatchers
	// 		// Use a different program file for each dispatcher
	// 		this->dispatcher_array[i][j].custom_constructor(this->gcp, this->program_file, this->gcp->DISPATCHER_COUNT,  i, j);
	// 		// FIXME: Need to configure this to dispatch just one row of instructions or set number of rows.
	// 		this->dispatcher_array[i][j].set_dispatch_count(this->gcp->DISPATCH_INSTX_COUNT);
	// 	}
	// }
	// this->dispatcher = new NeuraDispatcher();

	int total_core_count = this->core_x_count * this->core_y_count;
	this->dispatcher.custom_constructor(this->gcp, this->program_file, total_core_count);
	// For debugging, just dispatch 1 instruction
	this->dispatcher.set_dispatch_count(this->gcp->DISPATCH_INSTX_COUNT);
}

void NeuraTile::create_arch_graph_distances_for_all_units()
{
	// cout << "Creating arch graph distances for all units" << endl;

	// Create arch graph distances for the 2D core array
	for (int i = 0; i < this->core_x_count; i++)
	{
		for (int j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].create_arch_graph_distances();
		}
	}

	// Create arch graph distances for the 2D memory array
	for (int i = 0; i < this->mem_x_count; i++)
	{
		for (int j = 0; j < this->mem_y_count; j++)
		{
			this->mem_array[i][j].create_arch_graph_distances();
		}
	}

	// Create arch graph distances for the 2D router array
	for (int i = 0; i < this->router_x_count; i++)
	{
		for (int j = 0; j < this->router_y_count; j++)
		{
			this->router_array[i][j].create_arch_graph_distances();
		}
	}

	// Create arch graph distances for the 1D writer array
	for (int i = 0; i < this->writer_count; i++)
	{
		this->writer_array[i].create_arch_graph_distances();
	}

	// Create arch graph distances for the 1D memory controller array
	for (int i = 0; i < this->mem_controller_count; i++)
	{
		this->mem_controller_array[i].create_arch_graph_distances();
	}

	// Create arch graph distances for dispatcher
	this->dispatcher.create_arch_graph_distances();
}

void NeuraTile::print_unit_distances()
{
	cout << "Arch Distances" << endl;

	// Create arch graph distances for the 2D core array
	for (int i = 0; i < this->core_x_count; i++)
	{
		for (int j = 0; j < this->core_y_count; j++)
		{
			cout << "Core[" << i << "][" << j << "]: ";
			for (auto k = 0; k < this->core_array[i][j].arch_graph_distances.size(); k++)
			{
				cout << k << ":" << this->core_array[i][j].arch_graph_distances[k] << " ";
			}
			cout << endl;
		}
	}

	// Create arch graph distances for the 2D memory array
	for (int i = 0; i < this->mem_x_count; i++)
	{
		for (int j = 0; j < this->mem_y_count; j++)
		{
			cout << "Mem[" << i << "][" << j << "]: ";
			for (auto k = 0; k < this->mem_array[i][j].arch_graph_distances.size(); k++)
			{
				cout << k << ":" << this->mem_array[i][j].arch_graph_distances[k] << " ";
			}
			cout << endl;
		}
	}

	// Create arch graph distances for the 2D router array
	for (int i = 0; i < this->router_x_count; i++)
	{
		for (int j = 0; j < this->router_y_count; j++)
		{
			cout << "Router[" << i << "][" << j << "]: ";
			for (auto k = 0; k < this->router_array[i][j].arch_graph_distances.size(); k++)
			{
				cout << k << ":" << this->router_array[i][j].arch_graph_distances[k] << " ";
			}
			cout << endl;
		}
	}

	// Create arch graph distances for the 1D writer array
	for (int i = 0; i < this->writer_count; i++)
	{
		cout << "Writer[" << i << "]: ";
		for (auto k = 0; k < this->writer_array[i].arch_graph_distances.size(); k++)
		{
			cout << k << ":" << this->writer_array[i].arch_graph_distances[k] << " ";
		}
		cout << endl;
	}

	// Create arch graph distances for the 1D memory controller array
	for (int i = 0; i < this->mem_controller_count; i++)
	{
		cout << "MemController[" << i << "]: ";
		for (auto k = 0; k < this->mem_controller_array[i].arch_graph_distances.size(); k++)
		{
			cout << k << ":" << this->mem_controller_array[i].arch_graph_distances[k] << " ";
		}
		cout << endl;
	}

	// Create arch graph distances for the 1D dispatcher array
	cout << "Dispatcher: ";
	for (auto k = 0; k < this->dispatcher.arch_graph_distances.size(); k++)
	{
		cout << k << ":" << this->dispatcher.arch_graph_distances[k] << " ";
	}
	cout << endl;
}

// Unit Connector
void NeuraTile::connect_units(string unit1, int port1, string unit2, int port2)
{
	// cout << "Connecting " << unit1 << " to " << unit2 << endl;
	// Unit 1
	char unit1_type = unit1[0];
	int unit1_x = stoi(unit1.substr(2, unit1.find("_", 2)));
	int unit1_y = -1;
	if (unit1_type == 'C' || unit1_type == 'M' || unit1_type == 'R')
	{ // Not including writer, mem controller, or dispatcher
		unit1_y = stoi(unit1.substr(unit1.find("_", 2) + 1, unit1.length()));
	}
	// Unit 2
	char unit2_type = unit2[0];
	int unit2_x = stoi(unit2.substr(2, unit2.find("_", 2)));
	int unit2_y = -1;
	if (unit2_type == 'C' || unit2_type == 'M' || unit2_type == 'R')
	{ // Not including writer, mem controller, or dispatcher
		unit2_y = stoi(unit2.substr(unit2.find("_", 2) + 1, unit2.length()));
	}

	// // C
	// NeuraCore *core_ptr;
	// // M
	// NeuraMem *mem_ptr;
	// // X
	// MemoryController *mem_controller_ptr;
	// // W
	// Writer *writer_ptr;
	// // R
	// NeuraRouter *router_ptr;
	// // D
	// NeuraDispatcher *dispatcher_ptr;

	// this->conn_array.push_back(create_link(this->gcp, &this->mem_array[0][0], 1, this->mem_controller_array[this->id]], 0));

	if (unit1_type == 'C')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->core_array[unit2_x][unit2_y], port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->core_array[unit2_x][unit2_x].get_uid()] = port1;
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->core_array[unit1_y][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->core_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->mem_controller_array[this->id], port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->mem_controller_array[this->id].uid_to_port_map[this->core_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->writer_array[unit2_x], port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->writer_array[unit2_x].uid_to_port_map[this->core_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, STANDARD_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->router_array[unit2_x][unit2_y], port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->core_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->core_array[unit1_x][unit1_y], port1, &this->dispatcher, port2));
			this->core_array[unit1_x][unit1_y].uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->core_array[unit1_x][unit1_y].get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port2] = this->core_array[unit1_x][unit1_y].get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else if (unit1_type == 'M')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->core_array[unit2_x][unit2_y], port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->core_array[unit2_x][unit2_y].get_uid()] = port1;
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->mem_controller_array[this->id], port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->mem_controller_array[this->id].uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->writer_array[unit2_x], port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->writer_array[unit2_x].uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, STANDARD_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->router_array[unit2_x][unit2_y], port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_array[unit1_x][unit1_y], port1, &this->dispatcher, port2));
			this->mem_array[unit1_x][unit1_y].uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->mem_array[unit1_x][unit1_y].get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port2] = this->mem_array[unit1_x][unit1_y].get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else if (unit1_type == 'X')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->core_array[unit2_x][unit2_y], port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->core_array[unit2_x][unit2_y].get_uid()] = port1;
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->mem_controller_array[this->id], port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->mem_controller_array[this->id].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->writer_array[unit2_x], port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->writer_array[unit2_x].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, STANDARD_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->router_array[unit2_x][unit2_y], port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->mem_controller_array[this->id], port1, &this->dispatcher, port2));
			this->mem_controller_array[this->id].uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port2] = this->mem_controller_array[this->id].get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else if (unit1_type == 'W')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->core_array[unit2_x][unit2_y], port2));
			this->writer_array[unit1_x].uid_to_port_map[this->core_array[unit2_x][unit2_y].get_uid()] = port1;
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->writer_array[unit1_x].uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->mem_controller_array[this->id], port2));
			this->writer_array[unit1_x].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->mem_controller_array[this->id].uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->writer_array[unit2_x], port2));
			this->writer_array[unit1_x].uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->writer_array[unit2_x].uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, STANDARD_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->router_array[unit2_x][unit2_y], port2));
			this->writer_array[unit1_x].uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->writer_array[unit1_x], port1, &this->dispatcher, port2));
			this->writer_array[unit1_x].uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->writer_array[unit1_x].get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port2] = this->writer_array[unit1_x].get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else if (unit1_type == 'R')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->core_array[unit2_x][unit2_y], port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->core_array[unit2_x][unit2_y].get_uid()] = port1;
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->mem_controller_array[this->id], port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->mem_controller_array[this->id].uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->writer_array[unit2_x], port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->writer_array[unit2_x].uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->router_array[unit2_x][unit2_y], port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, STANDARD_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->router_array[unit1_x][unit1_y], port1, &this->dispatcher, port2));
			this->router_array[unit1_x][unit1_y].uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->router_array[unit1_x][unit1_y].get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port2] = this->router_array[unit1_x][unit1_y].get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else if (unit1_type == 'D')
	{
		if (unit2_type == 'C')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->core_array[unit2_x][unit2_y], port2));
			this->dispatcher.uid_to_port_map[this->core_array[unit2_x][unit2_y].get_uid()] = port1;
			this->dispatcher.port_to_uid_map[port1] = this->core_array[unit2_x][unit2_y].get_uid();
			this->core_array[unit2_x][unit2_y].uid_to_port_map[this->dispatcher.get_uid()] = port2;
		}
		else if (unit2_type == 'M')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->mem_array[unit2_x][unit2_y], port2));
			this->dispatcher.uid_to_port_map[this->mem_array[unit2_x][unit2_y].get_uid()] = port1;
			this->dispatcher.port_to_uid_map[port1] = this->mem_array[unit2_x][unit2_y].get_uid();
			this->mem_array[unit2_x][unit2_y].uid_to_port_map[this->dispatcher.get_uid()] = port2;
		}
		else if (unit2_type == 'X')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->mem_controller_array[this->id], port2));
			this->dispatcher.uid_to_port_map[this->mem_controller_array[this->id].get_uid()] = port1;
			this->dispatcher.port_to_uid_map[port1] = this->mem_controller_array[this->id].get_uid();
			this->mem_controller_array[this->id].uid_to_port_map[this->dispatcher.get_uid()] = port2;
		}
		else if (unit2_type == 'W')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->writer_array[unit2_x], port2));
			this->dispatcher.uid_to_port_map[this->writer_array[unit2_x].get_uid()] = port1;
			this->dispatcher.port_to_uid_map[port1] = this->writer_array[unit2_x].get_uid();
			this->writer_array[unit2_x].uid_to_port_map[this->dispatcher.get_uid()] = port2;
		}
		else if (unit2_type == 'R')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, STANDARD_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->router_array[unit2_x][unit2_y], port2));
			this->dispatcher.uid_to_port_map[this->router_array[unit2_x][unit2_y].get_uid()] = port1;
			this->dispatcher.port_to_uid_map[port1] = this->router_array[unit2_x][unit2_y].get_uid();
			this->router_array[unit2_x][unit2_y].uid_to_port_map[this->dispatcher.get_uid()] = port2;
		}
		else if (unit2_type == 'D')
		{
			this->gcp->add_edge_to_arch_graph(unit1, unit2, PENALTY_WEIGHT, PENALTY_WEIGHT);
			this->conn_array.push_back(create_link(this->gcp, &this->dispatcher, port1, &this->dispatcher, port2));
			this->dispatcher.uid_to_port_map[this->dispatcher.get_uid()] = port1;
			this->dispatcher.uid_to_port_map[this->dispatcher.get_uid()] = port2;
			this->dispatcher.port_to_uid_map[port1] = this->dispatcher.get_uid();
			this->dispatcher.port_to_uid_map[port2] = this->dispatcher.get_uid();
		}
		else
		{
			cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
		}
	}
	else
	{
		cout << FATAL_ERROR << "Error creating link between " << unit1_type << " and " << unit2_type << endl;
	}
}

// Generate CGRA Mesh topology
void NeuraTile::generate_cgra_mesh_topology()
{
	cout << "Generating CGRA mesh topology \u2713" << endl;
	// NeuraMem Connections
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			string neuramem_name = "M_" + to_string(i) + "_" + to_string(j);
			if (j % 2 == 0)
			{
				// TOP LEFT Port
				int router_x_cood = i * 2;
				int router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				string router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_TOP_LEFT_PORT, router_name, RTR_BOTTOM_RIGHT_PORT);
				// TOP RIGHT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_TOP_RIGHT_PORT, router_name, RTR_BOTTOM_LEFT_PORT);
				// BOTTOM LEFT Port
				router_x_cood = i * 2;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_BOTTOM_LEFT_PORT, router_name, RTR_TOP_RIGHT_PORT);
				// BOTTOM RIGHT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_BOTTOM_RIGHT_PORT, router_name, RTR_TOP_LEFT_PORT);
			}
			else
			{
				// TOP LEFT Port
				int router_x_cood = (i * 2) + 1;
				int router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}

				string router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_TOP_LEFT_PORT, router_name, RTR_BOTTOM_RIGHT_PORT);
				// TOP RIGHT Port
				router_x_cood = (i * 2) + 2;
				router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_TOP_RIGHT_PORT, router_name, RTR_BOTTOM_LEFT_PORT);
				// BOTTOM LEFT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_BOTTOM_LEFT_PORT, router_name, RTR_TOP_RIGHT_PORT);
				// BOTTOM RIGHT Port
				router_x_cood = (i * 2) + 2;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuramem_name, MEM_BOTTOM_RIGHT_PORT, router_name, RTR_TOP_LEFT_PORT);
			}
		}
	}

	// NeuraCore Connections
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			string neuracore_name = "C_" + to_string(i) + "_" + to_string(j);
			if (j % 2 == 0)
			{
				// TOP LEFT Port
				int router_x_cood = (i * 2) + 1;
				int router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				string router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_TOP_LEFT_PORT, router_name, RTR_BOTTOM_RIGHT_PORT);
				// TOP RIGHT Port
				router_x_cood = (i * 2) + 2;
				router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_TOP_RIGHT_PORT, router_name, RTR_BOTTOM_LEFT_PORT);
				// BOTTOM LEFT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_BOTTOM_LEFT_PORT, router_name, RTR_TOP_RIGHT_PORT);
				// BOTTOM RIGHT Port
				router_x_cood = (i * 2) + 2;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_BOTTOM_RIGHT_PORT, router_name, RTR_TOP_LEFT_PORT);
			}
			else
			{
				// TOP LEFT Port
				int router_x_cood = i * 2;
				int router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				string router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_TOP_LEFT_PORT, router_name, RTR_BOTTOM_RIGHT_PORT);
				// TOP RIGHT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_TOP_RIGHT_PORT, router_name, RTR_BOTTOM_LEFT_PORT);
				// BOTTOM LEFT Port
				router_x_cood = i * 2;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_BOTTOM_LEFT_PORT, router_name, RTR_TOP_RIGHT_PORT);
				// BOTTOM RIGHT Port
				router_x_cood = (i * 2) + 1;
				router_y_cood = j + 1;
				if (router_x_cood >= this->router_x_count)
				{
					router_x_cood = router_x_cood % this->router_x_count;
				}
				if (router_y_cood >= this->router_y_count)
				{
					router_y_cood = router_y_cood % this->router_y_count;
				}
				if (router_x_cood >= this->router_x_count || router_y_cood >= this->router_y_count)
				{
					cout << FATAL_ERROR << "Router coordinates out of bounds" << endl;
				}
				router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
				this->connect_units(neuracore_name, CORE_BOTTOM_RIGHT_PORT, router_name, RTR_TOP_LEFT_PORT);
			}
		}
	}

	// NeuraDispatcher Connections
	int connect_dispatcher_port = 0;
	for (auto y = 0; y < this->core_y_count; y++)
	{
		for (auto x = 0; x < this->core_x_count; x++)
		{
			string neuradispatcher_name = "D_0"; // + to_string(i) + "_" + to_string(j);
			string core_name = "C_" + to_string(x) + "_" + to_string(y);
			this->connect_units(neuradispatcher_name, connect_dispatcher_port, core_name, CORE_INSTX_PORT);
			this->dispatcher.insert_uid_to_port_map_entry(this->core_array[x][y].get_uid(), connect_dispatcher_port);
			connect_dispatcher_port++;
		}
	}

	// NeuraRouter Connections
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			string router_name = "R_" + to_string(i) + "_" + to_string(j);
			// Each router has to connect only to TOP and RIGHT routers
			// TOP Port
			int router_x_cood = i;
			int router_y_cood = (j + this->router_y_count - 1) % this->router_y_count;
			string top_router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
			this->connect_units(router_name, RTR_TOP_PORT, top_router_name, RTR_BOTTOM_PORT);
			// RIGHT Port
			router_x_cood = (i + 1) % this->router_x_count;
			router_y_cood = j;
			string right_router_name = "R_" + to_string(router_x_cood) + "_" + to_string(router_y_cood);
			this->connect_units(router_name, RTR_RIGHT_PORT, right_router_name, RTR_LEFT_PORT);
		}
	}

	// NeuraWriters Connections
	// TODO: Compute Router indices for Writers dynamically
	if (this->gcp->NEURA_TILE_SIZE == 2)
	{
		string writer_name = "W_0";
		string router_name = "R_2_1";
		this->connect_units(writer_name, WRITER_RTR_PORT, router_name, RTR_EXTRA_PORT);
		writer_name = "W_0";
		string mc_name = "X_0";
		this->connect_units(writer_name, WRITER_MC_PORT, mc_name, MC_WRITER_0_PORT);
		writer_name = "W_1";
		router_name = "R_0_3";
		this->connect_units(writer_name, WRITER_RTR_PORT, router_name, RTR_EXTRA_PORT);
		writer_name = "W_1";
		mc_name = "X_0";
		this->connect_units(writer_name, WRITER_MC_PORT, mc_name, MC_WRITER_1_PORT);
	}
	else
	{
		string writer_name = "W_0";
		string router_name = "R_6_1";
		this->connect_units(writer_name, WRITER_RTR_PORT, router_name, RTR_EXTRA_PORT);
		writer_name = "W_0";
		string mc_name = "X_0";
		this->connect_units(writer_name, WRITER_MC_PORT, mc_name, MC_WRITER_0_PORT);
		writer_name = "W_1";
		router_name = "R_2_3";
		this->connect_units(writer_name, WRITER_RTR_PORT, router_name, RTR_EXTRA_PORT);
		writer_name = "W_1";
		mc_name = "X_0";
		this->connect_units(writer_name, WRITER_MC_PORT, mc_name, MC_WRITER_1_PORT);
	}

	// Memory Controller Connections
	// TODO: Compute Router indices for memory controller dynamically
	if (this->gcp->NEURA_TILE_SIZE == 2)
	{
		string mc_name = "X_0";
		string router_name = "R_2_0";
		this->connect_units(mc_name, MC_RTR_0_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_0_2";
		this->connect_units(mc_name, MC_RTR_1_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_1_1";
		this->connect_units(mc_name, MC_RTR_2_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_0_3";
		this->connect_units(mc_name, MC_RTR_3_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_3_0";
		this->connect_units(mc_name, MC_RTR_4_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_1_3";
		this->connect_units(mc_name, MC_RTR_5_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_3_3";
		this->connect_units(mc_name, MC_RTR_6_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_2_2";
		this->connect_units(mc_name, MC_RTR_7_PORT, router_name, RTR_EXTRA_PORT);
	}
	else
	{
		string mc_name = "X_0";
		string router_name = "R_0_2";
		this->connect_units(mc_name, MC_RTR_0_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_4_0";
		this->connect_units(mc_name, MC_RTR_1_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_1_1";
		this->connect_units(mc_name, MC_RTR_2_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_3_2";
		this->connect_units(mc_name, MC_RTR_3_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_7_0";
		this->connect_units(mc_name, MC_RTR_4_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_5_3";
		this->connect_units(mc_name, MC_RTR_5_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_7_3";
		this->connect_units(mc_name, MC_RTR_6_PORT, router_name, RTR_EXTRA_PORT);
		mc_name = "X_0";
		router_name = "R_0_3";
		this->connect_units(mc_name, MC_RTR_7_PORT, router_name, RTR_EXTRA_PORT);
	}
}

// Destructor
NeuraTile::~NeuraTile()
{
	// cout << "NeuraTile::~NeuraTile()" << endl;
	// Delete all cores
	// cout << "Deleting Cores" << endl;
	for (auto i = 0; i < this->core_x_count; i++)
	{
		delete[] this->core_array[i];
	}
	delete[] this->core_array;

	// Delete all memories
	// cout << "Deleting Memories" << endl;
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		delete[] this->mem_array[i];
	}
	delete[] this->mem_array;

	// Delete all routers
	// cout << "Deleting Routers" << endl;
	for (auto i = 0; i < this->router_x_count; i++)
	{
		delete[] this->router_array[i];
	}
	delete[] this->router_array;

	// Delete all connections
	// cout << "Deleting Connections" << endl;
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		delete this->conn_array[i];
	}

	// Delete all ports
	// cout << "Deleting Ports" << endl;
	delete[] this->in_port_array;
	delete[] this->out_port_array;

	// Its not dynamic memory allocation
	// You don't need to delete it
	// Delete dispatcher array
	// for (auto i=0; i<this->core_x_count; i++) {
	// 	delete[] this->dispatcher_array[i];
	// }
	// delete[] this->dispatcher_array;

	// Delete memory controller array
	// cout << "Deleting Memory Controllers" << endl;
	delete[] this->mem_controller_array;

	// Delete all writers
	// for (auto i=0; i<this->writer_count; i++) {
	// 	delete[] this->writer_array[i];
	// }
	// cout << "Deleting Writers" << endl;
	delete[] this->writer_array;

	// Delete GlobalCounters
	// cout << "Deleting Global Counters" << endl;
	delete this->gcp;
}

int NeuraTile::get_id()
{
	return this->id;
}

// int NeuraTile::get_uid()
// {
// 	return this->uid;
// }

// Print NeuraTile Information
void NeuraTile::print()
{
	cout << "NeuraTile ID: " << this->id << endl;
	cout << "NeuraTile X Coordinate: " << this->x_cood << endl;
	cout << "NeuraTile Y Coordinate: " << this->y_cood << endl;
}

// Tick
// @return: True if simulation is over
bool NeuraTile::tick()
{
#ifdef TRACE_INSTX
	cout << "******** Cycle: " << CYAN_START << this->gcp->get_cycle() << COLOR_RESET << "********" << endl;
#endif
	bool sim_end = true;
	bool interval_tick;
	if (this->gcp->get_cycle() % this->gcp->INTERVAL == 0)
	{
		interval_tick = true;
	}
	else
	{
		interval_tick = false;
	}
	// Storing tick output before because "C++ short-circuits logical AND"
	bool tick_output;
	// bool unit_sim_end = true;
	// Tick all connections
	// It is important to tick all the connections before the components because
	// the components create data and the connections send data.
	// We dont send data in the same cycle that we create it.

	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		// cout << "Ticking Connection [" << i << "]" << endl;
		tick_output = this->conn_array[i]->tick();
		sim_end = sim_end && tick_output;
	}

#ifdef DUAL_TX_CONN
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		// cout << "Ticking Connection [" << i << "]" << endl;
		tick_output = this->conn_array[i]->tick();
		sim_end = sim_end && tick_output;
	}
#endif // DUAL_TX_CONN

	// #pragma omp parallel for
	for (int component_id = 0; component_id < 6; component_id++)
	{

		if (component_id == DISPATCHER_COMPONENT_INDEX)
		{
			// Tick dispatcher
			bool dispatcher_tick_output = this->dispatcher.tick();
			// sim_end = sim_end && tick_output;

			// cout << "Dispatcher Tick: ";
			// sim_end_printer(unit_sim_end);
			// cout << endl;

			// for (auto i=0; i<this->core_x_count; i++) {
			// 	for (auto j=0; j<this->core_y_count; j++) {
			// 		// cout << "Ticking Dispatcher [" << i << "][" << j << "]" << endl;
			// 		tick_output = this->dispatcher_array[i][j].tick();
			// 		sim_end = sim_end && tick_output;
			// 	}
			// }
			// tick_output = this->dispatcher->tick();

			this->component_sim_end_array[DISPATCHER_COMPONENT_INDEX] = dispatcher_tick_output;
		}
		else if (component_id == CORE_COMPONENT_INDEX)
		{
			// Tick all cores
			bool core_sim_end = true;
			// #pragma omp parallel for collapse(2) reduction(&& : core_sim_end)
			for (auto i = 0; i < this->core_x_count; i++)
			{
				for (auto j = 0; j < this->core_y_count; j++)
				{
					// cout << "Ticking Core [" << i << "][" << j << "]" << endl;
					bool core_tick_output = this->core_array[i][j].tick();
					core_sim_end = core_sim_end && core_tick_output;
				}
			}
			this->component_sim_end_array[CORE_COMPONENT_INDEX] = core_sim_end;
		}
		else if (component_id == MEM_COMPONENT_INDEX)
		{

			// cout << "Core Tick: ";
			// sim_end_printer(unit_sim_end);
			// cout << endl;

			// Tick all memories
			bool mem_sim_end = true;
			// #pragma omp parallel for collapse(2) reduction(&& : mem_sim_end)
			for (auto m_tick = 0; m_tick < MULTI_TICK_NEURAMEM; m_tick++)
			{
				for (auto i = 0; i < this->mem_x_count; i++)
				{
					for (auto j = 0; j < this->mem_y_count; j++)
					{
						// cout << "Ticking Memory [" << i << "][" << j << "]" << endl;
						bool mem_tick_output = this->mem_array[i][j].tick();
						mem_sim_end = mem_sim_end && mem_tick_output;
					}
				}
			}
			this->component_sim_end_array[MEM_COMPONENT_INDEX] = mem_sim_end;
		}
		else if (component_id == RTR_COMPONENT_INDEX)
		{

			// cout << "Mem Tick: ";
			// sim_end_printer(unit_sim_end);
			// cout << endl;

			// Tick all routers
			bool router_sim_end = true;
			// #pragma omp parallel for collapse(2)
			for (auto r_tick = 0; r_tick < MULTI_TICK_ROUTER; r_tick++)
			{
				for (auto i = 0; i < this->router_x_count; i++)
				{
					for (auto j = 0; j < this->router_y_count; j++)
					{
						// cout << "Ticking Router [" << i << "][" << j << "]" << endl;
						bool router_tick_output = this->router_array[i][j].tick();
						router_sim_end = router_sim_end && router_tick_output;
					}
				}
			}
			this->component_sim_end_array[RTR_COMPONENT_INDEX] = router_sim_end;

			// cout << "Router Tick: ";
			// sim_end_printer(unit_sim_end);
			// cout << endl;
		}
		else if (component_id == WRITER_COMPONENT_INDEX)
		{
			// Tick all Writers
			bool writer_sim_end = true;
			// #NOpragma omp parallel for
			for (auto w_tick = 0; w_tick < MULTI_TICK_WRITER; w_tick++)
			{
				for (auto i = 0; i < this->writer_count; i++)
				{
					// cout << "Ticking Writer [" << i << "][" << j << "]" << endl;
					bool writer_tick_output = this->writer_array[i].tick();
					writer_sim_end = writer_sim_end && writer_tick_output;
				}
			}
			this->component_sim_end_array[WRITER_COMPONENT_INDEX] = writer_sim_end;
		}
		else if (component_id == MC_COMPONENT_INDEX)
		{

			// cout << "Writer Tick: ";
			// sim_end_printer(unit_sim_end);
			// cout << endl;

			// Tick Memory Controller
			bool mc_sim_end = true;
			bool mc_tick_output = this->mem_controller_array[this->id].tick();
			// mc_sim_end = mc_sim_end && mc_tick_output;

			this->component_sim_end_array[MC_COMPONENT_INDEX] = mc_sim_end;
		}
	}

	// Reduce all component sim end flags and store in sim_end
	for (int component_id = 0; component_id < 6; component_id++)
	{
		sim_end = sim_end && this->component_sim_end_array[component_id];
	}

	// cout << "Mem Controller Tick: ";
	// sim_end_printer(unit_sim_end);
	// cout << endl;

	// Tick Global Counters
	this->gcp->tick();

	if (interval_tick)
	{
		this->interval_metrics();
		this->gcp->print_sim_speed();
	}

	// Barrier to limit the number of cycles
	if (this->gcp->get_cycle() > this->gcp->HARD_STOP)
	{ // TODO: Remove this
		// Print Simulation End Flags
		this->print_sim_end();
		return (this->return_routine(SIM_END_TRUE, COMPUTED));
	}

	// TODO: Remove this
	//  this->print_sim_end();

	return (this->return_routine(sim_end, COMPUTED));
}

void NeuraTile::interval_metrics()
{
	// Tick all interval metrics
	// Tick all connections
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		this->conn_array[i]->interval_metrics();
	}
	// Tick dispatcher
	this->dispatcher.interval_metrics();
	// for (auto i=0; i<this->core_x_count; i++) {
	// 	for (auto j=0; j<this->core_y_count; j++) {
	// 		this->dispatcher_array[i][j].interval_metrics();
	// 	}
	// }
	// this->dispatcher->interval_metrics();

	// Tick all cores
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].interval_metrics();
		}
	}

	// Tick all memories
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			this->mem_array[i][j].interval_metrics();
		}
	}

	// Tick all routers
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			this->router_array[i][j].interval_metrics();
		}
	}

	// Tick all Writers
	for (auto i = 0; i < this->writer_count; i++)
	{
		this->writer_array[i].interval_metrics();
	}

	// Tick Memory Controller
	this->mem_controller_array[this->id].interval_metrics();

	// Tick Global Counters
	this->gcp->interval_metrics();

	// Tick self
	this->self_interval_metrics();
}

// Statistics
void NeuraTile::statistics()
{
	cout << "************* NeuraTile ID: " << this->id << " *************" << endl;
	// Print Statistics for dispatcher
	this->dispatcher.statistics();
	// for (auto i=0; i<this->core_x_count; i++) {
	// 	for (auto j=0; j<this->core_y_count; j++) {
	// 		this->dispatcher_array[i][j].statistics();
	// 	}
	// }
	// this->dispatcher->statistics();
	// Print Statistics for all cores
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].statistics();
		}
	}
	// Print Statistics for all memories
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			this->mem_array[i][j].statistics();
		}
	}
	// Print Statistics for all routers
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			// cout << "Neura Tile Router [" << i << "][" << j << "]" << endl;
			this->router_array[i][j].statistics();
		}
	}
	// Print Statistics for Memory Controller
	this->mem_controller_array[this->id].statistics();

	// Print Statistics for all Writers
	for (auto i = 0; i < this->writer_count; i++)
	{
		this->writer_array[i].statistics();
	}

	// Print Statistics for all connections
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		this->conn_array[i]->statistics();
	}
	// Print final statistics
	this->gcp->statistics();
}

// Statistics
void NeuraTile::smart_stats()
{
	// Print Statistics for dispatcher
	this->dispatcher.smart_stats();
	// for (auto i=0; i<this->core_x_count; i++) {
	// 	for (auto j=0; j<this->core_y_count; j++) {
	// 		this->dispatcher_array[i][j].statistics();
	// 	}
	// }
	// this->dispatcher->statistics();
	// Print Statistics for all cores
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].smart_stats();
		}
	}
	// Print Statistics for all memories
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			// this->mem_array[i][j].statistics();
		}
	}
	// Print Statistics for all routers
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			// cout << "Neura Tile Router [" << i << "][" << j << "]" << endl;
			this->router_array[i][j].smart_stats();
		}
	}
	// Print Statistics for Memory Controller
	this->mem_controller_array[this->id].smart_stats();

	// Print Statistics for all connections
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		this->conn_array[i]->smart_stats();
	}
	// Print final statistics
	this->gcp->smart_stats();
}

void NeuraTile::print_sim_end()
{
	cout << "********** Sim End Flags **********" << endl;
	cout << "Dispatcher: ";
	sim_end_printer(this->dispatcher.get_sim_end());
	cout << endl;
	// for (auto i = 0; i < this->core_x_count; i++)
	// {
	// 	for (auto j = 0; j < this->core_y_count; j++)
	// 	{
	// 		cout << "Dispatcher [" << i << "][" << j << "]: ";
	// 		sim_end_printer(this->dispatcher_array[i][j].get_sim_end());
	// 		cout << endl;
	// 	}
	// }
	// cout << "Dispatcher: ";
	// sim_end_printer(this->dispatcher->get_sim_end());
	// cout << endl;
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			cout << "Core [" << i << "][" << j << "]: ";
			sim_end_printer(this->core_array[i][j].get_sim_end());
			cout << endl;
		}
	}
	// Print Statistics for all memories
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			// this->mem_array[i][j].statistics();
			cout << "Memory [" << i << "][" << j << "]: ";
			sim_end_printer(this->mem_array[i][j].get_sim_end());
			cout << endl;
		}
	}
	// Print Statistics for all routers
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			cout << "Router [" << i << "][" << j << "]: ";
			sim_end_printer(this->router_array[i][j].get_sim_end());
			cout << endl;
		}
	}
	// Print Statistics for Memory Controller
	cout << "Memory Controller: ";
	sim_end_printer(this->mem_controller_array[this->id].get_sim_end());
	cout << endl;

	// Print Statistics for all connections
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		cout << "Connection [" << i << "]: ";
		sim_end_printer(this->conn_array[i]->get_sim_end());
		cout << endl;
	}

	cout << "********** End of Sim End Flags **********" << endl;

	this->mem_controller_array[this->id].print_json();
}

// Post Sim Metrics
void NeuraTile::post_sim_metrics()
{
	// Post Sim Metrics for all cores
	// cout << "Post Sim Metrics for all cores" << endl;
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			this->core_array[i][j].post_sim_metrics();
		}
	}
	// Post Sim Metrics for all memories
	// cout << "Post Sim Metrics for all memories" << endl;
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			this->mem_array[i][j].post_sim_metrics();
		}
	}
	// Post Sim Metrics for all connections
	// cout << "Post Sim Metrics for all connections" << endl;
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		this->conn_array[i]->post_sim_metrics();
	}
	// Post Sim Metrics for Global Counters
	this->gcp->post_sim_metrics();

	// Post Sim Metrics for all routers
	// cout << "Post Sim Metrics for all routers" << endl;
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			this->router_array[i][j].post_sim_metrics();
		}
	}
	// Post Sim Metrics for Memory Controller
	// cout << "Post Sim Metrics for Memory Controller" << endl;
	this->mem_controller_array[this->id].post_sim_metrics();

	// Post Sim Metrics for Dispatcher
	// cout << "Post Sim Metrics for Dispatcher" << endl;
	this->dispatcher.post_sim_metrics();
	// for (auto i = 0; i < this->core_x_count; i++)
	// {
	// 	for (auto j = 0; j < this->core_y_count; j++)
	// 	{
	// 		this->dispatcher_array[i][j].post_sim_metrics();
	// 	}
	// }
	// this->dispatcher->post_sim_metrics();

	// Post Sim Metrics for all Writers
	// cout << "Post Sim Metrics for all Writers" << endl;
	for (auto i = 0; i < this->writer_count; i++)
	{
		this->writer_array[i].post_sim_metrics();
	}

	// Post Sim Metrics for NeuraTile
	// Self Post Sim Metrics
	// cout << "Post Sim Metrics for NeuraTile" << endl;
	this->self_post_sim_metrics();
}

// Sanity Check
bool NeuraTile::sanity_check()
{
	bool sanity = true;
	// Sanity Check for all cores
	for (auto i = 0; i < this->core_x_count; i++)
	{
		for (auto j = 0; j < this->core_y_count; j++)
		{
			sanity = sanity && this->core_array[i][j].sanity_check();
			if (!sanity)
			{
				cout << FATAL_ERROR << "Core [" << i << "][" << j << "] Sanity Check Failed" << endl;
			}
		}
	}
	// Sanity Check for all memories
	for (auto i = 0; i < this->mem_x_count; i++)
	{
		for (auto j = 0; j < this->mem_y_count; j++)
		{
			sanity = sanity && this->mem_array[i][j].sanity_check();
			if (!sanity)
			{
				cout << FATAL_ERROR << "Memory [" << i << "][" << j << "] Sanity Check Failed" << endl;
			}
		}
	}
	// Sanity Check for Global Counters
	sanity = sanity && this->gcp->sanity_check();
	if (!sanity)
	{
		cout << FATAL_ERROR << "Global Counters Sanity Check Failed" << endl;
	}

	// Sanity Check for all routers
	for (auto i = 0; i < this->router_x_count; i++)
	{
		for (auto j = 0; j < this->router_y_count; j++)
		{
			sanity = sanity && this->router_array[i][j].sanity_check();
			if (!sanity)
			{
				cout << FATAL_ERROR << "Router [" << i << "][" << j << "] Sanity Check Failed" << endl;
			}
		}
	}

	// Sanity Check for Memory Controller
	sanity = sanity && this->mem_controller_array[this->id].sanity_check();
	if (!sanity)
	{
		cout << FATAL_ERROR << "Memory Controller [" << this->id << "] Sanity Check Failed" << endl;
	}

	// Sanity Check for Dispatcher
	sanity = sanity && this->dispatcher.sanity_check();
	if (!sanity)
	{
		cout << FATAL_ERROR << "Dispatcher Sanity Check Failed" << endl;
	}
	// for (auto i = 0; i < this->core_x_count; i++)
	// {
	// 	for (auto j = 0; j < this->core_y_count; j++)
	// 	{
	// 		sanity = sanity && this->dispatcher_array[i][j].sanity_check();
	// 	}
	// }
	// sanity = sanity && this->dispatcher->sanity_check();

	// Sanity Check for all Writers
	for (auto i = 0; i < this->writer_count; i++)
	{
		sanity = sanity && this->writer_array[i].sanity_check();
		if (!sanity)
		{
			cout << FATAL_ERROR << "Writer [" << i << "] Sanity Check Failed" << endl;
		}
	}

	// Sanity Check for all connections
	for (size_t i = 0; i < this->conn_array.size(); i++)
	{
		sanity = sanity && this->conn_array[i]->sanity_check();
		if (!sanity)
		{
			cout << FATAL_ERROR << "Connection [" << i << "] connecting " << this->conn_array[i]->connected_vertices_names << " Sanity Check Failed" << endl;
		}
	}

	// Print Sanity Check Result
	if (sanity)
	{
		cout << "Sanity Check Passed" << endl;
	}
	else
	{
		cout << FATAL_ERROR << "Sanity Check Failed" << endl;
		exit(1);
	}
	return sanity;
}

queue<InstX *> *NeuraTile::get_in_port_ref(int port_id)
{
	return &this->in_port_array[port_id];
}

queue<InstX *> *NeuraTile::get_out_port_ref(int port_id)
{
	return &this->out_port_array[port_id];
}

bool NeuraTile::get_sim_end()
{
	return this->sim_end;
}

void NeuraTile::enable_max_buffer_sizes_computation()
{
	for (auto it : this->conn_array)
	{
		it->measure_max_buffer_sizes();
	}
}

void NeuraTile::set_sim_start_time()
{
	this->gcp->set_sim_start_time();
}

bool NeuraTile::return_routine(bool sim_end, bool computed)
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

void NeuraTile::self_post_sim_metrics()
{
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("busy_cycles", (long)this->busy_cycles),
			kvp("idle_cycles", (long)this->idle_cycles)));
	}
}

void NeuraTile::self_interval_metrics()
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
}

GlobalCounters *NeuraTile::get_gcp()
{
	return this->gcp;
}

string NeuraTile::get_vertex_name()
{
	return this->vertex_name;
}