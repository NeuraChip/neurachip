# neurachip_deploy
NeuraChip Deploy Repository

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

#### Not using WSL, 

You likely have systemd, so you can start it with:

```bash
systemctl start mongod
```

#### Mongo DB on WSL

Must be started on the background with:

```bash
sudo -u mongodb mongod --config /etc/mongod.conf --fork
```

### NeuraChip

```bash
cd /home/ktb/git/NeuraChip/NeuraSim

```bash
./compile_run.sh

```bash
./chippy.bin

