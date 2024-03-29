import pandas as pd


# Functions

def get_total_cores(db):
	collection_names = db.list_collection_names()
	# Count the number of elements that start with 'NeuraCore'
	total_cores = 0
	for collection_name in collection_names:
		if collection_name.startswith('NeuraCore'):
			total_cores += 1
	return total_cores

def get_total_mems(db):
	collection_names = db.list_collection_names()
	# Count the number of elements that start with 'NeuraMem'
	total_mems = 0
	for collection_name in collection_names:
		if collection_name.startswith('NeuraMem'):
			total_mems += 1
	return total_mems

def get_total_routers(db):
	collection_names = db.list_collection_names()
	# Count the number of elements that start with 'NeuraRouter'
	total_routers = 0
	for collection_name in collection_names:
		if collection_name.startswith('NeuraRouter'):
			total_routers += 1
	return total_routers

def get_total_writers(db):
	collection_names = db.list_collection_names()
	# Count the number of elements that start with 'Writer'
	total_writers = 0
	for collection_name in collection_names:
		if collection_name.startswith('Writer'):
			total_writers += 1
	return total_writers

def get_total_conns(db):
	collection_names = db.list_collection_names()
	# Count the number of elements that start with 'Conn'
	total_conns = 0
	for collection_name in collection_names:
		if collection_name.startswith('Conn'):
			total_conns += 1
	return total_conns

def get_total_cycles(db):
	gcp_collection = db['H_GlobalCounters']
	document = gcp_collection.find_one()
	return document['cycles']

def get_total_intervals(db):
	gcp_collection = db['GlobalCounters']
	# Count the number of documents in the collection
	total_intervals = gcp_collection.count_documents({})
	return total_intervals

def get_total_interval_cycles(db):
	gcp_collection = db['GlobalCounters']
	document = gcp_collection.find_one()
	return document['interval_cycles']

def get_overal_routers_df(db):
	collection_names = db.list_collection_names()
	# Get all router collections
	router_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_NeuraRouter'):
			router_collections.append(collection_name)
	# Get all router data
	router_data = []
	for router_collection in router_collections:
		collection = db[router_collection]
		router_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all router data
	router_df = pd.concat(router_data)
	return router_df

def get_overal_cores_df(db):
	collection_names = db.list_collection_names()
	# Get all core collections
	core_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_NeuraCore'):
			core_collections.append(collection_name)
	# Get all core data
	core_data = []
	for core_collection in core_collections:
		collection = db[core_collection]
		core_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all core data
	core_df = pd.concat(core_data)
	return core_df

def get_overal_mems_df(db):
	collection_names = db.list_collection_names()
	# Get all mem collections
	mem_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_NeuraMem'):
			mem_collections.append(collection_name)
	# Get all mem data
	mem_data = []
	for mem_collection in mem_collections:
		collection = db[mem_collection]
		mem_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all mem data
	mem_df = pd.concat(mem_data)
	return mem_df

def get_overal_writers_df(db):
	collection_names = db.list_collection_names()
	# Get all writer collections
	writer_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_Writer'):
			writer_collections.append(collection_name)
	# Get all writer data
	writer_data = []
	for writer_collection in writer_collections:
		collection = db[writer_collection]
		writer_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all writer data
	writer_df = pd.concat(writer_data)
	return writer_df

def get_overal_conns_df(db):
	collection_names = db.list_collection_names()
	# Get all conn collections
	conn_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_Conn'):
			conn_collections.append(collection_name)
	# Get all conn data
	conn_data = []
	for conn_collection in conn_collections:
		collection = db[conn_collection]
		conn_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all conn data
	conn_df = pd.concat(conn_data)
	return conn_df

def get_overal_mc_df(db):
	collection_names = db.list_collection_names()
	# Get all mc collections
	mc_collections = []
	for collection_name in collection_names:
		if collection_name.startswith('H_MemoryController'):
			mc_collections.append(collection_name)
	# Get all mc data
	mc_data = []
	for mc_collection in mc_collections:
		collection = db[mc_collection]
		mc_data.append(pd.DataFrame(list(collection.find())))
	# Concatenate all mc data
	mc_df = pd.concat(mc_data)
	return mc_df

def get_mc_attached_router_coods(db):
	conns_df = get_overal_conns_df(db)
	# Find rows with X_ and in the connected_comps column
	filtered_conns_df = conns_df[conns_df['connected_comps'].str.contains('X_')]
	# Find rows with R_ and in the filtered column
	filtered_conns_df = filtered_conns_df[filtered_conns_df['connected_comps'].str.contains('R_')]
	# Create a list of all the connected components
	connected_comps = filtered_conns_df['connected_comps'].tolist()
	# Remove "X_0 - " from the connected components
	connected_comps = [x.replace('X_0 - ', '') for x in connected_comps]
	# Remove "R_" from the connected components
	connected_comps = [x.replace('R_', '') for x in connected_comps]
	# Split each connected component into two integers (x and y) separated by a _
	connected_comps = [x.split('_') for x in connected_comps]
	mc_x_coods = [int(x[0]) for x in connected_comps]
	mc_y_coods = [int(x[1]) for x in connected_comps]
	return (mc_x_coods, mc_y_coods)

