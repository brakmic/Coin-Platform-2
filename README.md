# Coin Platform 2

Welcome to **Coin Platform 2**, a minimalist cryptocurrency platform designed to explore and demonstrate the core concepts of blockchain technology. This project aims to provide a simplified yet functional implementation of a blockchain network, including consensus mechanisms, networking, cryptography, and storage systems.

![coin_platform_2](./gifs/coin_platform_2.gif)

## Table of Contents

- [Coin Platform 2](#coin-platform-2)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Motivation](#motivation)
  - [Project Structure](#project-structure)
    - [Overview](#overview)
    - [Components](#components)
  - [Building the Project](#building-the-project)
    - [Prerequisites](#prerequisites)
    - [Setup Instructions](#setup-instructions)
  - [Detailed Component Descriptions](#detailed-component-descriptions)
    - [Common Utilities](#common-utilities)
    - [Cryptography Module](#cryptography-module)
    - [Networking Module](#networking-module)
    - [Storage Module](#storage-module)
    - [Time Chain](#time-chain)
    - [Value Chain](#value-chain)
    - [Consensus Mechanisms](#consensus-mechanisms)
    - [Node Implementation](#node-implementation)
    - [Configuration](#configuration)
    - [Tools](#tools)
  - [Running the Node](#running-the-node)
  - [Contributing](#contributing)
  - [License](#license)

## Introduction

Coin Platform 2 is a simplified blockchain platform that implements a dual-chain architecture:

- **Time Chain**: A chain that keeps track of time and provides a global time reference for the network.
- **Value Chain**: A chain that handles transactions and maintains the ledger of account balances.

By separating time tracking from transaction processing, the platform aims to simplify consensus mechanisms and improve scalability.

## Motivation

The primary motivation behind Coin Platform 2 is educational. It serves as a learning tool for developers interested in understanding how blockchain systems work under the hood. By providing a clean and straightforward codebase, the project allows developers to:

- Explore the implementation of consensus algorithms.
- Understand networking between nodes in a decentralized network.
- Learn about cryptographic operations essential to blockchain technology.
- Experiment with storage solutions for blockchain data.
- Build and run their own minimal cryptocurrency network.

## Project Structure

### Overview

The project is organized into several modules, each responsible for a specific aspect of the platform's functionality. Here's a high-level view of the directory structure:

```
coin_platform2/
├── CMakeLists.txt
├── build/
├── cmake/
├── config/
├── src/
│   ├── CMakeLists.txt
│   ├── common/
│   ├── config/
│   ├── consensus/
│   ├── cryptography/
│   ├── main.cpp
│   ├── networking/
│   ├── node/
│   ├── storage/
│   ├── time_chain/
│   └── value_chain/
├── tests/
├── third_party/
└── tools/
```

### Components

- **common**: Contains utility functions and type definitions used throughout the project.
- **config**: Handles configuration parsing and management.
- **consensus**: Implements consensus mechanisms for both Time Chain and Value Chain.
- **cryptography**: Provides cryptographic functionalities like key generation, hashing, and signing.
- **networking**: Manages peer-to-peer networking, including connection handling and data transmission.
- **node**: Represents the node implementation that ties all components together.
- **storage**: Deals with data persistence for blockchain data.
- **time_chain**: Implements the Time Chain, which tracks time blocks.
- **value_chain**: Implements the Value Chain, which processes transactions.
- **tools**: Contains utility tools like key generators and genesis block creators.

## Building the Project

### Prerequisites

Ensure you have the following installed on your system:

- **C++17 Compiler**: GCC or Clang with C++17 support.
- **CMake (>=3.10)**: For building the project.
- **OpenSSL**: Required for cryptographic functions.
- **Git**: To clone the repository and manage submodules.

### Setup Instructions

1. **Clone the Repository**

   ```bash
   git clone https://github.com/brakmic/coin-platform-2.git
   cd coin-platform-2
   ```

2. **Initialize and Update Submodules**

   The project relies on some third-party libraries included as submodules.

   ```bash
   git submodule update --init --recursive
   ```

3. **Build the Project**

   Create a `build` directory and compile the source code using CMake.

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

   This will compile the main executable `coin_platform2` along with necessary libraries.

4. **Generate Genesis Blocks**

   Before running the node, you need to generate the genesis blocks.

   - **Generate a Private Key**

     ```bash
     ./tools/keygen/keygen
     ```

     Save the generated private key securely; you'll need it in the next step.

   - **Generate Genesis Blocks**

     ```bash
     ./tools/genesis_generator/genesis_generator <private_key_hex>
     ```

     Replace `<private_key_hex>` with the private key obtained from the previous step. This will create `genesis_blocks.hpp` in `src/common/`.

5. **Rebuild with Genesis Blocks**

   After generating the genesis blocks, recompile the project to include them.

   ```bash
   cd build
   make clean
   make
   ```

## Detailed Component Descriptions

### Common Utilities

Located in `src/common/`, this module includes:

- **types.hpp**: Defines common types like `Hash`, `PublicKey`, `PrivateKey`, etc.
- **utilities.hpp/cpp**: Provides utility functions for logging, time retrieval, and byte manipulation.
- **genesis_blocks.hpp**: Contains the serialized genesis blocks and their hashes.

### Cryptography Module

Located in `src/cryptography/`, this module handles cryptographic operations:

- **cryptography.hpp/cpp**: Implements hashing functions and key derivation.
- **schnorr_signature.hpp/cpp**: Provides functions for Schnorr signature creation and verification.

The module uses `secp256k1` library for elliptic curve operations.

### Networking Module

Located in `src/networking/`, this module manages network communications:

- **network_manager.hpp/cpp**: Handles peer connections, data transmission, and message handling.
- Uses [Asio](https://think-async.com/Asio/) for asynchronous networking.

### Storage Module

Located in `src/storage/`, this module is responsible for data persistence:

- **storage_interface.hpp**: Defines the interface for storage operations.
- **file_storage.hpp/cpp**: Implements file-based storage for blockchain data.

Blocks are stored on disk in a simple format, making it easy to inspect and debug.

### Time Chain

Located in `src/time_chain/`, this module implements the Time Chain:

- **time_block.hpp/cpp**: Defines the structure and serialization of a time block.
- **time_chain.hpp/cpp**: Manages the chain of time blocks.

The Time Chain provides a global time reference, which is essential for synchronizing events across the network.

### Value Chain

Located in `src/value_chain/`, this module implements the Value Chain:

- **transaction.hpp/cpp**: Defines the structure and verification of transactions.
- **value_block.hpp/cpp**: Defines the structure and serialization of a value block.
- **value_chain.hpp/cpp**: Manages the chain of value blocks.

The Value Chain handles transaction processing and maintains the ledger of account balances.

### Consensus Mechanisms

Located in `src/consensus/`, this module implements consensus algorithms for both chains:

- **consensus_interface.hpp**: Defines a generic interface for consensus mechanisms.
- **time_chain_consensus.hpp/cpp**: Implements consensus logic for the Time Chain.
- **value_chain_consensus.hpp/cpp**: Implements consensus logic for the Value Chain.

Consensus is achieved through a simplified stochastic process, determining which node is eligible to produce the next block.

### Node Implementation

Located in `src/node/`, this module brings all components together:

- **node.hpp/cpp**: Implements the node logic, including initialization, event handling, and main loops.

The node manages both the Time Chain and Value Chain, handling incoming messages, producing blocks, and broadcasting transactions.

### Configuration

Located in `src/config/`, this module handles application configuration:

- **config.hpp/cpp**: Parses command-line arguments and provides configuration settings to other components.

### Tools

Located in `tools/`, this directory contains utility programs:

- **keygen**: Generates a new private key.
- **genesis_generator**: Creates the genesis blocks using a provided private key.

## Running the Node

After building the project and generating the genesis blocks, you can run the node:

```bash
./coin_platform2 --port 8001 --role dual
```

Command-line options:

- `--port <port_number>`: Specifies the port on which the node listens.
- `--role <time|value|dual>`: Specifies the node's role in the network.

Example of running two nodes on the same machine for testing:

```bash
# Terminal 1
./coin_platform2 --port 8001 --role dual

# Terminal 2
./coin_platform2 --port 8002 --role dual
```

The nodes will attempt to connect to each other based on predefined logic in the `main.cpp` file.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve the project.

When contributing, please:

- Follow the existing code style.
- Write clear commit messages.
- Ensure that the project builds without errors.

## License

This project is open-source and available under the [MIT License](LICENSE.md).
