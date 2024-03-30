#ifndef NEURA_CONFIG_H
#define NEURA_CONFIG_H

// General Config
#define NEURA_VERSION "0.2"

// #define ENABLE_COLORS

// Debug Config
// #define TRACE_INSTX

// Cache Microbenchmark
// #define CACHE_MICROBENCHMARK

// MongoDB Flag
#define MONGO_FLAG true

// Pushes connector interval metrics to MongoDB
// Makes simulation extremely slow
#define CONN_INTERVAL_MONGO_FLAG false

// #define DUAL_TX_CONN
// #define MULTI_DISPATCH_COUNT 1
// #define MULTI_TICK_WRITER 4
// #define MULTI_TICK_ROUTER 4
// #define MULTI_TICK_NEURAMEM 1


#define MULTI_TICK_MC 64
#define MULTI_TICK_CONN 32
#define MULTI_DISPATCH_COUNT 1
#define MULTI_TICK_WRITER 64
#define MULTI_TICK_ROUTER 64
#define MULTI_TICK_NEURAMEM 8
#define MULTI_TICK_NEURACORE 32

#define HASH_REQ_ALERT_THRESHOLD 10000000

// #define MC_DISABLE_WRITE
// #define WRITER_DISABLE_HBM_WRITE

// #define BYPASS_NEURA_CORE_HBM_READS

// #define SKIP_NEURAMEM_PROCESSING_COUNTER

// #define DISABLE_NM_HASH_RESPONSES


#define SIM_END_TRUE true
#define SIM_END_FALSE false


// Core Config
// #define NEURA_CORE_X_COUNT 1
// #define NEURA_CORE_Y_COUNT 1
// Last port is for Instructions from Dispatcher
// #define NEURA_CORE_PORT_COUNT 5
// #define NEURA_CORE_DATA_PORT_COUNT 4
// Instruction Port Index for NeuraCores
// #define INSTX_PORT_INDEX 4
// Core register file size
// Number of Instructions that can be stored in register file
// #define PIPELINE_COUNT 4
// #define PIPELINE_COUNT 4096
// Types of Tick Returns
#define CORE_COMPUTED true
#define CORE_STALLED false

// NeuraCore Config
#define CORE_TOP_LEFT_PORT 0
#define CORE_TOP_RIGHT_PORT 1
#define CORE_BOTTOM_LEFT_PORT 2
#define CORE_BOTTOM_RIGHT_PORT 3
#define CORE_INSTX_PORT 4

// Dispatcher Config
#define DISAPATCHER_INSTX_PORT 0

// Memory Config
// #define NEURA_MEM_X_COUNT 1
// #define NEURA_MEM_Y_COUNT 1
// #define NEURA_MEM_PORT_COUNT 4
// Memory Request Types
#define MEM_REQ_READ 1
#define MEM_REQ_READ_NO_CACHE 2
#define MEM_REQ_WRITE 3
#define MEM_REQ_MOVE 4
#define MEM_REQ_COPY 5
#define MEM_REQ_SCATTER 6
#define MEM_REQ_GATHER 7
#define MEM_REQ_BCAST 8
#define MEM_REQ_REDUCE 9
#define MEM_REQ_RESP 10
// Memory Response Types
#define MEM_RESP_READ 1
#define MEM_RESP_READ_NO_CACHE 2
#define MEM_RESP_WRITE 3
#define MEM_RESP_MOVE 4
#define MEM_RESP_COPY 5
#define MEM_RESP_SCATTER 6
#define MEM_RESP_GATHER 7
#define MEM_RESP_BCAST 8
#define MEM_RESP_REDUCE 9
// Types of Tick Returns
#define MEM_COMPUTED true
#define MEM_STALLED false

// NeuraMem Config
#define MEM_TOP_LEFT_PORT 0
#define MEM_TOP_RIGHT_PORT 1
#define MEM_BOTTOM_LEFT_PORT 2
#define MEM_BOTTOM_RIGHT_PORT 3



// Hash Request States
#define HR_NOT_STARTED 1
#define HR_LOAD_A_DATA_ADDR 2
#define HR_LOAD_B_DATA_ADDR 3
#define HR_LOAD_B_COL_ID_ADDR 4
#define HR_MULTIPLY 5
#define HR_HASH 6
#define HR_ACCUMULATE 7
#define HR_EVICT 8
#define HR_STORE 9
#define HR_COMPLETED 10

// Hashpad
// #define HASHPAD_COMPARATOR_COUNT 4

// Hash Request Operand IDs
#define HR_OPERAND_A_DATA_ADDR 1
#define HR_OPERAND_B_DATA_ADDR 2
#define HR_OPERAND_B_COL_ID_ADDR 3

