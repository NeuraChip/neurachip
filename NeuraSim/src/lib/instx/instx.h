#ifndef INSTX_H
#define INSTX_H

#include <iostream>
#include "../counters/globalcounters.h"
#include "../config.h"
// #include "../memreq/memrequest.h" // Do not add derived classes in base class

using namespace std;

// Base class for all instructions
// A single instruction
class InstX {
	public:
		InstX();
		~InstX();
		// virtual long long get_id() = 0;
		// virtual void print() = 0;
		// virtual long long get_id() = 0;
		// virtual bool tick() = 0;
		// virtual int get_src_unit_uid() = 0;
		// virtual int get_dest_unit_uid() = 0;
		// virtual int get_opcode() = 0;
		// virtual int get_src_address() = 0;
		// virtual int get_dest_address() = 0;
		// virtual int get_size() = 0;
		// virtual int get_payload() = 0;
		int type;
		int get_type();
		int *dest_unit_uid_ptr;
	protected:
		// int instx_type;
};

#endif // !INSTX_H
