#ifndef HASH_RESP_H
#define HASH_RESP_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include "../hashreq/hashrequest.h"

using namespace std;

// A single Hash response
class HashResponse: public InstX {
	public:
		// Constructor
		HashResponse();
		HashResponse(GlobalCounters *gcp,
					int src_unit_uid,
					int dest_unit_uid,
					long long table_id,
					long long b_col_id,
					int mult_result,
					HashRequest *src_hash_req);
		~HashResponse();
		void custom_constructor(GlobalCounters *gcp,
								int src_unit_uid,
								int dest_unit_uid,
								long long table_id,
								long long b_col_id,
								int mult_result,
								HashRequest *src_hash_req);
		int get_dest_unit_uid();
		int get_src_unit_uid();
		long long get_table_id();
		HashRequest *get_src_hash_req();
		int get_mult_result();
	private:
		// Global Counters
		GlobalCounters *gcp;
		// Source hash request corresponding to this response
		HashRequest *src_hash_req;
		// Hash Response ID
		long long id;
		// Routing information
		// Source unit uid
		int src_unit_uid;
		// Destination unit uid
		int dest_unit_uid;
		// Table ID
		long long table_id;
		// B_col_id
		long long b_col_id;
		// Mult_result
		int mult_result;
};

#endif // !HASH_RESP_H
