#ifndef MEM_RESPONSE_H
#define MEM_RESPONSE_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include "../memreq/memrequest.h"
#include "../hashreq/hashrequest.h"

using namespace std;

// A single memory response
class MemResponse: public InstX {
	public:
		// Constructor
		MemResponse(	GlobalCounters *gcp,
						int src_unit_uid,
						int dest_unit_uid,
						int resp_type,
						int src_address,
						int size,
						int payload,
						long long src_mem_req_id,
						MemRequest *src_mem_req,
						HashRequest *src_hash_req);
		~MemResponse();
		// Source memory request ID corresponding to this response
		long long src_mem_req_id;
		// Source memory request corresponding to this response
		MemRequest *src_mem_req;
		// Source hash request corresponding to this response
		HashRequest *src_hash_req;
		// Payload
		int payload;
		void set_operand_id(int operand_id);
		int get_operand_id();
		void print();
		string resp_type_in_text();
		string operand_id_in_text();
		int get_dest_unit_uid();
		int get_src_unit_uid();
		long long get_src_address();
		int get_payload();
		long long get_id();
	private:
		// Memory Response ID
		long long id;
		// Source Address
		long long src_address;
		// Size in bytes
		long long size;
		// Global Counters
		GlobalCounters *gcp;
		// Response Type
		int resp_type;
		// Routing information
		// Source unit uid
		int src_unit_uid;
		// Destination unit uid
		int dest_unit_uid;
		// Operand ID (1 = A_data_addr, 2 = B_data_addr, 3 = B_col_id)
		int operand_id;

};

#endif // !MEM_RESPONSE_H