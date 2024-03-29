// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

// #include "neuratile.h"
#include "functions.h"
#include "debugger/debugger.h"
#include "config.h"
#include "neuratile/neuratile.h"
#include "connector/conn.h"
#include "argparse/argparse.h"

#include <string>

using namespace std;

// class NeuraTile;

Input get_inputs(int argc, char *argv[], Input input, int printer)
{
	// cout << "\n\n\n\n\n\n\n\n\n\n\e[1;32mProgram Start\e[0m\n";
	if (argc < 2)
	{
		if (printer)
		{
			print_inputs(argc, input);
		}
		return (input);
	}
	input.number = (long *)malloc(sizeof(long) * argc);

	for (int i = 1; i < argc; i++)
	{
		input.number[i] = atol(argv[i]);
	}

	if (printer)
	{
		print_inputs(argc, input);
	}
	return (input);
}

void print_inputs(int argc, Input input)
{
	if (argc < 2)
	{
		cout << "No Inputs Provided" << endl;
	}
	for (int i = 1; i < argc; i++)
	{
		cout << "Input[" << i << "]: " << input.number[i] << endl;
	}
}

string get_date()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	// Format MM-DD-YYYY

	string date = to_string(1 + ltm->tm_mon) + "/" + to_string(ltm->tm_mday) + "/" + to_string(1900 + ltm->tm_year);
	return date;
}

long get_hash_instx_count(string program_file)
{
	long hash_instx_count = 0;
	ifstream program_file_stream;
	program_file_stream.open(program_file);
	string line;
	while (getline(program_file_stream, line))
	{
		if (line.find("MULT_HASH_ACC") != string::npos)
		{
			hash_instx_count++;
		}
	}
	return hash_instx_count;
}

NeuraTile *sim_init(ArgParse *arg_parse)
{

	string exp_name = arg_parse->get_exp_name();
	string description = arg_parse->get_description();
	string dataset = arg_parse->get_dataset();
	string program_file = arg_parse->get_dataset_root() + "/" + "bin" + "/" + "spmm.csv";
	int tile_size = arg_parse->get_tile_size();

	mongocxx::instance *instance;
	// This should be called only once per program.
	instance = new mongocxx::instance{};
	Mongo *ui_db;
	ui_db = new Mongo("UI_DB");
	mongocxx::collection ui_collection;
	ui_collection = ui_db->get_component_collection("UI_COLLECTION");
	ui_db->remove_document("UI_COLLECTION", "exp_name", exp_name);
	ui_collection.insert_one(make_document(kvp("exp_name", exp_name),
										   kvp("description", description),
										   kvp("dataset", dataset),
										   kvp("tile_size", tile_size),
										   kvp("hash_instx_count", get_hash_instx_count(program_file)),
										   kvp("start_date", get_date()),
										   kvp("completed", 0),
										   kvp("display", 0)));

	GlobalCounters *gcp;
	gcp = new GlobalCounters(arg_parse);
	NeuraTile *neura_tile;
	neura_tile = new NeuraTile(gcp, 0, 0, gcp->NEURA_TILE_PORT_COUNT);
	return neura_tile;
}

int run_sim(NeuraTile *neura_tile)
{
	cout << "Running Simulation" << endl;
	bool sim_end = false;
	neura_tile->set_sim_start_time();
	long long min_cycles = (neura_tile->get_gcp())->MIN_CYCLES;
	while (sim_end == false || min_cycles > 0)
	{
		sim_end = neura_tile->tick();
		min_cycles--;
	}
	cout << GREEN_START << "Simulation Ended" << COLOR_RESET << endl;
	neura_tile->get_gcp()->end_sim_time();
	return SUCCESS;
}

void statistics(NeuraTile *neura_tile)
{
	// neura_tile->statistics();
	// cout << "Stats" << endl;
	neura_tile->get_gcp()->statistics();
	// cout << "Smart Stats" << endl;
	neura_tile->smart_stats();
	// cout << "Smart Stats End" << endl;
}

bool sanity_check(NeuraTile *neura_tile)
{
	bool sanity = true;
	sanity = sanity && neura_tile->sanity_check();
	return sanity;
}

void sim_end_printer(bool b)
{
	if (!b)
	{
		// If sim_end is false, continue
		cout << GREEN_START << "Continue" << COLOR_RESET;
	}
	else
	{
		// If sim_end is true, halt
		cout << RED_START << "Halt" << COLOR_RESET;
	}
}

