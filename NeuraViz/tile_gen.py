# Imports
import importlib
import numpy as np
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go

# Import mongoDB
from pymongo import MongoClient
from utils import *
from stall_profiles import *
import random

# Functions


# Generate path arrays
def generate_path_arrays():
    ww_asset_root = "https://neurachip.us/results_neurachip/assets/ww"
    # ww_asset_root = './results_neurachip/assets/ww'
    core_paths = []
    core_paths.append(ww_asset_root + "/ww_core_0.png")
    core_paths.append(ww_asset_root + "/ww_core_1.png")
    core_paths.append(ww_asset_root + "/ww_core_2.png")
    core_paths.append(ww_asset_root + "/ww_core_3.png")
    mc_paths = []
    mc_paths.append(ww_asset_root + "/ww_mc_0.png")
    mc_paths.append(ww_asset_root + "/ww_mc_1.png")
    mc_paths.append(ww_asset_root + "/ww_mc_2.png")
    mc_paths.append(ww_asset_root + "/ww_mc_3.png")
    mem_paths = []
    mem_paths.append(ww_asset_root + "/ww_mem_0.png")
    mem_paths.append(ww_asset_root + "/ww_mem_1.png")
    mem_paths.append(ww_asset_root + "/ww_mem_2.png")
    mem_paths.append(ww_asset_root + "/ww_mem_3.png")
    router_paths = []
    router_paths.append(ww_asset_root + "/ww_router_0.png")
    router_paths.append(ww_asset_root + "/ww_router_1.png")
    router_paths.append(ww_asset_root + "/ww_router_2.png")
    router_paths.append(ww_asset_root + "/ww_router_3.png")
    writer_paths = []
    writer_paths.append(ww_asset_root + "/ww_writer_0.png")
    writer_paths.append(ww_asset_root + "/ww_writer_1.png")
    writer_paths.append(ww_asset_root + "/ww_writer_2.png")
    writer_paths.append(ww_asset_root + "/ww_writer_3.png")
    hbm_paths = []
    hbm_paths.append(ww_asset_root + "/ww_hbm.png")
    return (core_paths, mc_paths, mem_paths, router_paths, writer_paths, hbm_paths)


CORE_PATH_INDEX = 0
MC_PATH_INDEX = 1
MEM_PATH_INDEX = 2
ROUTER_PATH_INDEX = 3
WRITER_PATH_INDEX = 4
HBM_PATH_INDEX = 5

def intensity_classifier(intensity):
    # Classify the intensity into 4 classes
    # 0 - 25: Low
    # 25 - 50: Medium
    # 50 - 75: High
    # 75 - 100: Very High
    if intensity < 25:
        return 0
    elif intensity < 50:
        return 1
    elif intensity < 75:
        return 2
    else:
        return 3

def iclass(i):
    return intensity_classifier(i)

def iclass_10(i):
    return ((i/10) + 1)

def add_conn(
    x0,
    y0,
    x1,
    y1,
    fig,
    x_cood,
    y_cood,
    right_component_name,
    right_x,
    right_y,
    conns_df,
    total_cycles,
    TILE_DIM,
    del_intensity_value,
):
    hover_text, intensity_value = get_conn_hover_text_and_intensity(
    "R",
    x_cood,
    y_cood,
    right_component_name,
    right_x,
    right_y,
    conns_df,
    total_cycles,
    TILE_DIM,
    )
    intensity = iclass_10(intensity_value)
    # Line
    if intensity > 11:
        intensity = 11
    width_value = 1 + (intensity * 2)
    # Generate color based on intensity
    color = "rgba(0, 0, 0, 0`)"
    if intensity_value < 25:
        color = "rgba(0, 255, 0, 0.5)"
    elif intensity_value < 50:
        color = "rgba(255, 255, 0, 0.5)"
    elif intensity_value < 75:
        color = "rgba(255, 165, 0, 0.5)"
    else:
        color = "rgba(255, 0, 0, 0.5)"
    

    fig.add_trace(
        go.Scatter(
            x=[x0, x1],  # Assuming x,y is the top left of the image
            y=[y0, y1],
            hoverinfo="none",
            mode="lines",
            showlegend=False,
            line=dict(
                color=color,
                width=width_value,
            ),
        )
    )
    # Marker
    mid_x = (x0 + x1) / 2
    mid_y = (y0 + y1) / 2
    fig.add_trace(
        go.Scatter(
            x=[mid_x],  # Assuming x,y is the top left of the image
            y=[mid_y],
            hoverinfo="text",
            hovertext=[hover_text],
            mode="markers",
            showlegend=False,
            marker=dict(
                opacity=1,
                size=5,
            ),  # set the marker to be invisible
        )
    )


