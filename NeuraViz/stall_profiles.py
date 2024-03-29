# Stall Profiles

# Import modules
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import plotly.express as px
import os
from utils import *


# def plot_core_busy_cycles(db, fig_root_dir):
# 	collection_names = db.list_collection_names()
# 	# Make a list of core names by selecting the ones that start with 'NeuraCore'
# 	core_names = []
# 	for collection_name in collection_names:
# 		if collection_name.startswith('H_NeuraCore'):
# 			core_names.append(collection_name)
# 	# Sort the core names
# 	core_names.sort()

# 	# Remove H_Neura and replace _ with space
# 	plot_names = []
# 	for core_name in core_names:
# 		plot_names.append(core_name[7:].replace('_', ' '))

# 	core_idle_cycles = []
# 	core_busy_cycles = []

# 	for core_name in core_names:
# 		# Get the core collection
# 		core_collection = db[core_name]
# 		# Get first document
# 		document = core_collection.find_one()

# 		# Get the core idle cycles
# 		core_idle_cycles.append(document['idle_cycles'])
# 		# Get the core busy cycles
# 		core_busy_cycles.append(document['busy_cycles'])
	
# 	total_cycles = get_total_cycles(db)
# 	core_idle_percentages = np.array(core_idle_cycles) / total_cycles * 100
# 	core_busy_percentages = np.array(core_busy_cycles) / total_cycles * 100
	
# 	# Create stall_profiles directory if it doesn't exist in fig_root_dir
# 	stall_profiles_dir = fig_root_dir + '/stall_profiles'
# 	if not os.path.exists(stall_profiles_dir):
# 		os.makedirs(stall_profiles_dir)
	
# 	# Plot a bar chart of the core idle cycles
# 	plt.figure()
# 	plt.bar(plot_names, core_idle_percentages)
# 	plt.title('Core Idle Cycles')
# 	plt.xlabel('Core')
# 	plt.ylabel('Percentage of Total Cycles')
# 	plt.xticks(rotation=90)
# 	plt.tight_layout()
# 	plt.savefig(stall_profiles_dir + '/core_idle_cycles.png')
# 	plt.close()

# 	# Plot a bar chart of the core busy cycles
# 	plt.figure()
# 	plt.bar(plot_names, core_busy_percentages)
# 	plt.title('Core Busy Cycles')
# 	plt.xlabel('Core')
# 	plt.ylabel('Percentage of Total Cycles')
# 	plt.xticks(rotation=90)
# 	plt.tight_layout()
# 	plt.savefig(stall_profiles_dir + '/core_busy_cycles.png')
# 	plt.close()

def plot_component_overall_stall_profile(db, component_type, fig_root_dir):
	valid_component_names = ["NeuraCore", "NeuraMem", "NeuraRouter", "Writer", "Conn", "MemoryController"]
	if component_type not in valid_component_names:
		raise ValueError("Invalid component type. Valid component types are: " + str(valid_component_names))
	
	collection_name_filter = 'H_' + component_type
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

	# Plot a bar chart of the component busy cycles using plotly express
	# Set the background color #191c24
	fig = px.bar(x=plot_names, y=component_busy_percentages, title=component_type + ' Busy Cycles', labels={'x': component_type, 'y': 'Percentage of Total Cycles'})
	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
	fig.update_xaxes(tickangle=90)
	fig.write_image(stall_profiles_dir + '/' + component_type + '_busy_cycles.png', width=1920, height=1080)
	fig.write_html(stall_profiles_dir + '/' + component_type + '_busy_cycles.html')


	# plt.figure()
	# plt.bar(plot_names, component_busy_percentages)
	# plt.title(component_type + ' Busy Cycles')
	# plt.xlabel(component_type)
	# plt.ylabel('Percentage of Total Cycles')
	# plt.xticks(rotation=90)
	# plt.tight_layout()
	# plt.savefig(stall_profiles_dir + '/' + component_type + '_busy_cycles.png', dpi=300)
	# plt.savefig(stall_profiles_dir + '/' + component_type + '_busy_cycles.html', dpi=300)
	
	# plt.close()

def get_array_of_interval_metric_from_component(db, component_name, metric):
	total_intervals = get_total_intervals(db)
	# Get the component collection
	component_collection = db[component_name]
	interval_metric_array = np.zeros(total_intervals)
	for document in component_collection.find():
		interval_metric_array[document["interval_id"]] = document[metric]
	return interval_metric_array


