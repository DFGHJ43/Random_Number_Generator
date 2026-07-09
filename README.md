# Random Number Generator (RNG)

An interactive terminal-based random number generator written in C.
Zero external dependencies — uses only ANSI escape codes and the C standard library.

![](C:/Users/gfd/AppData/Roaming/marktext/images/2026-07-09-12-02-42-image.png)

## Features

- Three-column TUI layout: controls | graph | results
- Uniform distribution: random integers in [min, max]
- Normal distribution: Box-Muller transform with configurable mean/stddev
- Bernoulli distribution: 0/1 outcomes with configurable probability p
- Real-time ASCII graphs of probability density functions
- Export results to CSV with timestamp filenames
- Reproducible results via fixed random seed

## Build

```bash
gcc -Wall -Wextra -std=c99 -pedantic -Isrc -o rng-tui \
    src/tui.c src/term.c src/controls.c src/random.c \
    src/stats.c src/output.c src/graph/graph.c -lm
```

> Or `make` if you have it installed (uses the included Makefile).

## Usage

```bash
./rng-tui
```

### Key Bindings

| Key            | Action                           |
| -------------- | -------------------------------- |
| `G`            | Generate random numbers          |
| `E`            | Export results to CSV            |
| `U`            | Switch to uniform distribution   |
| `N`            | Switch to normal distribution    |
| `B`            | Switch to Bernoulli distribution |
| `Tab`          | Cycle focus between input fields |
| `Up` `Down`    | Scroll results                   |
| `Left` `Right` | Move cursor in input fields      |
| `Backspace`    | Delete character in input field  |
| `Q` / `Esc`    | Quit                             |

## Project Structure

```
src/
├── tui.c/h           Main event loop, frame rendering, results panel
├── term.c/h          Terminal control (ANSI) + keyboard input
├── controls.c/h      Left panel: parameter fields, field editing
├── graph/graph.c/h   ASCII graph rendering (uniform/normal/bernoulli)
├── random.c/h        Random number generation (rand-based)
├── stats.c/h         Statistics (min, max, mean, stddev)
└── output.c/h        Output to console / CSV file
```

## License

MIT