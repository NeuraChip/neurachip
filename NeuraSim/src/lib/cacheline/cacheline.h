#ifndef CACHE_LINE_H
#define CACHE_LINE_H

#include <iostream>

using namespace std;

class CacheLine {
	public:
		CacheLine(	int payload,
					long long address,
					long long accessed_at_cycle);
		~CacheLine();
		int payload;
		long long address;
		long long accessed_at_cycle;
	private:
};

#endif // !CACHE_LINE_H