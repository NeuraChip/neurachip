// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "mongo.h"
#include "../debugger/debugger.h"

using namespace std;

Mongo::Mongo(string experiment_name) {
	this->experiment_name = experiment_name;
	// Only has to be done once.
	// Now being done in Sim Init
	// this->instance = new mongocxx::instance{};
	this->uri = new mongocxx::uri("mongodb://localhost:27017");
	this->client = new mongocxx::client(*this->uri);
	// Create a new database called experiment_name
	this->db = (*this->client)[experiment_name];
	// Drop all collections in the database
	// Since mongo is being used for UI_DB, we don't want to drop the database
	// this->db.drop();
	// mongocxx::instance instance{}; // This should be done only once.
	// mongocxx::uri uri("mongodb://localhost:27017");
	// mongocxx::client client(uri);
	// // Create a new database called experiment_name
	// this->db = client[experiment_name];
	// // Drop all collections in the database
	// this->db.drop();
	// Create a new collection called GlobalCounters
	// this->db.create_collection("GlobalCounters");
	// mongocxx::collection collection = this->db["GlobalCounters"];
	// collection.insert_one(make_document(kvp("Cycles", 500)));

}

Mongo::~Mongo() {
	// delete this->instance;
	delete this->uri;
	delete this->client;
}

void Mongo::drop() {
	// Drop all collections in the database
	this->db.drop();
}

bool Mongo::sanity_check() {
	return true;
}

mongocxx::collection Mongo::get_component_collection(string component_name) {
	// this->db.create_collection(component_name);
	return this->db[component_name];
}

// void Mongo::add_document(string collection_name, bsoncxx::v_noabi::document::value doc) {
// 	// this->db[collection].insert_one(doc);
// 	mongocxx::collection collection = this->db[collection_name];
// 	// collection.insert_one(make_document(kvp(key, value), kvp("Cycles", 500)));
// 	collection.insert_one(doc);
// }

void Mongo::remove_document(string collection_name, string key, string value) {
	mongocxx::collection collection = this->db[collection_name];
	collection.delete_one(make_document(kvp(key, value)));
}