def get_core_hover_text_and_intensity(x_cood, y_cood, cores_df, total_cycles):
    # Get the core row matching x_cood and y_cood columns
    core_row = cores_df.loc[
        (cores_df["x_cood"] == x_cood) & (cores_df["y_cood"] == y_cood)
    ]
    # Get the idle cycles and busy cycles
    idle_cycles = core_row["idle_cycles"].values[0]
    busy_cycles = core_row["busy_cycles"].values[0]
    # Get the idle cycles and busy cycles as percentages (Round to 2 decimal places)
    idle_cycles_percent = round((idle_cycles / total_cycles) * 100, 2)
    busy_cycles_percent = round((busy_cycles / total_cycles) * 100, 2)
    intensity = iclass(busy_cycles_percent)
    # Get the hover text
    hover_text = (
        "NeuraCore ["
        + str(x_cood)
        + ", "
        + str(y_cood)
        + "]<br>Busy Cycles: "
        + str(busy_cycles)
        + " ("
        + str(busy_cycles_percent)
        + "%)"
        + "<br>Idle Cycles: "
        + str(idle_cycles)
        + " ("
        + str(idle_cycles_percent)
        + "%)"
    )
    return hover_text, intensity

def get_mem_hover_text_and_intensity(x_cood, y_cood, mems_df, total_cycles):
    # Get the core row matching x_cood and y_cood columns
    mem_row = mems_df.loc[
        (mems_df["x_cood"] == x_cood) & (mems_df["y_cood"] == y_cood)
    ]
    # Get the idle cycles and busy cycles
    idle_cycles = mem_row["idle_cycles"].values[0]
    busy_cycles = mem_row["busy_cycles"].values[0]
    # Get the idle cycles and busy cycles as percentages (Round to 2 decimal places)
    idle_cycles_percent = round((idle_cycles / total_cycles) * 100, 2)
    busy_cycles_percent = round((busy_cycles / total_cycles) * 100, 2)
    intensity = iclass(busy_cycles_percent)
    # Get the hover text
    hover_text = (
        "NeuraMem ["
        + str(x_cood)
        + ", "
        + str(y_cood)
        + "]<br>Busy Cycles: "
        + str(busy_cycles)
        + " ("
        + str(busy_cycles_percent)
        + "%)"
        + "<br>Idle Cycles: "
        + str(idle_cycles)
        + " ("
        + str(idle_cycles_percent)
        + "%)"
    )
    return hover_text, intensity

def get_router_hover_text_and_intensity(x_cood, y_cood, routers_df, total_cycles, tile_dim):
    if y_cood == tile_dim:
        y_cood = 0
    if x_cood == (2 * tile_dim):
        x_cood = 0
    # Get the core row matching x_cood and y_cood columns
    router_row = routers_df.loc[
        (routers_df["x_cood"] == x_cood) & (routers_df["y_cood"] == y_cood)
    ]
    if router_row.empty:
        print("ERROR: Could not find router row for: ", x_cood, y_cood)
        return "", 0
    # Get the idle cycles and busy cycles
    idle_cycles = router_row["idle_cycles"].values[0]
    busy_cycles = router_row["busy_cycles"].values[0]
    # Get the idle cycles and busy cycles as percentages (Round to 2 decimal places)
    idle_cycles_percent = round((idle_cycles / total_cycles) * 100, 2)
    busy_cycles_percent = round((busy_cycles / total_cycles) * 100, 2)
    intensity = iclass(busy_cycles_percent)
    # Get the hover text
    hover_text = (
        "NeuraRouter ["
        + str(x_cood)
        + ", "
        + str(y_cood)
        + "]<br>Busy Cycles: "
        + str(busy_cycles)
        + " ("
        + str(busy_cycles_percent)
        + "%)"
        + "<br>Idle Cycles: "
        + str(idle_cycles)
        + " ("
        + str(idle_cycles_percent)
        + "%)"
    )
    return hover_text, intensity

