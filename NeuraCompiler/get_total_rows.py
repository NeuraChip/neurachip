# Get total rows of the given dataset

# Imports
import pandas as pd
import os
import sys
import numpy as np
from torch_geometric.datasets import Planetoid
from torch_geometric.utils import to_scipy_sparse_matrix


dataset_name = sys.argv[1]

root_dir = '/home/ktb/git/NeuraChip/NeuraCompiler/data'
output_dir = '/home/ktb/git/NeuraChip/NeuraCompiler/programs/' + dataset_name
# Capitalize the first letter
dataset_name = dataset_name[0].upper() + dataset_name[1:]
dataset = Planetoid(root=root_dir, name=dataset_name)
# cora_dataset = Planetoid(root='/home/ktb/git/NeuraChip/NeuraCompiler/data',
# 				name='Cora')
data = dataset[0]
# Get the edge indices
edge_indices = data.edge_index
mat_A_csr = to_scipy_sparse_matrix(edge_indices).tocsr()
# Get the number of rows
num_rows = mat_A_csr.shape[0]
print(str(num_rows))
# Write the number of rows to a file
with open(output_dir + "/num_rows.txt", "w") as f:
    f.write(str(num_rows))

# Get the number of columns
num_cols = mat_A_csr.shape[1]
# Write the number of columns to a file
with open(output_dir + "/num_cols.txt", "w") as f:
    f.write(str(num_cols))

# Get the number of non-zero elements
num_nonzero = mat_A_csr.nnz
# Write the number of non-zero elements to a file
with open(output_dir + "/num_nonzero.txt", "w") as f:
    f.write(str(num_nonzero))

