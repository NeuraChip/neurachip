# Imports
import pandas as pd
from tqdm import tqdm
from torch_geometric.utils.convert import to_scipy_sparse_matrix
from scipy import sparse
from torch_geometric.datasets import Planetoid
import torch
import matplotlib.pyplot as plt
import numpy as np
import os
import shutil
from sys import argv
# Import Ray parallelization library




# Global variables
FREE_MEMORY_POINTER = 0
OFFSET_POINTER = -1

class Matrix:
	def __init__(self, type='', row_ptr=-1, col_indices=-1, data=-1, col_ptr=-1, row_indices=-1, x_dim=-1, y_dim=-1, name=''):
		self.type = type
		self.row_ptr = row_ptr
		self.col_indices = col_indices
		self.data = data
		self.data_int = [int(x) for x in data]
		self.col_ptr = col_ptr
		self.row_indices = row_indices
		self.x_dim = x_dim
		self.y_dim = y_dim
		self.name = name
		self.nnz = len(self.data)
		if type == 'CSR_CSC':
			self.row_ptr_address = malloc_4B(len(self.row_ptr))
			self.col_indices_address = malloc_4B(len(self.col_indices))
			self.data_address = malloc_4B(len(self.data))
			self.col_ptr_address = malloc_4B(len(self.col_ptr))
			self.row_indices_address = malloc_4B(len(self.row_indices))
		elif type == 'CSR':
			self.row_ptr_address = malloc_4B(len(self.row_ptr))
			self.col_indices_address = malloc_4B(len(self.col_indices))
			self.data_address = malloc_4B(len(self.data))
			self.col_ptr_address = -1
			self.row_indices_address = -1
		elif type == 'CSC':
			self.row_ptr_address = -1
			self.col_indices_address = -1
			self.data_address = malloc_4B(len(self.data))
			self.col_ptr_address = malloc_4B(len(self.col_ptr))
			self.row_indices_address = malloc_4B(len(self.row_indices))
		else:
			print("Invalid matrix type")
			exit()

	def __str__(self):
		text = "********** MATRIX " + self.name + "**********\n"
		text = text + "Type: " + str(self.type) + " "
		text = text + "Shape: " + str(self.x_dim) + " x " + str(self.y_dim) + " NNZ: " + str(self.nnz) + "\n"
		text = text + "Row ptr: " + hex(self.row_ptr_address) + "\n"
		text = text + "Col indices: " + hex(self.col_indices_address) + "\n"
		text = text + "Data: " + hex(self.data_address) + "\n"
		text = text + "Col ptr: " + hex(self.col_ptr_address) + "\n"
		text = text + "Row indices: " + hex(self.row_indices_address) + "\n"
		text = text + "************************************\n"
		return text

# MULT_HASH_ACC(a_data_addr, b_data_addr, c_col_id, count, table_id)
def MULT_HASH_ACC(a_data_addr, b_data_addr, c_col_id, count, table_id):	
	instx_name = "MULT_HASH_ACC"
	instx = (instx_name, a_data_addr, b_data_addr, c_col_id, count, table_id)
	return instx

def reset_env():
	global FREE_MEMORY_POINTER
	FREE_MEMORY_POINTER = 0
	reset_offset()

def reset_offset():
	global OFFSET_POINTER
	OFFSET_POINTER = -1

# Functions
def malloc_memory(size):
	global FREE_MEMORY_POINTER
	FREE_MEMORY_POINTER += size
	return FREE_MEMORY_POINTER - size

def malloc_4B(element_count):
	return malloc_memory(element_count * 4)

def get_memory_pointer():
	global FREE_MEMORY_POINTER
	return FREE_MEMORY_POINTER

