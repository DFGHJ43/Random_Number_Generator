# Random Number Generator (RNG)

A command-line random number generator written in C.

## Features

- Generate random numbers in a specified range (uniform distribution)
- Generate normally distributed random numbers (Box-Muller transform)
- Configurable random seed for reproducible results
- Output to console or file
- Statistical summary (mean, min, max, standard deviation)

## Build

```bash
make
```

## Usage

```bash
# Generate 5 uniform random numbers in [1, 100]
./rng -n 5 -l 1 -u 100

# Generate 1000 normal random numbers with mean=50, stddev=10, show stats
./rng -n 1000 -d normal -m 50 -v 10 --stats

# Generate 100 uniform random numbers and save to file
./rng -n 100 -l 0 -u 999 -o output.txt

# Reproducible results with a fixed seed
./rng -n 10 -l 1 -u 100 -s 42
```

## Options

| Option | Description |
|--------|-------------|
| `-n <count>` | Number of random numbers to generate (default: 1) |
| `-l <min>` | Lower bound for uniform distribution (default: 0) |
| `-u <max>` | Upper bound for uniform distribution (default: 100) |
| `-d <dist>` | Distribution: `uniform` (default) or `normal` |
| `-m <mean>` | Mean for normal distribution (default: 0) |
| `-s <seed>` | Random seed for reproducible results |
| `-v <stddev>` | Standard deviation for normal distribution (default: 1.0) |
| `-o <file>` | Output file path |
| `--stats` | Print statistical summary |
| `-h` | Show help message |

## License

MIT
