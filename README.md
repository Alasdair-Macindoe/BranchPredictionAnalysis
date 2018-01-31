# CS4202-P1

# Prerequisites
Both `gzip`, `gcc` and `C++14` are required.
NB: Data is not provided. 

# Summary
To replicate the results of the experimentation automatically do the following from root directory.

```
./install.sh
cd src
./experiment.sh
```

# Installation
Execute the following command from the root directory to install all necessary files:

```
./install.sh
```

## Manual
All commands should be done from root directory.

### Decompress Trace Files

```
cd data
gunzip *
```

### Install framework

```
cd src
make
```

### Install benchmarks

```
cd src/programs
make
```

**NOTE**: Because `PinTools` is not distributed with this if you want to recreate
the trace outputs you will have to do that based upon your installation.
This has not been automated. Further note that the results *may* differ because of hardware differences.

# Running Experiments

```
cd src
./experiments.sh
```

## Manually

```
cd src
./experiment.out "<file>"
```