def print_memory_usage():
	global FREE_MEMORY_POINTER
	# Convert to human readable format and print 2 decimal places
	if FREE_MEMORY_POINTER < 1024:
		print("Memory usage: {} B".format(FREE_MEMORY_POINTER))
	elif FREE_MEMORY_POINTER < 1024**2:
		free_memory = round(FREE_MEMORY_POINTER/1024, 2)
		print("Memory usage: {} KB".format(free_memory))
	elif FREE_MEMORY_POINTER < 1024**3:
		free_memory = round(FREE_MEMORY_POINTER/1024**2, 2)
		print("Memory usage: {} MB".format(free_memory))
	else:
		free_memory = round(FREE_MEMORY_POINTER/1024**3, 2)
		print("Memory usage: {} GB".format(free_memory))

def gen_dataset_matrices(dataset_name):
	print("Generating matrices for dataset " + dataset_name)
	print("Loading dataset...")
	root_dir = '/home/ktb/git/NeuraChip/NeuraCompiler/data'
	# Capitalize the first letter
	dataset_name = dataset_name[0].upper() + dataset_name[1:]
	dataset = Planetoid(root=root_dir, name=dataset_name)
	# cora_dataset = Planetoid(root='/home/ktb/git/NeuraChip/NeuraCompiler/data',
	# 				name='Cora')
	data = dataset[0]
	# Get the edge indices
	edge_indices = data.edge_index
	mat_A_csr = to_scipy_sparse_matrix(edge_indices).tocsr()
	mat_A_csc = to_scipy_sparse_matrix(edge_indices).tocsc()
	mat_A = Matrix(name='A', type='CSR_CSC', row_ptr=mat_A_csr.indptr, col_indices=mat_A_csr.indices, data=mat_A_csr.data, col_ptr=mat_A_csc.indptr, row_indices=mat_A_csc.indices, x_dim=mat_A_csr.shape[0], y_dim=mat_A_csr.shape[1])
	mat_B = data.x
	mat_B_csr = sparse.csr_matrix(mat_B)
	mat_B_csc = sparse.csc_matrix(mat_B)
	mat_B = Matrix(name='B', type='CSR_CSC', row_ptr=mat_B_csr.indptr, col_indices=mat_B_csr.indices, data=mat_B_csr.data, col_ptr=mat_B_csc.indptr, row_indices=mat_B_csc.indices, x_dim=mat_B_csr.shape[0], y_dim=mat_B_csr.shape[1])
	return mat_A, mat_B

def gen_dense_matrix(x_dim, y_dim):
	# Create a dense matrix of ones
	mat_A = np.ones((x_dim, y_dim))
	# Convert to CSR and CSC
	mat_A_csr = sparse.csr_matrix(mat_A)
	mat_A_csc = sparse.csc_matrix(mat_A)
	# Create a matrix object
	mat_A = Matrix(name='A', type='CSR_CSC', row_ptr=mat_A_csr.indptr, col_indices=mat_A_csr.indices, data=mat_A_csr.data, col_ptr=mat_A_csc.indptr, row_indices=mat_A_csc.indices, x_dim=mat_A_csr.shape[0], y_dim=mat_A_csr.shape[1])
	return mat_A

def generate_padded_mat_C(mat_A, mat_B):
	print("Generating padded matrix C")
	c_row_ptr = np.zeros(mat_A.x_dim + 1, dtype=np.int32)
	max_nnz = 0
	c_x_dim = mat_A.x_dim
	c_y_dim = mat_B.y_dim
	for i in tqdm(range(mat_A.x_dim)):
		single_row_max_nnz = 0
		for j in range(mat_A.row_ptr[i], mat_A.row_ptr[i+1]):
			mat_B_start_index = mat_B.row_ptr[mat_A.col_indices[j]]
			mat_B_end_index = mat_B.row_ptr[mat_A.col_indices[j]+1]
			single_row_max_nnz += mat_B_end_index - mat_B_start_index
			# single_row_max_nnz += mat_B.col_ptr[mat_A.col_indices[j]+1] - mat_B.col_ptr[mat_A.col_indices[j]]
		c_row_ptr[i+1] = c_row_ptr[i] + single_row_max_nnz
		max_nnz += single_row_max_nnz
	c_col_indices = np.zeros(max_nnz, dtype=np.int32)
	c_data = np.zeros(max_nnz, dtype=np.float32)
	mat_C = Matrix(name='C', type='CSR', row_ptr=c_row_ptr, col_indices=c_col_indices, data=c_data, col_ptr=None, row_indices=None, x_dim=c_x_dim, y_dim=c_y_dim)
	return mat_C

