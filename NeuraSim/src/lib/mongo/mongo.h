#ifndef MONGO_H
#define MONGO_H

#include <iostream>
#include "../debugger/debugger.h"
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>

#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/exception/exception.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace std;

class Mongo {
	public:
		Mongo(string experiment_name);
		~Mongo();
		bool sanity_check();
		mongocxx::collection get_component_collection(string component_name);
		void drop();
		void remove_document(string collection_name, string key, string value);
	private:
		// mongocxx::instance *instance; // This should be done only once.
		mongocxx::database db;
		mongocxx::uri *uri;
		mongocxx::client *client;
		string experiment_name;

};



#endif // !MONGO_H