def get_writer_hover_text_and_intensity(x_cood, y_cood, writers_df, total_cycles):
    # Get the core row matching x_cood and y_cood columns
    writer_row = writers_df.loc[
        (writers_df["x_cood"] == x_cood) & (writers_df["y_cood"] == y_cood)
    ]
    # Get the idle cycles and busy cycles
    idle_cycles = writer_row["idle_cycles"].values[0]
    busy_cycles = writer_row["busy_cycles"].values[0]
    # Get the idle cycles and busy cycles as percentages (Round to 2 decimal places)
    idle_cycles_percent = round((idle_cycles / total_cycles) * 100, 2)
    busy_cycles_percent = round((busy_cycles / total_cycles) * 100, 2)
    intensity = iclass(busy_cycles_percent)
    # Get the hover text
    hover_text = (
        "Writer ["
        + str(x_cood)
        + ", "
        + str(y_cood)
        + "]<br>Busy Cycles: "
        + str(busy_cycles)
        + " ("
        + str(busy_cycles_percent)
        + "%)"
        + "<br>Idle Cycles: "
        + str(idle_cycles)
        + " ("
        + str(idle_cycles_percent)
        + "%)"
    )
    return hover_text, intensity



def add_comp(
    component,
    del_intensity,
    x,
    y,
    fig,
    component_paths,
    x_cood,
    y_cood,
    tile_dim,
    cores_df,
    mems_df,
    routers_df,
    writers_df,
    total_cycles,
    overlay_mc
):
    component_index = -1
    sx = 1
    sy = 1
    hover_text = ""
    if component == "core":
        core_hover_text, intensity = get_core_hover_text_and_intensity(x_cood, y_cood, cores_df, total_cycles)
        component_index = CORE_PATH_INDEX
        hover_text = core_hover_text
    elif component == "mc":
        component_index = MC_PATH_INDEX
        hover_text = (
            "Memory Controller ["
            + str(x_cood)
            + ", "
            + str(y_cood)
            + "]<br>Busy Cycles: "
            + "0"
            + "<br>Idle Cycles: "
            + "0"
        )
        intensity = 0  # Temporarily set to 0
    elif component == "mem":
        component_index = MEM_PATH_INDEX
        hover_text, intensity = get_mem_hover_text_and_intensity(x_cood, y_cood, mems_df, total_cycles)
    elif component == "router":
        component_index = ROUTER_PATH_INDEX
        hover_text, intensity = get_router_hover_text_and_intensity(x_cood, y_cood, routers_df, total_cycles, tile_dim)
        sx = 1.0
        sy = 1.0
    elif component == "writer":
        component_index = WRITER_PATH_INDEX
        hover_text, intensity = get_writer_hover_text_and_intensity(x_cood, y_cood, writers_df, total_cycles)
    elif component == "hbm":
        component_index = HBM_PATH_INDEX
        intensity = 0  # HBM only has one intensity
        sx = 10
        sy = 10
        hover_text = "HBM<br>Busy Cycles: " + "0" + "<br>Idle Cycles: " + "0"
    else:
        print("ERROR: Invalid component")
        return

    if component_index != -1:
        if overlay_mc:
            # Add annotation "M" on top of the component
            fig.add_annotation(
                x=x,
                y=y,
                text="M",
                showarrow=False,
                font=dict(
                    size=40,
                    color="rgba(0, 0, 0, 1)",
                    # Bold
                    # family="Courier New, monospace",
                    # Make text more visible
                    # color="RebeccaPurple"
                ),
            )
        # Add hover info
        fig.add_layout_image(
            dict(
                source=component_paths[component_index][intensity],
                xref="x",
                yref="y",
                xanchor="center",
                yanchor="middle",
                x=x,
                y=y,
                sizex=sx,
                sizey=sy,
                sizing="contain",  # "stretch" or "fill" or "contain"
                opacity=1,  # 0 - 1
                # layer="above", # "above" or "below"
            )
        )
        fig.add_trace(
            go.Scatter(
                x=[x],
                y=[y],
                hoverinfo="text",
                hovertext=[hover_text],
                mode="markers",
                showlegend=False,
                marker=dict(
                    opacity=0,
                    size=40,
                ),  # set the marker to be invisible
            )
        )