def array_writer(array, file_path):
	with open(file_path, 'w') as f:
		for element in array:
			f.write(str(element) + '\n')

def matrix_header_writer(mat, file_folder):
	# print("Writing matrix header to " + file_folder + "header.csv")
	header = "name," + mat.name + "\n"
	header += "x_dim," + str(mat.x_dim) + "\n"
	header += "y_dim," + str(mat.y_dim) + "\n"
	header += "type," + mat.type + "\n"
	header += "nnz," + str(mat.nnz) + "\n"
	header += "row_ptr_address," + str(mat.row_ptr_address) + "\n"
	header += "col_indices_address," + str(mat.col_indices_address) + "\n"
	header += "data_address," + str(mat.data_address) + "\n"
	header += "col_ptr_address," + str(mat.col_ptr_address) + "\n"
	header += "row_indices_address," + str(mat.row_indices_address) + "\n"
	with open(file_folder + "header.csv", 'w') as f:
		f.write(header)

def dram_raw_data_writer(mat_A, mat_B, mat_C):
	print("Writing raw data to DRAM")
	dram_file_path = PROG_ROOT + DATASET_NAME + "/raw_data/"
	if not os.path.exists(dram_file_path):
		os.makedirs(dram_file_path)
	dram_file_name = dram_file_path + "dram_raw_data.csv"
	with open(dram_file_name, 'w+') as f:
		
		# Write the matrix A
		if mat_A.type != 'CSR_CSC':
			raise Exception("Matrix A must be of type CSR_CSC")
		# Row pointer
		if len(mat_A.row_ptr) == 1:
			raise Exception("Matrix A row pointer is not initialized")
		for i in range(len(mat_A.row_ptr)):
			address = mat_A.row_ptr_address + (i * 4)
			data = mat_A.row_ptr[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Column indices
		if len(mat_A.col_indices) == 1:
			raise Exception("Matrix A column indices is not initialized")
		for i in range(len(mat_A.col_indices)):
			address = mat_A.col_indices_address + (i * 4)
			data = mat_A.col_indices[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Data
		if len(mat_A.data_int) == -1:
			raise Exception("Matrix A data is not initialized")
		for i in range(len(mat_A.data_int)):
			address = mat_A.data_address + (i * 4)
			data = mat_A.data_int[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Column pointer
		if len(mat_A.col_ptr) == -1:
			raise Exception("Matrix A column pointer is not initialized")
		for i in range(len(mat_A.col_ptr)):
			address = mat_A.col_ptr_address + (i * 4)
			data = mat_A.col_ptr[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Row indices
		if len(mat_A.row_indices) == -1:
			raise Exception("Matrix A row indices is not initialized")
		for i in range(len(mat_A.row_indices)):
			address = mat_A.row_indices_address + (i * 4)
			data = mat_A.row_indices[i]
			f.write(str(address) + "," + str(data) + "\n")
		
		# Write the matrix B
		if mat_B.type != 'CSR_CSC':
			raise Exception("Matrix B must be of type CSR_CSC")
		# Row pointer
		if len(mat_B.row_ptr) == -1:
			raise Exception("Matrix B row pointer is not initialized")
		for i in range(len(mat_B.row_ptr)):
			address = mat_B.row_ptr_address + (i * 4)
			data = mat_B.row_ptr[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Column indices
		if len(mat_B.col_indices) == -1:
			raise Exception("Matrix B column indices is not initialized")
		for i in range(len(mat_B.col_indices)):
			address = mat_B.col_indices_address + (i * 4)
			data = mat_B.col_indices[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Data
		if len(mat_B.data_int) == -1:
			raise Exception("Matrix B data is not initialized")
		for i in range(len(mat_B.data_int)):
			address = mat_B.data_address + (i * 4)
			data = mat_B.data_int[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Column pointer
		if len(mat_B.col_ptr) == -1:
			raise Exception("Matrix B column pointer is not initialized")
		for i in range(len(mat_B.col_ptr)):
			address = mat_B.col_ptr_address + (i * 4)
			data = mat_B.col_ptr[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Row indices
		if len(mat_B.row_indices) == -1:
			raise Exception("Matrix B row indices is not initialized")
		for i in range(len(mat_B.row_indices)):
			address = mat_B.row_indices_address + (i * 4)
			data = mat_B.row_indices[i]
			f.write(str(address) + "," + str(data) + "\n")
		
		# Write the matrix C
		if mat_C.type != 'CSR':
			raise Exception("Matrix C must be of type CSR")
		# Row pointer
		if len(mat_C.row_ptr) == -1:
			raise Exception("Matrix C row pointer is not initialized")
		for i in range(len(mat_C.row_ptr)):
			address = mat_C.row_ptr_address + (i * 4)
			data = mat_C.row_ptr[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Column indices
		if len(mat_C.col_indices) == -1:
			raise Exception("Matrix C column indices is not initialized")
		for i in range(len(mat_C.col_indices)):
			address = mat_C.col_indices_address + (i * 4)
			data = mat_C.col_indices[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Data
		if len(mat_C.data_int) == -1:
			raise Exception("Matrix C data is not initialized")
		for i in range(len(mat_C.data_int)):
			address = mat_C.data_address + (i * 4)
			data = mat_C.data_int[i]
			f.write(str(address) + "," + str(data) + "\n")
		# Close the file
		f.close()


def csr_csc_writer(mat, type, mat_folder):
	os.makedirs(mat_folder, exist_ok=True)
	if type == 'CSR_CSC':
		array_writer(mat.row_ptr, mat_folder + "row_ptr.txt")
		array_writer(mat.col_indices, mat_folder + "col_indices.txt")
		array_writer(mat.data, mat_folder + "data.txt")
		array_writer(mat.col_ptr, mat_folder + "col_ptr.txt")
		array_writer(mat.row_indices, mat_folder + "row_indices.txt")
	elif type == 'CSR':
		array_writer(mat.row_ptr, mat_folder + "row_ptr.txt")
		array_writer(mat.col_indices, mat_folder + "col_indices.txt")
		array_writer(mat.data, mat_folder + "data.txt")
	elif type == 'CSC':
		array_writer(mat.col_ptr, mat_folder + "col_ptr.txt")
		array_writer(mat.row_indices, mat_folder + "row_indices.txt")
		array_writer(mat.data, mat_folder + "data.txt")
	else:
		raise ValueError("Wrong matrix type. Only CSR_CSC, CSR and CSC are supported.")


# DRAM Dataloader
def create_dram_file(mat_A, mat_B, mat_C):
	START_ADDR = 0x00000000
	FOLDER_PATH = PROG_ROOT + DATASET_NAME + "/matrix/"
	MAT_A_FOLDER = FOLDER_PATH + "mat_A/"
	MAT_B_FOLDER = FOLDER_PATH + "mat_B/"
	MAT_C_FOLDER = FOLDER_PATH + "mat_C/"

	# Create the folders
	os.makedirs(MAT_A_FOLDER, exist_ok=True)
	os.makedirs(MAT_B_FOLDER, exist_ok=True)
	os.makedirs(MAT_C_FOLDER, exist_ok=True)

	csr_csc_writer(mat_A, 'CSR_CSC', MAT_A_FOLDER)
	csr_csc_writer(mat_B, 'CSR_CSC', MAT_B_FOLDER)
	csr_csc_writer(mat_C, 'CSR', MAT_C_FOLDER)

	matrix_header_writer(mat_A, MAT_A_FOLDER)
	matrix_header_writer(mat_B, MAT_B_FOLDER)
	matrix_header_writer(mat_C, MAT_C_FOLDER)

def get_next_offset():
	global OFFSET_POINTER
	OFFSET_POINTER += 1
	return OFFSET_POINTER

def get_single_row_program_df(dataset_name_input, row_number):
	mat_A, mat_B = gen_dataset_matrices(DATASET_NAME)
	PROGRAM_NAME = "row_" + str(row_number)
	d_name = dataset_name_input
	FOLDER_PATH = PROG_ROOT + d_name + "/bin/"
	PROGRAM_FILE_PATH = FOLDER_PATH + PROGRAM_NAME + ".csv"
	os.makedirs(FOLDER_PATH, exist_ok=True)
	final_program = []
	instx_list = []
	# c_offset_addr_list = []
	a_data_addr_list = []
	b_data_addr_list = []
	b_col_id_addr_list = []
	count_list = []
	table_id_list = []
	# MULT_HASH_ACC(a_data_addr, b_data_addr, b_col_id_addr, count, table_id)
	# print("Processing row " + str(i) + " of matrix A")
	for j in range(mat_A.row_ptr[row_number], mat_A.row_ptr[row_number+1]):
		mat_B_start_index = mat_B.row_ptr[mat_A.col_indices[j]]
		mat_B_end_index = mat_B.row_ptr[mat_A.col_indices[j]+1]
		# print("Processing column " + str(mat_A.col_indices[j]) + " of matrix A")
		for k in range(mat_B_start_index, mat_B_end_index):
			# c_offset = get_next_offset()
			A_data_address = mat_A.data_address + (j * 4)
			B_data_address = mat_B.data_address + (k * 4)
			B_col_index = mat_B.col_indices[k]
			B_col_id_address = mat_B.col_indices_address + (k * 4)
			# # Temporary for debugging
			# B_col_id_address = B_col_index
			# A_data_address = j
			# B_data_address = k
			# C_address_offset = c_offset * 4
			# print("Processing column " + str(B_col_index) + " of matrix B")
			hash_count = get_hash_count(row_number, B_col_index, mat_A, mat_B)
			# hash_count = mat_B.col_ptr[B_col_index+1] - mat_B.col_ptr[B_col_index]
			table_id = row_number
			instx_list.append("MULT_HASH_ACC")
			# c_offset_addr_list.append(C_address_offset)
			a_data_addr_list.append(A_data_address)
			b_data_addr_list.append(B_data_address)
			b_col_id_addr_list.append(B_col_id_address)
			count_list.append(hash_count)
			table_id_list.append(table_id)
			final_program.append(MULT_HASH_ACC(A_data_address, B_data_address, B_col_id_address, hash_count, table_id))
	# Create a Dataframe to save the final program
	final_program_df = pd.DataFrame({
		"instx": instx_list,
		"a_data_addr": a_data_addr_list,
		"b_data_addr": b_data_addr_list,
		"b_col_id_addr": b_col_id_addr_list,
		"count": count_list,
		"table_id": table_id_list
	})
	# Save the final program to a file
	final_program_df.to_csv(PROGRAM_FILE_PATH, index=False, header=False)
	print("Completed Row " + str(row_number))
	return final_program_df



# SpMM Program Creater
def create_spmm_program(mat_A, mat_B, mat_C, dataset_name):
	if SMALL_PROG:
		print("WARNING: SMALL_PROG is True.")
	PROGRAM_NAME = "spmm"
	DATASET_NAME = dataset_name
	FOLDER_PATH = PROG_ROOT + DATASET_NAME + "/bin/"
	PROGRAM_FILE_PATH = FOLDER_PATH + PROGRAM_NAME + ".csv"
	os.makedirs(FOLDER_PATH, exist_ok=True)
	final_program = []
	instx_list = []
	# c_offset_addr_list = []
	a_data_addr_list = []
	b_data_addr_list = []
	b_col_id_addr_list = []
	count_list = []
	table_id_list = []
	# MULT_HASH_ACC(a_data_addr, b_data_addr, b_col_id_addr, count, table_id)
	for i in tqdm(range(mat_A.x_dim)):
		if SMALL_PROG and i >= 1:
			break
		# print("Processing row " + str(i) + " of matrix A")
		for j in range(mat_A.row_ptr[i], mat_A.row_ptr[i+1]):
			mat_B_start_index = mat_B.row_ptr[mat_A.col_indices[j]]
			mat_B_end_index = mat_B.row_ptr[mat_A.col_indices[j]+1]
			# print("Processing column " + str(mat_A.col_indices[j]) + " of matrix A")
			for k in range(mat_B_start_index, mat_B_end_index):
				# c_offset = get_next_offset()
				A_data_address = mat_A.data_address + (j * 4)
				B_data_address = mat_B.data_address + (k * 4)
				B_col_index = mat_B.col_indices[k]
				B_col_id_address = mat_B.col_indices_address + (k * 4)
				# # Temporary for debugging
				# B_col_id_address = B_col_index
				# A_data_address = j
				# B_data_address = k
				# C_address_offset = c_offset * 4
				# print("Processing column " + str(B_col_index) + " of matrix B")
				hash_count = get_hash_count(i, B_col_index, mat_A, mat_B)
				# hash_count = mat_B.col_ptr[B_col_index+1] - mat_B.col_ptr[B_col_index]
				table_id = i
				instx_list.append("MULT_HASH_ACC")
				# c_offset_addr_list.append(C_address_offset)
				a_data_addr_list.append(A_data_address)
				b_data_addr_list.append(B_data_address)
				b_col_id_addr_list.append(B_col_id_address)
				count_list.append(hash_count)
				table_id_list.append(table_id)
				final_program.append(MULT_HASH_ACC(A_data_address, B_data_address, B_col_id_address, hash_count, table_id))
	# Create a Dataframe to save the final program
	final_program_df = pd.DataFrame({
		"instx": instx_list,
		"a_data_addr": a_data_addr_list,
		"b_data_addr": b_data_addr_list,
		"b_col_id_addr": b_col_id_addr_list,
		"count": count_list,
		"table_id": table_id_list
	})
	# Save the final program to a file
	final_program_df.to_csv(PROGRAM_FILE_PATH, index=False, header=False)
	return final_program_df


def get_hash_count(row_number, col_number, mat_A, mat_B):
	hash_count = 0
	# Iterate over col_number column of mat_B
	for i in range(mat_B.col_ptr[col_number], mat_B.col_ptr[col_number+1]):
		# Iterate over row_number row of mat_A
		# Sadly, we perform expensive index matching here
		for j in range(mat_A.row_ptr[row_number], mat_A.row_ptr[row_number+1]):
			if mat_A.col_indices[j] == mat_B.row_indices[i]:
				hash_count += 1
	
	return hash_count


if __name__ == "__main__":
	SMALL_PROG = False
	PROG_ROOT = "/home/ktb/git/NeuraChip/NeuraCompiler/programs/"
	DATASET_NAME = argv[1]
	# Delete directory DATASET_NAME
	if os.path.exists(PROG_ROOT + DATASET_NAME):
		shutil.rmtree(PROG_ROOT + DATASET_NAME)

	# Create directory DATASET_NAME
	os.mkdir(PROG_ROOT + DATASET_NAME)
	reset_env()
	mat_A, mat_B = gen_dataset_matrices(DATASET_NAME)
	print("mat_A dims: ", mat_A.x_dim, mat_A.y_dim)
	print("mat_B dims: ", mat_B.x_dim, mat_B.y_dim)
	mat_C = generate_padded_mat_C(mat_A, mat_B)
	create_dram_file(mat_A, mat_B, mat_C)
	# prog = create_spmm_program(mat_A, mat_B, mat_C, DATASET_NAME)
	# prog = create_spmm_program_parallel(mat_A, mat_B, mat_C, DATASET_NAME)
	# get_single_row_program_df(DATASET_NAME, 0)
	# get_single_row_program_df(DATASET_NAME, 1)
	# get_single_row_program_df(DATASET_NAME, 2)
	dram_raw_data_writer(mat_A, mat_B, mat_C)
	print("Precompile Complete")

