#ifndef HASHPAD_H
#define HASHPAD_H

#include <iostream>
#include <map>
#include "../hashreq/hashrequest.h"
#include "../hashline/hashline.h"


using namespace std;

class HashPad {
	public:
		HashPad();
		HashPad(size_t hash_table_size, long long table_id);
		~HashPad();
		bool sanity_check();
		bool is_data_in_hash_table(long long);
		long long get_table_id();
		long long hit_routine(HashRequest *hash_req);
		long long miss_routine(HashRequest *hash_req);
		bool try_hash_insert(long long col_id);
		long long hash_insert(long long col_id);
		size_t hashing_index(long long col_id);
		size_t linear_probing(size_t index);
		void custom_constructor(size_t hash_table_size, long long table_id);
		void decrement_count_map(long long col_id);
		long long get_tag_match_collision_count(long long col_id);
		void accumulate_data(long long col_id, int data);
		bool is_hash_pad_empty();
		void evict_hash_line(long long col_id);
		bool check_eviction_condition(long long col_id);
		int get_current_hash_lines();
		void statistics();
		bool is_empty;
	private:
	// Map of Column IDs to Hash Requests
	map <long long, HashLine *> hash_line_map;
	long long *hash_table;
	size_t hash_table_size;
	long long table_id;
	// Map of Column IDs to Counts
	map <long long, long long> count_map;
	// int single_insert_collision_count;
};








#endif // !HASHPAD_H