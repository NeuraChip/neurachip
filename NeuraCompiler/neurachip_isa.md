# The NeuraCHIP ISA

## Macro Instructions
* **REDUCE_FLOAT_32** (DST, SRC, COUNT, REDUCE_OP)
SRC address will have to be contiguous in memory.
This might require virtual global address space.


* **REDUCE_INT_32** (DST, SRC, COUNT, REDUCE_OP)

## Micro Instructions
### 32-bit Floating Point Instructions
* **ADD_FLOAT_32** (DST, SRC1, SRC2)
* **MUL_FLOAT_32** (DST, SRC1, SRC2)
* **LOAD_FLOAT_32** (DST, SRC)
* **STORE_FLOAT_32** (DST, SRC)
### 32-bit Integer Instructions
* **ADD_INT_32** (DST, SRC1, SRC2)
* **MUL_INT_32** (DST, SRC1, SRC2)
* **LOAD_INT_32** (DST, SRC)
* **STORE_INT_32** (DST, SRC)
### 16-bit Integer Instructions
* **ADD_INT_16** (DST, SRC1, SRC2)
* **MUL_INT_16** (DST, SRC1, SRC2)
* **LOAD_INT_16** (DST, SRC)
* **STORE_INT_16** (DST, SRC)
### 8-bit Integer Instructions
* **ADD_INT_8** (DST, SRC1, SRC2)
* **MUL_INT_8** (DST, SRC1, SRC2)
* **LOAD_INT_8** (DST, SRC)
* **STORE_INT_8** (DST, SRC)

### Control Instructions
* **NOP**
No operation. Does nothing.

* **HCF**
Halt and Catch Fire. Stops the execution of the program.

* **HALT**
Halt. Stops the execution of the program.


### Jump Instructions
* **JUMP** (DST)
* **JUMP_IF** (DST, SRC1, SRC2, CMP_OP)
* **JUMP_IF_NOT** (DST, SRC1, SRC2, CMP_OP)
* **JUMP_IF_ZERO** (DST, SRC, CMP_OP)
* **JUMP_IF_NOT_ZERO** (DST, SRC, CMP_OP)
* **JUMP_IF_LESS** (DST, SRC1, SRC2)
* **JUMP_IF_LESS_EQUAL** (DST, SRC1, SRC2)
* **JUMP_IF_GREATER** (DST, SRC1, SRC2)
* **JUMP_IF_GREATER_EQUAL** (DST, SRC1, SRC2)
* **JUMP_IF_EQUAL** (DST, SRC1, SRC2)
* **JUMP_IF_NOT_EQUAL** (DST, SRC1, SRC2)
* **JUMP_IF_LESS_ZERO** (DST, SRC)
* **JUMP_IF_LESS_EQUAL_ZERO** (DST, SRC)
* **JUMP_IF_GREATER_ZERO** (DST, SRC)
* **JUMP_IF_GREATER_EQUAL_ZERO** (DST, SRC)

### Memory Instructions
* **LOAD_32** (DST, SRC)
* **STORE_32** (DST, SRC)
* **LOAD_64** (DST, SRC)
* **STORE_64** (DST, SRC)

