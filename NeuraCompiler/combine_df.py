# Combine multiple CSV files into one dataframe and save as CSV

# Imports
import pandas as pd
import os
import sys
from tqdm import tqdm

# Set working directory
PROG_ROOT = "/home/ktb/git/NeuraChip/NeuraCompiler/programs/"
DATASET_NAME = sys.argv[1]

csv_dir = PROG_ROOT + DATASET_NAME + "/bin"

# Count the total number of files starting with row_
num_files = len([f for f in os.listdir(csv_dir) if f.startswith('row_')])
print("Number of rows: " + str(num_files))

# Create a list of all the files
file_list = []
for i in range(num_files):
    file_list.append(csv_dir + "/row_" + str(i) + ".csv")

# Make sure all the files exist
for file in file_list:
    if not os.path.exists(file):
        print("ERROR: File does not exist: " + file)
        sys.exit()

# Join all files together into a single file called spmm.csv
output_file = csv_dir + "/spmm.csv"
if os.path.exists(output_file):
    print("WARN: Output file already exists: " + output_file)
    # Delete the output file
    os.remove(output_file)


print("Joining files into " + output_file)
with open(output_file, 'w') as outfile:
    for file in tqdm(file_list):
        with open(file) as infile:
            for line in infile:
                outfile.write(line)

print("Done!")









# Print the number of rows in the combined dataframe
# print("Number of rows in combined dataframe: " + str(len(df.index)))

# Print the number of rows in the original dataframes
# print("Number of rows in original dataframes: " + str(sum([len(df.index) for df in df_list])))