void post_sim(NeuraTile *neura_tile)
{
	// cout << "Post Simulation Metrics" << endl;
	neura_tile->post_sim_metrics();

	// cout << "Update UI_DB" << endl;
	// Update UI_DB
	Mongo *ui_db;
	ui_db = new Mongo("UI_DB");
	mongocxx::collection ui_collection;
	ui_collection = ui_db->get_component_collection("UI_COLLECTION");
	ui_collection.update_one(make_document(kvp("exp_name", neura_tile->get_gcp()->get_exp_name())),
							 make_document(kvp("$set", make_document(kvp("completed", 1)))));
	GlobalCounters *gcp = neura_tile->get_gcp();
	string experiment_name = gcp->get_exp_name();
	reset_results_dir_permissions(experiment_name);

}

void reset_results_dir_permissions(string experiment_name) {
	string cmd = "chmod -R 777 /home/ktb/git/NeuraChip/results_neurachip/" + experiment_name;
	system(cmd.c_str());
}

int address_to_mc_id(GlobalCounters *gcp, int address)
{
	int *mc_id_array = gcp->get_mc_id_array();
	int mc_id = mc_id_array[(address / 4) % gcp->get_total_mcs()];
	return mc_id;
}

void sim_cleanup(NeuraTile *neura_tile)
{
	cout << "Cleaning up simulation" << endl;
	// Skipping simulation cleanup.
	// delete neura_tile;
	// Print Results Button
	// <a href="https://neurachip.us/pages/deploy/results.php?exp_name='. $experiment_name . '"> <button type="button" class="btn btn-inverse-success btn-icon-text btn-lg"> <i class="mdi mdi-rocket btn-icon-prepend"></i> Results
	string exp_name = neura_tile->get_gcp()->get_exp_name();
	string href_link = "https://neurachip.us/pages/deploy/results.php?exp_name=" + exp_name;
	string button = "<a href=\"" + href_link + "\"> <button type=\"button\" class=\"btn btn-inverse-success btn-icon-text btn-lg\"> <i class=\"mdi mdi-rocket btn-icon-prepend\"></i> Results";
	string preprend_text = "<div class=\"col-12 grid-margin\">  <div class=\"card\"> <div class=\"card-body\"> <div id=\"sim_completed_last\" class=\"text-center\"> ";
	string append_text = " </div> </div> </div> </div>";
	string html = preprend_text + button + append_text;
	cout << html << endl;

}

void greetings()
{
	// ASCII Art for NeuraSim
	// Font: Big Money-ne
	/*
	 /$$   /$$                                         /$$$$$$  /$$
	| $$$ | $$                                        /$$__  $$|__/
	| $$$$| $$  /$$$$$$  /$$   /$$  /$$$$$$  /$$$$$$ | $$  \__/ /$$ /$$$$$$/$$$$
	| $$ $$ $$ /$$__  $$| $$  | $$ /$$__  $$|____  $$|  $$$$$$ | $$| $$_  $$_  $$
	| $$  $$$$| $$$$$$$$| $$  | $$| $$  \__/ /$$$$$$$ \____  $$| $$| $$ \ $$ \ $$
	| $$\  $$$| $$_____/| $$  | $$| $$      /$$__  $$ /$$  \ $$| $$| $$ | $$ | $$
	| $$ \  $$|  $$$$$$$|  $$$$$$/| $$     |  $$$$$$$|  $$$$$$/| $$| $$ | $$ | $$
	|__/  \__/ \_______/ \______/ |__/      \_______/ \______/ |__/|__/ |__/ |__/

	  | ======================== N E U R A - C H I P = L A B S ======================== |
	*/
	// cout << endl << endl;
	// cout << " /" GREEN($$) "   /" GREEN($$) "                                         /" RED($$$$$$) "  /" RED($$) "              " << endl;
	// cout << "| " GREEN($$$) " | " GREEN($$) "                                        /" RED($$) "__  " RED($$) "|__/              " << endl;
	// cout << "| " GREEN($$$$) "| " GREEN($$) "  /" GREEN($$$$$$) "  /" GREEN($$) "   /" GREEN($$) "  /" GREEN($$$$$$) "  /" GREEN($$$$$$) " | " RED($$) "  \\__/ /" RED($$) " /" RED($$$$$$) "/" RED($$$$) " " << endl;
	// cout << "| " GREEN($$) " " GREEN($$) " " GREEN($$) " /" GREEN($$) "__  " GREEN($$) "| " GREEN($$) "  | " GREEN($$) " /" GREEN($$) "__  " GREEN($$) "|____  " GREEN($$) "|  " RED($$$$$$) " | " RED($$) "| " RED($$) "_  " RED($$) "_  " RED($$) "" << endl;
	// cout << "| " GREEN($$) "  " GREEN($$$$) "| " GREEN($$$$$$$$) "| " GREEN($$) "  | " GREEN($$) "| " GREEN($$) "  \\__/ /" GREEN($$$$$$$) " \\____  " RED($$) "| " RED($$) "| " RED($$) " \\ " RED($$) " \\ " RED($$) "" << endl;
	// cout << "| " GREEN($$) "\\  " GREEN($$$) "| " GREEN($$) "_____/| " GREEN($$) "  | " GREEN($$) "| " GREEN($$) "      /" GREEN($$) "__  " GREEN($$) " /" RED($$) "  \\ " RED($$) "| " RED($$) "| " RED($$) " | " RED($$) " | " RED($$) "" << endl;
	// cout << "| " GREEN($$) " \\  " GREEN($$) "|  " GREEN($$$$$$$) "|  " GREEN($$$$$$) "/| " GREEN($$) "     |  " GREEN($$$$$$$) "|  " RED($$$$$$) "/| " RED($$) "| " RED($$) " | " RED($$) " | " RED($$) "" << endl;
	// cout << "|__/  \\__/ \\_______/ \\______/ |__/      \\_______/ \\______/ |__/|__/ |__/ |__/" << endl;
	
	cout << endl;
	cout << "<h1 class=\"display-3\">";
	cout << RED_START << "Neura" << COLOR_RESET;
	cout << GREEN_START << "Chip" << COLOR_RESET;
	cout << " Labs";
	cout << " </h1>" << endl;
	cout << endl << endl;
}

