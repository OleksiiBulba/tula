# Tula Language Implementation

Tula (**Tu**ring **La**nguage) is an [Esoteric Programming Language](https://en.wikipedia.org/wiki/Esoteric_programming_language) based on [Turing Machine](https://en.wikipedia.org/wiki/Turing_machine) and extended with [Set Theory](https://en.wikipedia.org/wiki/Set_theory).

This repository contains an implementation of Tula, a Turing Language, using C, Flex, and Bison. Tula allows for the description and execution of Turing machines with a simplified, readable syntax.

## Introduction

Tula, as specified [here](https://raw.githubusercontent.com/tsoding/tula/main/README.md), is designed to facilitate the description of Turing machine behavior using a simple textual language. This implementation aims to provide a straightforward compiler and runtime for Tula scripts, leveraging C for the backend processing, Flex for lexical analysis, and Bison for parsing.

## Features

Currently, this implementation supports a limited set of features:
- **Cases**: Define transition cases for the Turing machine.
- **States**: Allow specification of different states within the Turing machine.
- **Tula Tape**: Define the initial tape contents.

### Not Supported
- S-expressions
- Loops
- Sets
- Compound Expressions
- Operations

These features are planned for future releases as the project evolves.

## Installation

To install and run this Tula implementation, follow these steps:

### Prerequisites
- GCC (GNU Compiler Collection)
- Bison (GNU Parser Generator)
- Flex (Fast Lexical Analyzer)

### Steps
1. Clone the repository:
```bash
   git clone https://git@github.com:OleksiiBulba/tula.git <repo-dir>
   cd <repo-dir>
```
2. Build the project:
```bash
make
```
3. Clean the project:
```bash
make clean
```

## Usage
To run a Tula script, use the following command:
```bash
./bin/tula path/to/script.tula
./bin/tula examples/increment.tula
```
Ensure your script follows the limitations of the currently supported features.

## Contributing

Contributions to extend the functionality of this Tula implementation are welcome. Particularly, efforts to implement the unsupported features would greatly enhance this project.

Before contributing, please ensure to read the contributing guidelines (not yet provided). You can submit pull requests via GitHub.

## License

This project is licensed under the MIT License - see the [LICENSE.md](./LICENSE.md) file for details.

## Acknowledgments

* Inspired by the original Tula specification by Tsoding: [Tula](https://github.com/tsoding/tula)