// InstX Config
#define INSTX_TYPE_MEM_REQ 1
#define INSTX_TYPE_MEM_RESP 2
#define INSTX_TYPE_HASH_REQ 3
#define INSTX_TYPE_HASH_RESP 4


// Tile Config
// #define NEURA_TILE_X_COUNT 1
// #define NEURA_TILE_Y_COUNT 1
// #define NEURA_TILE_SIZE 4
// #define NEURA_TILE_PORT_COUNT 4

#define WORK_DONE 1
#define WORK_NOT_DONE 0

// Memory Controller Config
// #define MC_X_COUNT 1
// #define MC_Y_COUNT 1
// #define MC_PORT_COUNT 4

// Router Config
// #define NEURA_ROUTER_X_COUNT 2
// #define NEURA_ROUTER_Y_COUNT 3
// #define NEURA_ROUTER_PORT_COUNT 8
// #define NEURA_ROUTER_PROCESSING_CYCLES 0
// #define NEURA_ROUTER_BUFFER_SIZE 16
// Port IDs
#define RTR_TOP_PORT 0
#define RTR_TOP_RIGHT_PORT 1
#define RTR_RIGHT_PORT 2
#define RTR_BOTTOM_RIGHT_PORT 3
#define RTR_BOTTOM_PORT 4
#define RTR_BOTTOM_LEFT_PORT 5
#define RTR_LEFT_PORT 6
#define RTR_TOP_LEFT_PORT 7
#define RTR_EXTRA_PORT 8
// Port Directions
#define RTR_PORT_NORTH RTR_TOP_PORT
#define RTR_PORT_EAST RTR_RIGHT_PORT
#define RTR_PORT_SOUTH RTR_BOTTOM_PORT
#define RTR_PORT_WEST RTR_LEFT_PORT


// Writer Config
// #define WRITER_X_COUNT 1
// #define WRITER_Y_COUNT 1
// #define WRITER_PORT_COUNT 3

// Connection Config
#define CONN_COMPUTED true
#define CONN_STALLED false

#define COMPUTED true
#define STALLED false

// Cache Config
// #define CACHE_LINES_COUNT 4096
// #define CACHE_LINES_COUNT 16384
#define CACHE_REQUEST_TABLE_SAME_EXTENDED 1
#define CACHE_REQUEST_TABLE_NEW_CREATED 2

// Memory Controller Config
#define MC_WRITER_0_PORT 0
#define MC_WRITER_1_PORT 1
#define MC_RTR_0_PORT 2
#define MC_RTR_1_PORT 3
#define MC_RTR_2_PORT 4
#define MC_RTR_3_PORT 5
#define MC_RTR_4_PORT 6
#define MC_RTR_5_PORT 7
#define MC_RTR_6_PORT 8
#define MC_RTR_7_PORT 9


// You dont need a port for HBM
// #define MC_HBM_PORT 4

#define MC_PORT_TICKED true
#define MC_PORT_EMPTY false

// Writer Config
#define WRITER_RTR_PORT 0
#define WRITER_MC_PORT 1



// Timings
// #define DRAM_CONST_LATENCY 300
// #define IF_PROCESSING_CYCLES 1
// #define MULT_PROCESSING_CYCLES 1

// Dispatcher Config
// #define DISPATCHER_X_COUNT 1
// #define DISPATCHER_Y_COUNT 1
// Constant
// #define DISPATCH_INF_INSTX -10
// Configurable
// Just Table 0
// #define DISPATCH_INSTX_COUNT 53
// #define DISPATCH_INSTX_COUNT 118
// Entire Workload
// #define DISPATCH_INSTX_COUNT 192885
// Sample Instruction | Warning: This will never HALT


// #define DISPATCH_INSTX_COUNT DISPATCH_INF_INSTX

// #define INTERVAL 100000
// #define MIN_CYCLES 0
// #define HARD_STOP 100000000

// #define INTERMEDIATE_STAT_PRINT 10000

// Frequency 1.0 GHz
// #define FREQUENCY 1000000000

#define SUCCESS 0
#define ERROR 1

#define STANDARD_WEIGHT 1
#define PENALTY_WEIGHT 1000

#define DISPATCHER_COMPONENT_INDEX 0
#define CORE_COMPONENT_INDEX 1
#define MEM_COMPONENT_INDEX 2
#define RTR_COMPONENT_INDEX 3
#define WRITER_COMPONENT_INDEX 4
#define MC_COMPONENT_INDEX 5
#define COMPONENT_COUNT 6

// #define TX_PER_CYCLE 8

// 32 Byte align requires 5 bits
// #define ALIGN_BIT_SHIFT 5
#define ALIGN_BIT_SHIFT 6

#define MC_MAX_ACCESSED_COUNT 1000000000

#define RF_HISTOGRAM_BIN_SIZE 100



#endif // !NEURA_CONFIG_H