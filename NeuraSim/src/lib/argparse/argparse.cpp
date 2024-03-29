// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "argparse.h"
#include "../debugger/debugger.h"

namespace po = boost::program_options;
using namespace std;

ArgParse::ArgParse(int argc, char *argv[])
{
	parse(argc, argv);
}

ArgParse::~ArgParse()
{
}

void ArgParse::parse(int argc, char *argv[])
{
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "produce help message")
			("exp_name,n", po::value<string>(&exp_name)->default_value("NeuraChip_V000"), "Experiment Name")
			("program_root,r", po::value<string>(&program_root)->default_value("/home/ktb/git/NeuraChip/NeuraCompiler/programs"), "Program Root File")
			("description,d", po::value<string>(&description)->default_value("Default experiment description"), "Experiment Description")
			("dataset,x", po::value<string>(&dataset)->default_value("cora"), "Dataset")
			("tile_size,t", po::value<string>(&tile_size)->default_value("16"), "Tile Size")
			("stride,s", po::value<int>(&stride)->default_value(1), "Stride")
			("cache_lines_count,c", po::value<string>(&cache_lines_count)->default_value("16384"), "Cach Lines Count")
			("pipeline_count,p", po::value<string>(&pipeline_count)->default_value("128"), "Pipeline Count")
			("comparator_count,m", po::value<string>(&comparator_count)->default_value("16"), "Comparator Count")

			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		this->dataset_root = this->program_root + "/" + this->dataset;

		if (vm.count("help")) {
			cout << desc << "\n";
			exit(0);
		}
	}
	catch (exception& e) {
		cerr << "error: " << e.what() << "\n";
		exit(1);
	}
	catch (...) {
		cerr << "Exception of unknown type!\n";
		exit(1);
	}
}

void ArgParse::print()
{
	cout << "Experiment Name: " << this->exp_name << endl;
	// cout << "Program Root: " << this->program_root << endl;
	cout << "Description: " << this->description << endl;
	cout << "Dataset: " << this->dataset << endl;
	// cout << "Dataset Root: " << this->dataset_root << endl;
	cout << "Tile Size: " << this->tile_size << endl;
	cout << "Cache Lines Count: " << this->cache_lines_count << endl;
	cout << "Pipeline Count: " << this->pipeline_count << endl;
	cout << "Comparator Count: " << this->comparator_count << endl;
}