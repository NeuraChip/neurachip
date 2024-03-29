import numpy as np
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from utils import *
from stall_profiles import *
from pymongo import MongoClient
import sys

def plot_iterative_in_flight_transactions(db, EXP_NAME):
	metric_array = get_array_of_interval_metric_from_component(db, "MemoryController_0", "interval_avg_in_flight_requests")
	x_ticks = np.arange(0, len(metric_array))
	x_ticks = x_ticks * 1000
	fig = px.line(x=x_ticks, y=metric_array, title='MemoryController_0', labels={'x': 'Cycles', 'y': 'Avg In Flight Requests'})
	fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white', title='Average In Flight Requests in Memory Controller across all channels')
	# Save to html
	# Create directory if it doesn't exist
	if not os.path.exists("./results_neurachip/" + EXP_NAME + "/mc"):
		os.makedirs("./results_neurachip/" + EXP_NAME + "/mc")
	fig.write_html("./results_neurachip/" + EXP_NAME + "/mc/avg_in_flight_requests.html")
	# fig.show()