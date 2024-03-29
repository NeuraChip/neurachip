// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "conn.h"
#include "../debugger/debugger.h"

using namespace std;

Conn::Conn(GlobalCounters *gcp)
{
	this->gcp = gcp;
	this->id = this->gcp->generate_conn_id();
	this->sim_end = false;
	// Oscillates between 0 and 1 to indicate the next link to be used
	this->next_link_id = 0;
	this->link_0_in_port_max_buffer_size = -1;
	this->link_0_out_port_max_buffer_size = -1;
	this->link_1_in_port_max_buffer_size = -1;
	this->link_1_out_port_max_buffer_size = -1;
	this->max_buffer_sizes_computation = false;
	this->idle_cycles = 0;
	this->busy_cycles = 0;
	this->interval_idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_id = 0;
	this->component_name = "Conn_" + to_string(this->id);
	this->header_component_name = "H_" + this->component_name;
	this->collection = this->gcp->mongo->get_component_collection(this->component_name);
	this->header_collection = this->gcp->mongo->get_component_collection(this->header_component_name);
}

Conn::~Conn()
{
}

void Conn::print()
{
	cout << "Connection ID: " << this->id << endl;
}

bool Conn::tick()
{
	// cout << "Connection Tick: " << this->id << endl;
	// this->statistics();
	this->next_link_id = this->get_next_link_id();
	// cout << "Next Link ID: " << this->next_link_id << endl;
	// If the next link is -1 (no packet in the system)
	if (this->next_link_id == -1)
	{
		return (this->return_routine(SIM_END_TRUE, CONN_STALLED));
	}
	// If the next link is 0, send the packet to link 0
	if (this->next_link_id == 0)
	{
		// Get the packet
		InstX *mem_req = this->link_0_in_port->front();
		// Send the packet to the output port of link 0
		this->link_0_out_port->push(mem_req);
		// Remove the packet from the input port of link 0
		this->link_0_in_port->pop();
		return (this->return_routine(SIM_END_FALSE, CONN_COMPUTED));
	}
	// If the next link is 1, send the packet to link 1
	else if (this->next_link_id == 1)
	{
		// Get the packet
		InstX *mem_req = this->link_1_in_port->front();
		// Send the packet to the output port of link 1
		this->link_1_out_port->push(mem_req);
		// Remove the packet from the input port of link 1
		this->link_1_in_port->pop();
		return (this->return_routine(SIM_END_FALSE, CONN_COMPUTED));
	}
	// If the next link is neither 0 nor 1, something is wrong
	else
	{
		// Print an error message
		cout << FATAL_ERROR << "Received invalid next link ID" << endl;
		// Exit the program
		exit(1);
	}
}

int Conn::get_next_link_id()
{
	// BUG: next link id does not need to be set to 0
	// this->next_link_id = 0;
	if (this->next_link_id == -1)
	{
		this->next_link_id = 0;
	}

	// If both links are empty, return -1
	if (this->link_0_in_port->empty() && this->link_1_in_port->empty())
	{
		return -1;
	}
	// If the next link is 0, and link 1 is not empty, return 1
	if (this->next_link_id == 0 && !this->link_1_in_port->empty())
	{
		return 1;
	}
	else if (this->next_link_id == 0 && this->link_1_in_port->empty())
	{
		return 0;
	}
	// If the next link is 1, and link 0 is not empty, return 0
	else if (this->next_link_id == 1 && !this->link_0_in_port->empty())
	{
		return 0;
	}
	else if (this->next_link_id == 1 && this->link_0_in_port->empty())
	{
		return 1;
	}
	// Else return the original link itself
	else
	{
		cout << FATAL_ERROR << "Invalid next link ID" << endl;
		cout << "Next Link ID: " << this->next_link_id << endl;
		cout << "Link 0: <" << this->link_0_in_port->size() << "/" << this->link_0_out_port->size() << ">" << endl;
		cout << "Link 1: <" << this->link_1_in_port->size() << "/" << this->link_1_out_port->size() << ">" << endl;
		exit(1);
	}
}