void neuraviz_greetings()
{
	/*

	 /$$   /$$                                        /$$    /$$ /$$          
	| $$$ | $$                                       | $$   | $$|__/          
	| $$$$| $$  /$$$$$$  /$$   /$$  /$$$$$$  /$$$$$$ | $$   | $$ /$$ /$$$$$$$$
	| $$ $$ $$ /$$__  $$| $$  | $$ /$$__  $$|____  $$|  $$ / $$/| $$|____ /$$/
	| $$  $$$$| $$$$$$$$| $$  | $$| $$  \__/ /$$$$$$$ \  $$ $$/ | $$   /$$$$/ 
	| $$\  $$$| $$_____/| $$  | $$| $$      /$$__  $$  \  $$$/  | $$  /$$__/  
	| $$ \  $$|  $$$$$$$|  $$$$$$/| $$     |  $$$$$$$   \  $/   | $$ /$$$$$$$$
	|__/  \__/ \_______/ \______/ |__/      \_______/    \_/    |__/|________/
                                                                      
	*/
	// cout << endl
	// 	 << endl;
	// cout << " /" YELLOW($$) "   /" YELLOW($$) "                                        /" CYAN($$) "    /" CYAN($$) " /" CYAN($$) "          " << endl;
	// cout << "| " YELLOW($$$) " | " YELLOW($$) "                                       | " CYAN($$) "   | " CYAN($$) "|__/          " << endl;
	// cout << "| " YELLOW($$$$) "| " YELLOW($$) "  /" YELLOW($$$$$$) "  /" YELLOW($$) "   /" YELLOW($$) "  /" YELLOW($$$$$$) "  /" YELLOW($$$$$$) " | " CYAN($$) "   | " CYAN($$) " /" CYAN($$) " /" CYAN($$$$$$$$) "" << endl;
	// cout << "| " YELLOW($$) " " YELLOW($$) " " YELLOW($$) " /" YELLOW($$) "__  " YELLOW($$) "| " YELLOW($$) "  | " YELLOW($$) " /" YELLOW($$) "__  " YELLOW($$) "|____  " YELLOW($$) "|  " CYAN($$) " / " CYAN($$) "/| " CYAN($$) "|____ /" CYAN($$) "/" << endl;
	// cout << "| " YELLOW($$) "  " YELLOW($$$$) "| " YELLOW($$$$$$$$) "| " YELLOW($$) "  | " YELLOW($$) "| " YELLOW($$) "  \\__/ /" YELLOW($$$$$$$) " \\  " CYAN($$) " " CYAN($$) "/ | " CYAN($$) "   /" CYAN($$$$) "/ " << endl;
	// cout << "| " YELLOW($$) "\\  " YELLOW($$) "$| " YELLOW($$) "_____/| " YELLOW($$) "  | " YELLOW($$) "| " YELLOW($$) "      /" YELLOW($$) "__  " YELLOW($$) "  \\  " CYAN($$$) "/  | " CYAN($$) "  /" CYAN($$) "__/  " << endl;
	// cout << "| " YELLOW($$) " \\  " YELLOW($$) "|  " YELLOW($$$$$$$) "|  " YELLOW($$$$$$) "/| " YELLOW($$) "     |  " YELLOW($$$$$$$) "   \\  " CYAN($) "/   | " CYAN($$) " /" CYAN($$$$$$$$) "" << endl;
	// cout << "|__/  \\__/ \\_______/ \\______/ |__/      \\_______/    \\_/    |__/|________/" << endl;
	// cout << endl;
	// cout << "  | ======================== " WHITE(N E U R A - C H I P) " = " WHITE(L A B S) " ======================== |  " << endl;
	// cout << endl << endl;
                                                                          
                                                                          
                                                                          
}

