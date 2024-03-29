# Import modules
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import os
from utils import *

def plot_hbm_profile(db, fig_root_dir):
	collection_name_filter = 'H_MemoryController'
	collection_names = db.list_collection_names()
	component_names = []
	for collection_name in collection_names:
		if collection_name.startswith(collection_name_filter):
			component_names.append(collection_name)
	# Sort the component names
	component_names.sort()
	# Create plot names
	plot_names = []
	# Replace the word "H_Neura" with nothing (if it exists) and replace underscores with spaces
	for component_name in component_names:
		plot_names.append(component_name.replace('H_Neura', '').replace('_', ' '))
	
	# Get the total cycles
	total_cycles = get_total_cycles(db)

	component_busy_cycles = []

	for component_name in component_names:
		# Get the component collection
		component_collection = db[component_name]
		# Get first document
		document = component_collection.find_one()

		# Get the component busy cycles
		component_busy_cycles.append(document['busy_cycles'])
	
	component_busy_percentages = np.array(component_busy_cycles) / total_cycles * 100

	# Create stall_profiles directory if it doesn't exist in fig_root_dir
	stall_profiles_dir = fig_root_dir + '/stall_profiles_overall'
	if not os.path.exists(stall_profiles_dir):
		os.makedirs(stall_profiles_dir)

	# Plot a bar chart of the component busy cycles
	plt.figure()
	plt.bar(plot_names, component_busy_percentages)
	plt.title(component_type + ' Busy Cycles')
	plt.xlabel(component_type)
	plt.ylabel('Percentage of Total Cycles')
	plt.xticks(rotation=90)
	plt.tight_layout()
	plt.savefig(stall_profiles_dir + '/' + component_type + '_busy_cycles.png', dpi=300)
	plt.close()