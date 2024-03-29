#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <math.h>

using namespace std;

class ArgParse {
public:
	ArgParse(int argc, char *argv[]);
	~ArgParse();
	void parse(int argc, char *argv[]);
	void print();
	string get_exp_name() { return exp_name; }
	string get_program_root() { return program_root; }
	string get_description() { return description; }
	string get_dataset() { return dataset; }
	string get_dataset_root() { return dataset_root; }
	int get_tile_size() {
		// if tile size is 4, 16 or 64, convert to int and return square root of it in int
		if (tile_size == "4" || tile_size == "16" || tile_size == "64") {
			return (int)sqrt(stoi(tile_size));
		}
		// if tile size is Tile 4, Tile 16 or Tile 64, convert to int and return square root of it in int
		else if (tile_size == "Tile 4" || tile_size == "Tile 16" || tile_size == "Tile 64") {
			return (int)sqrt(stoi(tile_size.substr(5)));
		}
		else {
			cout << "ERROR: Invalid tile size" << endl;
			return stoi(tile_size);
		}
	}
	int get_stride() { return stride; }
	int get_cache_lines_count() { return stoi(cache_lines_count); }
	int get_pipeline_count() { return stoi(pipeline_count); }
	int get_comparator_count() { return stoi(comparator_count); }
private:
	string exp_name;
	string program_root;
	string description;
	string dataset;
	string dataset_root;
	string tile_size;
	string pipeline_count;
	int stride;
	string cache_lines_count;
	string comparator_count;

};

#endif // !ARG_PARSE_H