int packet_routing(int dest_uid, GlobalCounters *gcp, vector<int> *neighbors_uid, map<int, int> *uid_to_port_map)
{
	// FIXME: Pending implementation
	// Get the destination unit's vertex descriptor
	#ifdef TRACE_INSTX
	cout << YELLOW_START << "PACKET ROUTING" << COLOR_RESET << endl;
	cout << "Destination: " << gcp->uid_to_vertex_name_map[dest_uid] << endl;
	#endif
	auto dest_unit_vertex_desc = gcp->vertex_uid_to_desc_map[dest_uid];

	// cout << "Dest Unit Vertex Desc: " << dest_unit_vertex_desc << endl;

	// this->neighbors_uid is a vector of all the neighbors of this unit
	// Print neighbors UID size
	// cout << "Neighbors UID Size: " << neighbors_uid->size() << endl;
	// cout << "Neighbors UID: ";
	// for (auto neighbor_uid : *neighbors_uid)
	// {
	// 	cout << neighbor_uid << " ";
	// }
	// cout << endl;

	vector<int> neighbor_distances_to_dest;
	// Iterate over all the neighbors uids of this unit
	// cout << "Iterating over all the neighbors of this unit" << endl;
	for (auto neighbor_uid : *neighbors_uid)
	{
		// cout << "Neighbor UID: " << neighbor_uid << endl;
		// Get the arch_graph_distances vector of the neighbor
		auto neighbor_distances_ptr = gcp->uid_to_arch_graph_distances_ptr_map[neighbor_uid];
		auto neighbor_distance_to_dest = (*neighbor_distances_ptr)[dest_unit_vertex_desc];
		// cout << "Neighbor Distance to Dest: " << neighbor_distance_to_dest << endl;
		neighbor_distances_to_dest.push_back(neighbor_distance_to_dest);
	}

	// cout << "Neighbor Distances to Dest: ";
	// for (auto neighbor_distance_to_dest : neighbor_distances_to_dest)
	// {
	// 	cout << neighbor_distance_to_dest << " ";
	// }
	// cout << endl;

	// This is X-Y routing
	// Find the port with the minimum distance to the destination
	// TODO: Use neighbor_distances_to_dest to do adaptive routing
	int min_distance = INT_MAX;
	int min_distance_port = -1;
	int min_distance_uid = -1;
	for (int i = 0; i < neighbor_distances_to_dest.size(); i++)
	{
		if (neighbor_distances_to_dest[i] < min_distance)
		{
			min_distance = neighbor_distances_to_dest[i];
			min_distance_port = (*uid_to_port_map)[(*neighbors_uid)[i]];
			min_distance_uid = (*neighbors_uid)[i];
		}
	}

	// cout << "Min Distance: " << min_distance << endl;
	// cout << "Min Distance Port: " << min_distance_port << endl;
	#ifdef TRACE_INSTX
	cout << "Sending packet to unit: " << GREEN_START << gcp->uid_to_vertex_name_map[min_distance_uid] << COLOR_RESET << endl;
	#endif
	// FIXME: Get rid of this since this is run every single cycle
	if (min_distance_port == -1)
	{
		cout << FATAL_ERROR << "Port directing packet to min distance not found" << endl;
		e_print(1);
		exit(1);
	}
	// cout << "*************** X X X X X ****************" << endl;
	return (min_distance_port);
}

