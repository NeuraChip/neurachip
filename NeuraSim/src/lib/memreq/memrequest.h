#ifndef MEM_REQUEST_H
#define MEM_REQUEST_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../instx/instx.h"
#include "../hashreq/hashrequest.h"

using namespace std;

// A single memory request
class MemRequest: public InstX {
	public:
		// Constructor
		// @param id: Memory Request ID
		MemRequest(	GlobalCounters *gcp,
					int src_unit_uid,
					int dest_unit_uid,
					int req_type,
					int src_address,
					int dest_address,
					int size,
					int payload,
					HashRequest *src_hash_req);
		~MemRequest();
		void print();
		long long get_id();
		bool tick();
		int get_src_unit_uid();
		int get_dest_unit_uid();
		int get_req_type();
		void set_payload(int payload);
		int get_payload();
		long long get_src_address();
		long long get_dest_address();
		long long get_size();
		HashRequest *get_src_hash_req();
		void set_operand_id(int operand_id);
		int get_operand_id();
		long long get_address();
		bool get_is_read();
		bool get_is_write();
		// Memory Request ID
		long long id;
	private:
		// Address to read and write from
		long long src_address;
		// Seconds address to move data to
		long long dest_address;
		// Size in bytes
		long long size;
		// Payload
		int payload;
		// Global Counters
		GlobalCounters *gcp;
		// Request Type
		int req_type;
		// DRAM processing time
		long long dram_processing_time;
		// Routing information
		// Source unit uid
		int src_unit_uid;
		// Destination unit uid
		int dest_unit_uid;
		// Source hash request corresponding to this response
		HashRequest *src_hash_req;
		// Operand ID (1 = A_data_addr, 2 = B_data_addr, 3 = B_col_id)
		int operand_id;

};


#endif // !MEM_REQUEST_H
