CPP-Utils Python Package
===========

## Information

This package was created by starting C ++ methods to incorporate into my python implementations.

To make things easier for me, lightweight public libraries were included (especially to carry out string operations):
- hash-library
- strtk

The following methods have currently been implemented:
- ini_load (flexible and ligthweigh ini to dict parser, Faster Than Configparser)
- eval_type (method to parse strings in python-types e.g. int | bool | timestamp
- simple_hmac (vectorized c++ hmac implementation)
- default_ca_path (python function to find a default ssl / ca certificate path)

In the future the following will be implemented:
- more hash methods
- c++ optimized grep methods
- c++ optimized url-tools methods

Currently, the package was only tested for Linux

## Installation
```bash
pip install cpputils
```

## Development

### Prerequisites

* A compiler with C++17 support
* Pip 10+ or CMake >= 3.4 (or 3.8+ on Windows, which was the first version to support VS 2015)
* Ninja or Pip 3.8+

### Installation

Just clone this repository and pip install. Note the `--recursive` option which is
needed for the pybind11 submodule:

```bash
git clone --recursive https://gitlab.blubblub.tech/sgeist/cpputils.git
pip install .
```

### Publish
```bash
twine upload -r pypi $(ls dist/cpputils-1.0.0-*.whl) --verbose
```
