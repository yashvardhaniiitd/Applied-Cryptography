# Assignments of Applied Cryptography

This repo contains all the assignments I did in my course of Applied Cryptography. 

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Cloning the Repository](#cloning-the-repository)
  - [Compiling the Code](#compiling-the-code)
- [Usage](#usage)
  - [Running the Program](#running-the-program)
  - [Customizing the Simulation](#customizing-the-simulation)
- [Makefile](#makefile)
  - [Makefile Structure](#makefile-structure)
  - [Commands](#commands)
- [Results](#results)
- [License](#license)

## Project Overview

This program simulates WiFi performance across three different WiFi generations: **WiFi 4**, **WiFi 5**, and **WiFi 6**. The simulation focuses on key network metrics:
- **Throughput**: The data transfer rate in Mbps.
- **Latency**: The delay experienced in the network, calculated as average and maximum latency.
  
Each WiFi generation has its own throughput model, taking into account the number of users and the corresponding random variability in the throughput.

### Key Features:
- **Simulation of WiFi 4, 5, and 6**: Models the performance for different WiFi technologies.
- **Randomized Throughput**: Introduces random factors to simulate real-world variability in WiFi performance.
- **Customizable User Input**: Users can choose predefined scenarios or enter a custom number of users.
- **Formatted Output**: Results are displayed in a clean tabular format, making it easy to compare the performance of different WiFi types across various user counts.

## Features

- **WiFi 4 Simulation**: Simulates the throughput and latency for WiFi 4 with a base throughput of 150 Mbps and a random variability factor.
- **WiFi 5 Simulation**: Simulates the throughput and latency for WiFi 5 with a higher base throughput of 200 Mbps and an efficiency factor.
- **WiFi 6 Simulation**: Simulates the throughput and latency for WiFi 6, offering the highest throughput, influenced by user count and an OFDMA efficiency factor.
- **Custom User Count**: Users can test the system with a custom number of users beyond the predefined scenarios of 1, 10, and 100 users.
- **Formatted Output**: Outputs the results in a table with user count, WiFi type, throughput, average latency, and max latency.

## Installation

### Prerequisites

To build and run the simulation, you will need:
- **C++ Compiler**: A C++ compiler that supports C++17 or later (e.g., g++ for Linux, MinGW for Windows).
- **Make**: A build automation tool (available on most Unix-based systems).
- **C++17 Standard**: Ensure your compiler supports C++17 or higher.

## Took help of GenAI


### Cloning the Repository

To start using this project, first, clone the repository to your local machine:

```bash
git clone https://github.com/your-username/wifi-simulation.git
cd wifi-simulation


