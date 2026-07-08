# Random Number Generator (RNG)

An interactive terminal-based random number generator written in C.

## Features

- Interactive TUI with left-right split layout
- Generate uniformly distributed random numbers in a specified range
- Generate normally distributed random numbers (Box-Muller transform)
- Real-time ASCII graph of the probability density function
- Export results to CSV file
- Statistical summary (mean, min, max, standard deviation)

## Build

```bash
make
```

Or manually:

```bash
gcc -Wall -Wextra -std=c99 -pedantic -o rng-tui src/tui.c src/random.c src/stats.c src/output.c -lm
```

## Usage

```bash
./rng-tui
```

### Key Bindings

| Key | Action |
|-----|--------|
| `G` | Generate random numbers |
| `E` | Export results to CSV |
| `U` | Switch to uniform distribution |
| `N` | Switch to normal distribution |
| `Tab` | Cycle focus between input fields |
| `↑` `↓` | Navigate fields / scroll results |
| `Q` / `Esc` | Quit |

## License

MIT
