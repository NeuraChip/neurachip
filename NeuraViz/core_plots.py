import numpy as np
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from utils import *
from stall_profiles import *
import random

# Create a barplot
# Rotate xticks by 90 degrees
def create_barplot(df, HISTO_BIN_SIZE, EXP_NAME):
    fig = px.bar(df, x='bin_range', y='count', labels={'bin_range': 'Bin range', 'count': 'Count'}, title='Histogram of cycles spent in Register File by Hash InstX', color='count', color_continuous_scale=px.colors.sequential.Viridis)
    fig.update_layout(xaxis_tickangle=-90)
    fig.update_layout(plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white')
    fig.update_layout(
        xaxis=dict(
            tickmode='linear',
            tick0=0,
            dtick=HISTO_BIN_SIZE
        )
    )
    # fig.show()
    # Save as html
    fig.write_html('./results_neurachip/' + EXP_NAME + '/neuracore/rf_histogram.html')


def plot_rf_histogram(EXP_NAME):
    HISTO_BIN_SIZE = 100
    # Read data from directory in pandas dataframe
    df = pd.read_csv('./results_neurachip/' + EXP_NAME + '/neuracore/rf_histogram.csv', header=None)
    # Rename columns
    df.columns = ['count']
    # Add a column with bin range
    df['bin_range'] = (df.index + 1) * HISTO_BIN_SIZE

    create_barplot(df, HISTO_BIN_SIZE, EXP_NAME)