// Print the statistics of the connection
void Conn::statistics()
{
	// cout << "************* ";
	cout << YELLOW_START << "Connector: " << this->id << " " << CYAN_START << this->connected_vertices_names << COLOR_RESET;
	cout << "\t";
	//   <<  " *************" << endl;
	cout << "Pressure | Link 0 <";
	this->print_pressure_number(this->link_0_in_port->size());
	//  << CYAN_START << this->link_0_in_port->size() << COLOR_RESET;
	cout << "/";
	this->print_pressure_number(this->link_0_out_port->size());
	//  << CYAN_START << this->link_0_out_port->size() << COLOR_RESET;
	cout << "> | Link 1 <";
	this->print_pressure_number(this->link_1_in_port->size());
	//  << CYAN_START << this->link_1_in_port->size() << COLOR_RESET;
	cout << "/";
	this->print_pressure_number(this->link_1_out_port->size());

	//  << CYAN_START << this->link_1_out_port->size() << COLOR_RESET <<
	cout << ">" << endl;
	// cout << "****************************************" << endl;
}

void Conn::print_pressure_number(int pressure)
{
	if (pressure == 0)
	{
		cout << GREEN_START << pressure << COLOR_RESET;
	}
	else
	{
		cout << RED_START << pressure << COLOR_RESET;
	}
}

void Conn::smart_stats()
{
	if (this->link_0_in_port->size() > 0 ||
		this->link_0_out_port->size() > 0 ||
		this->link_1_in_port->size() > 0 ||
		this->link_1_out_port->size() > 0)
	{

		this->statistics();
	}
}

bool Conn::sanity_check()
{
	// If both link in ports are empty, return true
	if (this->link_0_in_port->empty() &&
		this->link_1_in_port->empty() &&
		this->link_0_out_port->empty() &&
		this->link_1_out_port->empty())
	{
		return true;
	}
	else
	{
		if (this->link_0_in_port->empty() == false)
		{
			cout << FATAL_ERROR << "Link 0 in port is not empty" << endl;
		}
		if (this->link_1_in_port->empty() == false)
		{
			cout << FATAL_ERROR << "Link 1 in port is not empty" << endl;
		}
		if (this->link_0_out_port->empty() == false)
		{
			cout << FATAL_ERROR << "Link 0 out port is not empty" << endl;
		}
		if (this->link_1_out_port->empty() == false)
		{
			cout << FATAL_ERROR << "Link 1 out port is not empty" << endl;
		}
		return false;
	}
}

bool Conn::get_sim_end()
{
	return this->sim_end;
}

void Conn::update_max_buffer_sizes()
{
	// Update the max buffer sizes of link 0
	if (this->link_0_in_port->size() > this->link_0_in_port_max_buffer_size)
	{
		this->link_0_in_port_max_buffer_size = this->link_0_in_port->size();
	}
	if (this->link_0_out_port->size() > this->link_0_out_port_max_buffer_size)
	{
		this->link_0_out_port_max_buffer_size = this->link_0_out_port->size();
	}
	// Update the max buffer sizes of link 1
	if (this->link_1_in_port->size() > this->link_1_in_port_max_buffer_size)
	{
		this->link_1_in_port_max_buffer_size = this->link_1_in_port->size();
	}
	if (this->link_1_out_port->size() > this->link_1_out_port_max_buffer_size)
	{
		this->link_1_out_port_max_buffer_size = this->link_1_out_port->size();
	}
}

void Conn::measure_max_buffer_sizes()
{
	this->max_buffer_sizes_computation = true;
}

bool Conn::return_routine(bool sim_end, bool computed)
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
	if (this->max_buffer_sizes_computation)
	{
		this->update_max_buffer_sizes();
	}
	return this->sim_end;
}

void Conn::post_sim_metrics()
{
	if (MONGO_FLAG)
	{
		this->header_collection.insert_one(make_document(
			kvp("name", "post_sim_metrics"),
			kvp("id", (long)this->id),
			kvp("connected_comps", this->connected_vertices_names),
			kvp("idle_cycles", (long)this->idle_cycles),
			kvp("busy_cycles", (long)this->busy_cycles)));
	}
	return;
}

void Conn::interval_metrics()
{
	if (MONGO_FLAG)
	{
		if (CONN_INTERVAL_MONGO_FLAG)
		{
			this->collection.insert_one(make_document(
				kvp("name", "interval_metrics"),
				kvp("interval_id", (long)this->interval_id),
				kvp("interval_idle_cycles", (long)this->interval_idle_cycles),
				kvp("interval_busy_cycles", (long)this->interval_busy_cycles)));
		}
	}
	this->interval_idle_cycles = 0;
	this->interval_busy_cycles = 0;
	this->interval_id++;
	return;
}

