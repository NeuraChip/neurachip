#  /$$   /$$                                        /$$    /$$ /$$          
# | $$$ | $$                                       | $$   | $$|__/          
# | $$$$| $$  /$$$$$$  /$$   /$$  /$$$$$$  /$$$$$$ | $$   | $$ /$$ /$$$$$$$$
# | $$ $$ $$ /$$__  $$| $$  | $$ /$$__  $$|____  $$|  $$ / $$/| $$|____ /$$/
# | $$  $$$$| $$$$$$$$| $$  | $$| $$  \__/ /$$$$$$$ \  $$ $$/ | $$   /$$$$/ 
# | $$\  $$$| $$_____/| $$  | $$| $$      /$$__  $$  \  $$$/  | $$  /$$__/  
# | $$ \  $$|  $$$$$$$|  $$$$$$/| $$     |  $$$$$$$   \  $/   | $$ /$$$$$$$$
# |__/  \__/ \_______/ \______/ |__/      \_______/    \_/    |__/|________/
                                                                          

# Imports
import importlib
import numpy as np
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots
# Import mongoDB
from pymongo import MongoClient
import sys
# Import custom modules
import utils
import stall_profiles

# Custom imports
from utils import *
from stall_profiles import *
from tile_gen import *
from core_plots import *
from mc_plots import *

# Reload modules multiple times because of Jupyter Notebook
# for i in range(3):
# 	importlib.reload(utils)
# 	importlib.reload(stall_profiles)





def main():
	args = sys.argv[1:]
	# Connect to mongoDB
	client = MongoClient('localhost', 27017)
	EXP_NAME = args[0]
	TILE_DIM = int(args[1])
	db = client[EXP_NAME]
	# # Get collection names
	# collection_names = db.list_collection_names()
	# # Get collection
	# collection = db[collection_names[0]]
	# # Get data
	# data = pd.DataFrame(list(collection.find()))

	# Figures root directory
	FIG_ROOT_DIR = 'results_neurachip/' + EXP_NAME + '/figs/'


	# Print which linux user is running the script
	# print("Running script as user: ", os.getlogin())


	# Generate tile visualization
	print("Generating tile visulaization")
	generate_tile_viz(EXP_NAME, TILE_DIM, db)


	component_names = ["NeuraCore", "NeuraMem", "NeuraRouter", "Writer", "Conn", "MemoryController"]
	for component_name in component_names:
		print("Plotting overall stall profile for: ", component_name)
		plot_component_overall_stall_profile(db, component_name, FIG_ROOT_DIR)


	total_cycles = get_total_cycles(db)
	print("Total Cycles: ", total_cycles)
	total_intervals = get_total_intervals(db)
	print("Total Intervals: ", total_intervals)
	total_cores = get_total_cores(db)
	print("Total Cores: ", total_cores)
	total_mems = get_total_mems(db)
	print("Total Mems: ", total_mems)
	total_routers = get_total_routers(db)
	print("Total Routers: ", total_routers)
	total_writers = get_total_writers(db)
	print("Total Writers: ", total_writers)
	total_conns = get_total_conns(db)
	print("Total Conns: ", total_conns)



	# component_names = ["NeuraCore", "NeuraMem", "NeuraRouter", "Writer", "Conn", "MemoryController"]
	component_names = ["NeuraCore", "NeuraMem", "NeuraRouter", "Writer", "MemoryController"]
	for component_name in component_names:
		print("Plotting overall stall profile for: ", component_name)
		plot_component_iterative_stall_profile(db, component_name, FIG_ROOT_DIR)

	# Plot RF Histogram
	print("Plotting RF Histogram")
	plot_rf_histogram(EXP_NAME)

	# Plot MC Access Profile
	print("Plotting MC Access Profile")
	accessed_df = pd.read_csv('results_neurachip/' + EXP_NAME + '/mc_accessed_0.csv', sep=',')
	plot_mc_access_profile(accessed_df, FIG_ROOT_DIR)
	print("Plotting MC Rejected Access Profile")
	rejected_accessed_df = pd.read_csv('results_neurachip/' + EXP_NAME + '/rejected_mc_accessed_0.csv', sep=',')
	plot_mc_rejected_access_profile(rejected_accessed_df, FIG_ROOT_DIR)
	print("Plotting MC Stall Profile")
	plot_mc_profile(db, FIG_ROOT_DIR)
	print("Plotting MC Avg In Flight Requests")
	plot_iterative_in_flight_transactions(db, EXP_NAME)



if __name__ == '__main__':
	main()