def plot_component_iterative_stall_profile(db, component_type, fig_root_dir):
	valid_component_names = ["NeuraCore", "NeuraMem", "NeuraRouter", "Writer", "Conn", "MemoryController"]
	if component_type not in valid_component_names:
		raise ValueError("Invalid component type. Valid component types are: " + str(valid_component_names))
	
	collection_name_filter = component_type
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
	total_interval_cycles = get_total_interval_cycles(db)

	component_iterative_busy_cycles_array = []

	for component_name in component_names:
		component_iterative_busy_cycles_array.append(get_array_of_interval_metric_from_component(db, component_name, "interval_busy_cycles"))
	

	component_iterative_busy_cycles_array = np.array(component_iterative_busy_cycles_array).T
	component_iterative_busy_percentage_array = np.array(component_iterative_busy_cycles_array) / total_interval_cycles * 100
	comp_df = pd.DataFrame(component_iterative_busy_percentage_array, columns=plot_names)
	# Create stall_profiles directory if it doesn't exist in fig_root_dir
	stall_profiles_dir = fig_root_dir + '/stall_profiles_iterative'
	if not os.path.exists(stall_profiles_dir):
		os.makedirs(stall_profiles_dir)

	# Plot all the component iterative busy percentage on a single line plot using seaborn
	# plt.figure()
	# sns.set_style("darkgrid")
	# for i in range(len(component_names)):
	# 	plt.plot(component_iterative_busy_percentage_array[i], label=plot_names[i])
	# plt.title(component_type + ' Iterative Busy Percentage')
	# plt.xlabel('Interval')
	# plt.ylabel('Percentage of Total Cycles')
	# plt.legend()
	# plt.tight_layout()
	# plt.savefig(stall_profiles_dir + '/' + component_type + '_iterative_busy_percentage.png', dpi=300)
	# plt.close()

	# Plot all the component iterative busy percentage on a single line plot using plotly express
	# Set the background color #191c24
	# fig = px.line(component_iterative_busy_percentage_array, title=component_type + ' Iterative Busy Percentage', labels={'x': 'Interval', 'y': 'Percentage of Total Cycles'})
	fig = px.line(comp_df, title=component_type + ' Iterative Busy Percentage', labels={'index': 'Interval', 'value': 'Percentage of Total Cycles', 'variable': 'Component'})

	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
	fig.update_xaxes(tickangle=90)
	fig.write_image(stall_profiles_dir + '/' + component_type + '_iterative_busy_percentage.png', width=1920, height=1080)
	fig.write_html(stall_profiles_dir + '/' + component_type + '_iterative_busy_percentage.html')
	# Show the plot
	# fig.show()

	

def plot_mc_access_profile(accessed_df, fig_root_dir):
	stall_profiles_dir = fig_root_dir + '/stall_profiles_overall'
	fig = px.scatter(accessed_df, x="cycle", y="address", color='is_write', hover_data=['cycle', 'address'], color_discrete_sequence=['#ff77a0', '#ff7f0e'])
	fig.update_layout(title='Memory Controller Accessed Data', xaxis_title='Cycle', yaxis_title='Address')
	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
	fig.write_image(stall_profiles_dir + '/' + 'mc_access_profile.png', width=1920, height=1080)
	fig.write_html(stall_profiles_dir + '/' + 'mc_access_profile.html')
	# Show the plot
	# fig.show()

def plot_mc_rejected_access_profile(rejected_accessed_df, fig_root_dir):
	stall_profiles_dir = fig_root_dir + '/stall_profiles_overall'
	fig = px.scatter(rejected_accessed_df, x="cycle", y="address", color='port_number', hover_data=['cycle', 'address'])
	fig.update_layout(title='Memory Controller Rejected Accessed Data', xaxis_title='Cycle', yaxis_title='Address')
	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
	fig.write_image(stall_profiles_dir + '/' + 'rejected_mc_access_profile.png', width=1920, height=1080)
	fig.write_html(stall_profiles_dir + '/' + 'rejected_mc_access_profile.html')
	# Show the plot
	# fig.show()

def plot_mc_profile(db, fig_root_dir):
	stalls_profile_dir = fig_root_dir + '/stall_profiles_overall'
	mc_df = get_overal_mc_df(db)
	port_names = ['CH 0', 'CH 1', 'CH 2', 'CH 3', 'CH 4', 'CH 5', 'CH 6', 'CH 7', 'Writer 0', 'Writer 1']
	col_names = ['stalls_ch_0', 'stalls_ch_1', 'stalls_ch_2', 'stalls_ch_3', 'stalls_ch_4', 'stalls_ch_5', 'stalls_ch_6', 'stalls_ch_7', 'stalls_writer_0', 'stalls_writer_1']
	dat_array = np.zeros((1, len(col_names)))
	for i in range(len(col_names)):
		dat_array[0, i] = mc_df[col_names[i]].sum()
	fig = px.bar(x=port_names, y=dat_array[0], title='Memory Controller Stalls', labels={'x': 'Port', 'y': 'Stalls'})
	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
	fig.update_xaxes(tickangle=90)
	fig.write_image(stalls_profile_dir + '/mc_stalls.png', width=1920, height=1080)
	fig.write_html(stalls_profile_dir + '/mc_stalls.html')
	# Show the plot
	# fig.show()