def add_grid(tile_dim, fig, db):
    conns_df = get_overal_conns_df(db)
    total_cycles = get_total_cycles(db)
    for i in range((tile_dim * 2) + 1):
        for j in range(tile_dim + 1):
            add_single_router_conns(i, j, tile_dim, fig, conns_df, total_cycles)


def add_single_router_conns(x_cood, y_cood, tile_dim, fig, conns_df, total_cycles):
    sx = 1.0
    sy = 1.0
    x = x_cood * 2
    y = y_cood * 2
    MAX_WIDTH_VALUE = 10
    # Add connections
    if x_cood < (2 * tile_dim):
        # Right connection
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x + 2,
            y,
            fig,
            x_cood,
            y_cood,
            "R",
            x_cood + 1,
            y_cood,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )
    if y_cood < (tile_dim):
        # Top connection
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x,
            y + 2,
            fig,
            x_cood,
            y_cood,
            "R",
            x_cood,
            y_cood + 1,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )
    if x_cood < (2 * tile_dim) and y_cood < (tile_dim):
        # Top Right connection
        right_component_name = ""
        if (x_cood + y_cood) % 2 == 0:
            right_component_name = "M"
            right_x = int(x_cood / 2)
            right_y = y_cood
        else:
            right_component_name = "C"
            right_x = int(x_cood / 2)
            # right_y = y_cood - 1
            right_y = y_cood
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x + 1,
            y + 1,
            fig,
            x_cood,
            y_cood,
            right_component_name,
            right_x,
            right_y,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )
    if y_cood > 0 and x_cood < (2 * tile_dim):
        # Bottom Right connection
        right_component_name = ""
        if (x_cood + y_cood) % 2 == 0:
            right_component_name = "C"
            right_x = int(x_cood / 2)
            right_y = int(y_cood - 1)
        else:
            right_component_name = "M"
            right_x = int(x_cood / 2)
            right_y = y_cood - 1
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x + 1,
            y - 1,
            fig,
            x_cood,
            y_cood,
            right_component_name,
            right_x,
            right_y,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )
    if y_cood > 0 and x_cood > 0:
        # Bottom Left connection
        right_component_name = ""
        if (x_cood + y_cood) % 2 == 0:
            right_component_name = "M"
            # right_x = int((x_cood - 0.5) / 2)
            right_x = -1
            if right_x % 2 == 0:
                right_x = int((x_cood - 2) // 2)
            else:
                right_x = int((x_cood - 1) // 2)
            right_y = y_cood - 1
            # if y_cood % 2 == 0:
            #     right_y = y_cood - 2
            # else:
            #     right_y = y_cood
        else:
            right_component_name = "C"
            right_x = int((x_cood - 0.5) / 2)
            right_y = y_cood - 1
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x - 1,
            y - 1,
            fig,
            x_cood,
            y_cood,
            right_component_name,
            right_x,
            right_y,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )
    if y_cood < (tile_dim) and x_cood > 0:
        # Top Left connection
        right_component_name = ""
        if (x_cood + y_cood) % 2 == 0:
            right_component_name = "C"
            right_x = int((x_cood - 0.5) / 2)
            # right_y = -1
            # if y_cood % 2 == 0:
            #     right_y = y_cood + 1
            # else:
            #     right_y = y_cood - 1
            right_y = y_cood
            
        else:
            right_component_name = "M"
            right_x = int((x_cood - 0.5) / 2)
            right_y = y_cood
        conn_intensity = random.randint(0, MAX_WIDTH_VALUE)
        add_conn(
            x,
            y,
            x - 1,
            y + 1,
            fig,
            x_cood,
            y_cood,
            right_component_name,
            right_x,
            right_y,
            conns_df,
            total_cycles,
            tile_dim,
            conn_intensity,
        )


def get_conn_hover_text_and_intensity(
    left_component,
    left_x,
    left_y,
    right_component,
    right_x,
    right_y,
    conns_df,
    total_cycles,
    TILE_DIM,
):
    if left_component == "R":
        left_x = left_x % (2 * TILE_DIM)
        left_y = left_y % TILE_DIM
    if right_component == "R":
        right_x = right_x % (2 * TILE_DIM)
        right_y = right_y % TILE_DIM
    left_compoent_name = left_component + "_" + str(left_x) + "_" + str(left_y)
    right_compoent_name = right_component + "_" + str(right_x) + "_" + str(right_y)
    full_name = left_compoent_name + " - " + right_compoent_name
    alt_full_name = right_compoent_name + " - " + left_compoent_name
    # In conns_df, find the row with the name full_name in the column 'connected_comps'
    row = conns_df.loc[conns_df["connected_comps"] == full_name]
    if row.empty:
        row = conns_df.loc[conns_df["connected_comps"] == alt_full_name]
    if row.empty:
        print("ERROR: Could not find row for: ", full_name)
        return "", 100
    # print("Finding: ", full_name)
    # print(row)
    # Get idle_cycles and busy_cycles
    idle_cycles = row["idle_cycles"].values[0]
    busy_cycles = row["busy_cycles"].values[0]
    # Get idle_cycles and busy_cycles as percentages (Round to 2 decimal places)
    idle_cycles_percent = round((idle_cycles / total_cycles) * 100, 2)
    busy_cycles_percent = round((busy_cycles / total_cycles) * 100, 2)
    intensity = busy_cycles_percent
    # Get the hover text
    hover_text = (
        left_compoent_name
        + " -> "
        + right_compoent_name
        + "<br>Busy Cycles: "
        + str(busy_cycles)
        + " ("
        + str(busy_cycles_percent)
        + "%)"
        + "<br>Idle Cycles: "
        + str(idle_cycles)
        + " ("
        + str(idle_cycles_percent)
        + "%)"
    )
    return hover_text, intensity


def generate_tile_viz(exp_name, tile_dim, db):
    print("Generating tile visulaization")
    total_cycles = get_total_cycles(db)
    component_paths = generate_path_arrays()
    TILE_DIM = tile_dim

    # EXP_NAME = 'MC_CH_V01'
    EXP_NAME = exp_name
    # mkdir results_neurachip/MC_CH_V01/figs/tile if it doesn't exist
    if not os.path.exists("/home/ktb/git/NeuraChip/NeuraViz/results_neurachip/" + EXP_NAME + "/figs/tile"):
        os.makedirs("/home/ktb/git/NeuraChip/NeuraViz/results_neurachip/" + EXP_NAME + "/figs/tile")
    


    DEBUG_GRID = False

    client = MongoClient("localhost", 27017)
    db = client[EXP_NAME]

    frame_max_x = 8.25
    frame_min_x = 0

    frame_max_y = 8.5
    frame_min_y = -0.8

    frame_x = [frame_min_x, frame_min_x, frame_max_x, frame_max_x]
    frame_y = [frame_min_y, frame_max_y, frame_max_y, frame_min_y]

    # Create a plotly figure with the image
    # Use plot_bgcolor='#191c24', paper_bgcolor='#191c24', font_color='white' to set the background color to black and the font color to white
    fig = go.Figure()
    fig.update_layout(
        plot_bgcolor="#191c24", paper_bgcolor="#191c24", font_color="white"
    )

    if DEBUG_GRID == False:
        # Disable the grid
        fig.update_xaxes(showgrid=False)
        fig.update_yaxes(showgrid=False)
        # Remove the margin
        fig.update_layout(margin=dict(l=0, r=0, b=0, t=0, pad=0))
        # Remove the axis ticks
        fig.update_xaxes(showticklabels=False)
        fig.update_yaxes(showticklabels=False)
        # Remove the axis
        fig.update_xaxes(showline=False, zeroline=False)
        fig.update_yaxes(showline=False, zeroline=False)

    # Scatter plot the frame
    # fig.add_trace(go.Scatter(x=frame_x, y=frame_y, showlegend=False, line=dict(color="LightSeaGreen", width=0)))

    # Add grid
    add_grid(TILE_DIM, fig, db)

    cores_df = get_overal_cores_df(db)
    mems_df = get_overal_mems_df(db)
    routers_df = get_overal_routers_df(db)
    writers_df = get_overal_writers_df(db)
    mc_x_coods, mc_y_coods = get_mc_attached_router_coods(db)

    # cores_df, mems_df, routers_df, writers_df
    # mcs_df = get_overal_mcs_df(db)
    # hbm_df = get_overal_hbm_df(db)

    # Add MCs
    for i in range(8):
        intensity = random.randint(0, 3)
        add_comp(
            "mc",
            intensity,
            2*i + 1,
            -1.0,
            fig,
            component_paths,
            i,
            0,
            TILE_DIM,
            cores_df,
            mems_df,
            routers_df,
            writers_df,
            total_cycles,
            False
        )


    # Add Routers
    for i in range((TILE_DIM * 2) + 1):
        for j in range(TILE_DIM + 1):
            intensity = random.randint(0, 3)
            mc_overlay = False
            mc_compare_x = i % (2 * TILE_DIM)
            mc_compare_y = j % TILE_DIM
            for k in range(len(mc_x_coods)):
                if mc_x_coods[k] == mc_compare_x and mc_y_coods[k] == mc_compare_y:
                    mc_overlay = True
                    break
            add_comp(
                "router",
                intensity,
                i * 2,
                j * 2,
                fig,
                component_paths,
                i,
                j,
                TILE_DIM,
                cores_df,
                mems_df,
                routers_df,
                writers_df,
                total_cycles,
                mc_overlay
            )

    # Add Cores
    for i in range(TILE_DIM):
        for j in range(TILE_DIM):
            intensity = random.randint(0, 3)
            if j % 2 == 0:
                add_comp(
                    "core",
                    intensity,
                    3 + i * 4, # i * 4 + 1,
                    1 + j * 2, # j * 2 + 3,
                    fig,
                    component_paths,
                    i,
                    j,
                    TILE_DIM,
                    cores_df,
                    mems_df,
                    routers_df,
                    writers_df,
                    total_cycles,
                    False
                )
            else:
                add_comp(
                    "core",
                    intensity,
                    1 + i * 4, # i * 4 + 3,
                    1 + j * 2, # j * 2 - 1,
                    fig,
                    component_paths,
                    i,
                    j,
                    TILE_DIM,
                    cores_df,
                    mems_df,
                    routers_df,
                    writers_df,
                    total_cycles,
                    False
                )

    # # Add Mems
    for i in range(TILE_DIM):
        for j in range(TILE_DIM):
            intensity = random.randint(0, 3)
            if j % 2 == 0:
                add_comp(
                    "mem",
                    intensity,
                    i * 4 + 1,
                    j * 2 + 1, # j * 2 + 3,
                    fig,
                    component_paths,
                    i,
                    j,
                    TILE_DIM,
                    cores_df,
                    mems_df,
                    routers_df,
                    writers_df,
                    total_cycles,
                    False
                )
            else:
                add_comp(
                    "mem",
                    intensity,
                    i * 4 + 3,
                    j * 2 + 1, # j * 2 - 1,
                    fig,
                    component_paths,
                    i,
                    j,
                    TILE_DIM,
                    cores_df,
                    mems_df,
                    routers_df,
                    writers_df,
                    total_cycles,
                    False
                )

    # Display the figure
    # fig.show()
    # Save the figure
    # fig.write_image('./results_neurachip/' + EXP_NAME + './figs/tile/tile.html', width=1920, height=1080)
    fig.write_html("/home/ktb/git/NeuraChip/NeuraViz/results_neurachip/" + EXP_NAME + "/figs/tile/tile.html")
