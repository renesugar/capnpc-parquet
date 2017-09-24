capnpc-parquet
==============

capnpc-parquet is a Cap'n Proto compiler plugin that converts a Cap'n Proto schema to a Parquet schema.

Reference:

[Cap'n Proto Schema Language](https://capnproto.org/language.html)

# Table of Contents

- [License](#license)
- [Building](#building)
- [Using](#using)

# License

This project is licensed under the [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0).

# Building

capnpc-parquet uses CMake for its build system.

    mkdir build
    cd build
    cmake ..
    make

# Using

The basic command line to use this compiler plugin is:

    capnpc -o ./capnpc-parquet file.capnp

This plugin prints out the generated Parquet schema.

Possible uses:

1) Write out a program that reads/writes a Parquet file using the compiled schema. The coded generated could use the Parquet-Cpp or Arrow libraries.

2) Write out a program that generates a Parquet file filed with randomly generated data using the compiled schema.
