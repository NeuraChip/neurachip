// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "hashpad.h"
#include <iostream>

using namespace std;

HashPad::HashPad()
{
	// Constructor
}

HashPad::HashPad(size_t hash_table_size, long long table_id)
{
	// Constructor
	this->custom_constructor(hash_table_size, table_id);
}

void HashPad::custom_constructor(size_t hash_table_size, long long table_id)
{
	// Custom constructor
	this->hash_table_size = hash_table_size;
	this->table_id = table_id;
	this->hash_table = new long long[this->hash_table_size];
	for (size_t i = 0; i < this->hash_table_size; i++) {
		this->hash_table[i] = -1;
	}
	this->is_empty = true;
}

HashPad::~HashPad()
{
	// Destructor
	delete[] this->hash_table;
}

bool HashPad::sanity_check()
{
	// Sanity check
	return true;
}

bool HashPad::is_data_in_hash_table(long long col_id)
{
	if (this->hash_line_map.find(col_id) != this->hash_line_map.end()) {
		return true;
	}
	else {
		return false;
	}
}

long long HashPad::get_table_id()
{
	return this->table_id;
}

// @return: Processing Delay
long long HashPad::hit_routine(HashRequest *hash_req)
{
	// Hit routine
	// Decrement the count of the column ID in the count map
	this->decrement_count_map(hash_req->get_b_col_id());
	long long collisions = this->get_tag_match_collision_count(hash_req->get_b_col_id());
	// Accumulate the data
	this->accumulate_data(hash_req->get_b_col_id(), hash_req->get_mult_result());
	return collisions;
}

void HashPad::decrement_count_map(long long col_id) {
	// Decrement the count of the column ID in the count map
	this->count_map[col_id] -= 1;
	if (this->count_map[col_id] < 0) {
		cout << "Error: Count of column ID " << col_id << " is less than 0" << endl;
		// exit(1);
	}
	// This is done by the hash eviction routine
	// if (this->count_map[col_id] == 0) {
	// 	this->count_map.erase(col_id);
	// }
	// return;
}

bool HashPad::check_eviction_condition(long long col_id) {
	// Check if the count of the column ID is 0
	if (this->count_map[col_id] <= 0) {
		return true;
	}
	else {
		return false;
	}
}

void HashPad::evict_hash_line(long long col_id) {
	// Evict the hash line from the hash line map
	this->hash_line_map.erase(col_id);
	this->is_empty = this->is_hash_pad_empty();
	// Evict the column ID from the count map
	this->count_map.erase(col_id);
	// Evict the column ID from the hash table
	this->hash_table[this->hashing_index(col_id)] = -1;
	return;
}

long long HashPad::miss_routine(HashRequest *hash_req)
{
	// Miss routine
	// Create a new hash line
	HashLine *hash_line = new HashLine(	hash_req->get_b_col_id(), 
										hash_req->get_mult_result(), 
										hash_req->get_count());
	// Add the hash request to the hash line map
	this->hash_line_map.insert(pair<long long, HashLine *>(hash_req->get_b_col_id(), hash_line));
	this->is_empty = false;
	// Insert the column ID into the hash table
	long long collisions = this->hash_insert(hash_req->get_b_col_id());
	// Add the column ID to the count map
	this->count_map.insert(pair<long long, long long>(hash_req->get_b_col_id(), hash_req->get_count()));
	this->decrement_count_map(hash_req->get_b_col_id());
	return collisions;
}

bool HashPad::try_hash_insert(long long col_id)
{
	// Try to insert the column ID into the hash table
	// If the column ID is already present, return false
	// If the column ID is not present, insert it and return true
	size_t index = this->hashing_index(col_id);
	if (this->hash_table[index] == -1) {
		this->hash_table[index] = col_id;
		return true;
	}
	else {
		return false;
	}
}

size_t HashPad::hashing_index(long long col_id)
{
	// Hashing function
	return col_id % this->hash_table_size;
}

size_t HashPad::linear_probing(size_t index)
{
	// Linear probing function
	return (index + 1) % this->hash_table_size;
}

long long HashPad::hash_insert(long long col_id)
{
	long long collision_count = 0;
	// Insert the column ID into the hash table
	size_t index = this->hashing_index(col_id);
	while (this->hash_table[index] != -1) {
		index = this->linear_probing(index);
		collision_count++;
		if (collision_count > this->hash_table_size) {
			cout << "Error: Infinite loop in hash_insert" << endl;
			exit(1);
		}
	}
	this->hash_table[index] = col_id;
	return collision_count;
}

long long HashPad::get_tag_match_collision_count(long long col_id) {
	// Get the number of collisions that occur while inserting col_id into the hash table
	// The hash table should have col_id already
	// Find the number of collisions that occur while reaching col_id in the hash table
	long long collision_count = 0;
	size_t index = this->hashing_index(col_id);
	while (this->hash_table[index] != col_id) {
		index = this->linear_probing(index);
		collision_count++;
		if (collision_count > this->hash_table_size) {
			cout << "Error: Infinite loop in get_tag_match_collision_count" << endl;
			exit(1);
		}
	}
	return collision_count;
}

void HashPad::accumulate_data(long long col_id, int data) {
	// Accumulate the data in the hash line
	this->hash_line_map[col_id]->accumulate_data(data);
	return;
}

bool HashPad::is_hash_pad_empty()
{
	// Check if the hash pad is empty
	if (this->hash_line_map.size() == 0) {
		return true;
	}
	else {
		return false;
	}
}

int HashPad::get_current_hash_lines()
{
	// Get the number of hash lines in the hash pad
	return this->hash_line_map.size();
}

void HashPad::statistics()
{
	// Print the statistics of the hash pad
	cout << "Hash pad statistics for table id: " << this->table_id << endl;
	cout << "Current hash lines: " << this->get_current_hash_lines() << endl;
	// Print a pretty table of col ID | data | count
	cout << "Col ID\t\tData\t\tCount" << endl;
	for (auto it = this->hash_line_map.begin(); it != this->hash_line_map.end(); it++) {
		HashLine *hash_line = it->second;
		long long col_id = hash_line->get_col_id();
		int data = hash_line->get_data();
		long long count = hash_line->get_count();
		cout << col_id << "\t\t" << data << "\t\t" << count << endl;
	}
	return;
}