void arch_distancing(	GlobalCounters *gcp,
						vector<int>& arch_graph_distances, 
						string vertex_name, 
						int uid, 
						vector<int>& neighbors_desc, 
						vector<int>& neighbors_uid)
{
	int total_vertices = boost::num_vertices(gcp->arch_graph);
	arch_graph_distances.resize(total_vertices);
	// cout << "Vertex name: " << this->vertex_name << endl;
	boost::dijkstra_shortest_paths(gcp->arch_graph,
								   gcp->vertex_name_to_desc_map[vertex_name],
								   boost::distance_map(&arch_graph_distances[0]));
	// cout << "During arch distances size: " << arch_graph_distances.size() << endl;

	// Print arch_graph_distances
	// cout << "Total Vertices: " << total_vertices << endl;
	// cout << "Arch Graph Distances: ";
	// for (auto distance : arch_graph_distances)
	// {
	// 	cout << distance << " ";
	// }
	// cout << endl;

	gcp->uid_to_arch_graph_distances_ptr_map[uid] = &arch_graph_distances;

	// Source vertex descriptor
	auto src_unit_vertex_desc = gcp->vertex_uid_to_desc_map[uid];

	// Find the neighbors of this vertex
	auto neighbors_iter_pair = boost::adjacent_vertices(src_unit_vertex_desc, gcp->arch_graph);
	for (auto iter = neighbors_iter_pair.first; iter != neighbors_iter_pair.second; iter++)
	{
		// BUG: OLD BUG In neighbors_desc, we are storing the vertex descriptors of the neighbors, not the uids
		// this->neighbors_desc.push_back(this->gcp->vertex_desc_to_uid_map[*iter]);
		neighbors_desc.push_back(*iter);
	}

	// Create neighbor_uid vector
	// cout << "Creating Core: " << vertex_name << " Neighbors: ";
	for (auto iter = neighbors_desc.begin(); iter != neighbors_desc.end(); iter++)
	{
		neighbors_uid.push_back(gcp->vertex_desc_to_uid_map[*iter]);
		// cout << gcp->vertex_desc_to_uid_map[*iter] << " ";
	}
	// cout << endl;

	// if (this->id == 0)
	// {
	// 	// Print vertex_desc_to_uid_map
	// 	cout << "Vertex Desc to UID Map: ";
	// 	for (auto iter = gcp->vertex_desc_to_uid_map.begin(); iter != gcp->vertex_desc_to_uid_map.end(); iter++)
	// 	{
	// 		cout << iter->first << ": " << iter->second << endl;
	// 	}
	// }
}

void launch_visualizer_script(NeuraTile *neura_tile)
{
	// neuraviz_greetings();
	// Launch the visualizer script
	// cout << "Launching Visualizer Script" << endl;
	// string env_command = "source /home/ktb/miniforge3/bin/activate ktb_env; ";
	string cd_command = "cd /home/ktb/git/neurachip_deploy/NeuraViz; ";
	string viz_script = "/home/ktb/git/neurachip_deploy/NeuraViz/neuraviz_run.py";
	string python_command = "/home/ktb/miniforge3/envs/neurachip/bin/python ";

	string visualizer_script_command = cd_command + " " + python_command + " ";
	visualizer_script_command += viz_script + " ";
	visualizer_script_command += neura_tile->get_gcp()->get_exp_name() + " ";
	visualizer_script_command += to_string(neura_tile->get_gcp()->NEURA_TILE_SIZE);
	cout << "Visualizer Script Command: " << visualizer_script_command << endl;
	system(visualizer_script_command.c_str());
}

void host_notify(ArgParse *arg_parse)
{
	// For SPAM protection
	
	string subject = "NeuraChip: " + arg_parse->get_exp_name();
	string message = "Experiment Name: " + arg_parse->get_exp_name() + "\n";
	message += "Description: " + arg_parse->get_description() + "\n";
	message += "Dataset: " + arg_parse->get_dataset() + "\n";
	message += "Tile Size: " + to_string(arg_parse->get_tile_size()) + "\n";
	message += "Cache Lines Count: " + to_string(arg_parse->get_cache_lines_count()) + "\n";
	message += "Pipeline Count: " + to_string(arg_parse->get_pipeline_count()) + "\n";
	message += "Comparator Count: " + to_string(arg_parse->get_comparator_count()) + "\n";
	message += "Stride: " + to_string(arg_parse->get_stride()) + "\n";
	message += "Program Root: " + arg_parse->get_program_root() + "\n";
	message += "Dataset Root: " + arg_parse->get_dataset_root() + "\n";
	message += "Date: " + get_date() + "\n";

	string host_email = "mail@kaustubh.us";
	string email_command = "echo \"" + message + "\" | mail -s \"" + subject + "\" \"" + host_email + "\"";
	// cout << "Email Command: " << email_command << endl;
	system(email_command.c_str());
}
