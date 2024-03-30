// Created by Kaustubh Shivdikar
//
//  (C) All Rights Reserved

#include "../lib/functions.h"
#include "../lib/debugger/debugger.h"
#include "../lib/neuracore/neuracore.h"
#include "../lib/counters/globalcounters.h"
#include "../lib/neuratile/neuratile.h"
#include "../lib/config.h"
#include "../lib/argparse/argparse.h"

using namespace std;

// Global Variables

// ************ MAIN FUNCTION ************

int main(int argc, char *argv[])
{
	ArgParse arg_parse(argc, argv);

	// For SPAM protection, comment out to disable SPAM protection.
	// host_notify(&arg_parse);

	greetings();
	// cout << "Experiment Name: " << arg_parse.get_exp_name() << endl;
	// return 0;

	// arg_parse.print();

	// Set omp threads
	// omp_set_num_threads(32);

	NeuraTile *neura_tile;
	neura_tile = sim_init(&arg_parse);

	// launch_visualizer_script(neura_tile);
	// return 0;

	sanity_check(neura_tile);

	// return 0;
	// statistics(neura_tile);
	
	run_sim(neura_tile);

	// cout << "Main Statistics" << endl;
	statistics(neura_tile);

	// cout << "Post Sim" << endl;
	post_sim(neura_tile);

	// cout << "Launch Visualizer" << endl;
	// launch_visualizer_script(neura_tile);

	// cout << "Cleanup" << endl;
	sim_cleanup(neura_tile);



	cout << "\n\e[1;31mProgram End\e[0m\n\n\n";
	return SUCCESS;
}
