# Scheduling Ships

Scheduling Ships is a project that implements a custom thread library in C called **CEThreads**, designed to manage scheduling algorithms for controlling ship traffic through a virtual canal.

## Features

- Custom thread library (**CEThreads**) similar to Pthreads.
- Support for multiple scheduling algorithms:
  - Round-Robin (RR)
  - Priority Scheduling
  - Shortest Job First (SJF)
  - First Come First Serve (FCFS)
  - Real-Time Scheduling
- Flow control mechanisms for ship traffic:
  - Equity (W ships from each side)
  - Sign-based control (direction switching)
  - Tico (free flow with collision prevention)

## Project Structure

- **CEThreads**: Custom thread library with basic thread creation, joining, and mutex functionality.
- **Schedulers**: Handles ship scheduling based on the chosen algorithm.
- **Canal Simulation**: Simulates a bidirectional canal where ships travel, with configurable flow control.

## Setup and Usage

### Tools

```bash
sudo apt install build-essential cmake ninja-build
```

## Configuration

Modify the configuration file to set parameters for the canal:

- Canal length
- Ship speed
- Number of ships
- Scheduling algorithm (RR, Priority, SJF, FCFS, Real-Time)
- Flow control method

## Design Hardware

https://docs.google.com/spreadsheets/d/19UOhnq-4k82qcbcaTn_bHiEcXRDhYykFDQgcdTzBlP0/edit?usp=sharing
