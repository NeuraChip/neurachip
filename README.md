# NeuraChip

NeuraChip is a GNN accelerator that is built using the NeuraSim simulator. NeuraSim is a cycle accurate simulator that contains six modules as follows:

1. **NeuraSim Engine**: The cycle accurate simulator engine built in C++.
2. **NeuraCompiler**: A pythonic compiler that takes graphs as inputs and generates a binary to be executed by the NeuraSim Engine. The NeuraCompiler uses an extended x86 ISA.
3. **Mongo**: The Mongo module is a part of NeuraSim Engine that is coupled with an instance of MongoDB server. Throughout the simulation, NeuraSim pushes performance metrics to the MongoDB server.
4. **NeuraViz**: A pythonic module that generates all plots to visualize the data. NeuraViz is coupled with MongoDB and fetches simulation metrics from MongoDB server.
5. **Dashboard**: For ease of use, we deploy NeuraChip on our servers and make it available to the community over a WebGUI called Dashboard. The web Dashboard can be accessed over [https://neurachip.us](https://neurachip.us).
6. **DRAMSim3**: NeuraChip utilizes the memory simulator provided by Li et. al., called DRAMSim3 for computing HBM memory request latencies.

# Installation and Usage

Clone and enter the repository:

```bash
git clone https://github.com/NeuraChip/neurachip.git
cd neurachip
```

### Docker

```bash
./build-docker.sh
```

```bash
# This starts the container with the --rm flag, 
# so it will be removed when you stop or exit the container
./start-docker.sh
```

The commands below are to be executed inside the container.

### Mongo DB

NeuraSim requires a MongoDB instance to store the simulation results.
So we need to start a MongoDB instance.


#### Mongo DB Server Launch

Must be started on the background with:

```bash
sudo -u mongodb mongod --config /etc/mongod.conf --fork
```

### NeuraChip

Change directory to NeuraSim
```bash
cd /home/ktb/git/NeuraChip/NeuraSim
```

Compile NeuraSim
```bash
./compile_run.sh
```

Run NeuraChip Simulation
```bash
./chippy.bin
```

