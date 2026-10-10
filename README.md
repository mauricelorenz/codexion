*This project has been created as part of the 42 curriculum by mlorenz.*

# Codexion

## Description

Codexion is a concurrency simulation written in C. One or more coders sit in a
circular co-working hub and share a set of USB dongles. To compile quantum code a
coder must hold **two** dongles at once, one in each hand. Between builds a coder
alternates through the states **compiling**, **debugging** and **refactoring**.

There are as many dongles as coders, one between each pair of neighboring coders.
A single coder has only one dongle on the table. Whenever a coder is not compiling,
their deadline keeps running: if a coder does not start a new compile within
`time_to_burnout` milliseconds, they burn out and the simulation stops.

The goal is to orchestrate the concurrent access to the dongles with POSIX threads,
mutexes and condition variables, implement the requested arbitration policy (FIFO or
EDF), respect a dongle cooldown after release, and detect a burnout precisely, all
without deadlocks, starvation or data races.

Each coder is represented by a thread. A separate monitor thread watches the
deadlines and stops the simulation either on a burnout or once every coder has
compiled at least `number_of_compiles_required` times.

## Instructions

### Build

The project only needs `cc` and `pthread`. To compile:

```bash
make
```

Other available rules: `make clean`, `make fclean` and `make re`.

### Run

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> \
           <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

| Argument | Meaning |
| --- | --- |
| `number_of_coders` | Number of coders (= number of dongles), integer > 0. |
| `time_to_burnout` | Milliseconds before a coder burns out without starting a compile, integer >= 0. |
| `time_to_compile` | Milliseconds a coder spends compiling (holding two dongles), integer >= 0. |
| `time_to_debug` | Milliseconds spent debugging, integer >= 0. |
| `time_to_refactor` | Milliseconds spent refactoring, integer >= 0. |
| `number_of_compiles_required` | The simulation stops once every coder compiled this many times, integer >= 0. |
| `dongle_cooldown` | Milliseconds a released dongle is unavailable, integer >= 0. |
| `scheduler` | `fifo` or `edf`. |

Example:

```bash
./codexion 5 800 200 200 200 3 0 edf
```

Invalid input (missing/extra arguments, negative or non-integer values, or a
scheduler other than `fifo`/`edf`) is rejected with a non-zero exit code.

### Output format

```text
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

## Resources

- [GNU C Library Manual — Threads](https://www.gnu.org/software/libc/manual/html_node/Threads.html)
  — reference for the pthread primitives (threads, mutexes, condition variables) used in the simulation
- [Operating Systems: Three Easy Pieces — Concurrency](https://pages.cs.wisc.edu/~remzi/OSTEP/)
  — book chapters on threads, locks, condition variables and concurrency bugs
- [The Little Book of Semaphores](https://greenteapress.com/wp/semaphores/)
  — classic synchronization problems, including the dining philosophers

**AI usage:** AI assistants were used for understanding concurrency concepts,
debugging race and timing issues, and drafting this README. The core
implementation and thread synchronization were done manually.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions).** The circular-wait condition is
  broken by a fixed, coder-dependent acquisition order: odd-numbered coders take
  their left dongle first, even-numbered coders take their right dongle first. A
  coder never waits for a dongle while holding it in a way that forms a cycle, so
  the classic circular wait cannot occur.
- **Starvation prevention.** Each dongle keeps a priority queue of pending
  requests. With `fifo` the queue is ordered by arrival time. With `edf` it is
  ordered by deadline `last_compile_start + time_to_burnout`, with arrival order as
  tie-breaker. Under feasible parameters every coder eventually obtains the two
  dongles they need.
- **Dongle cooldown.** After a coder releases a dongle, the dongle stores an
  `available_at` timestamp and cannot be granted again before
  `dongle_cooldown` milliseconds have elapsed. A waiting coder sleeps with a timed
  wait until that moment.
- **Precise burnout detection.** The monitor computes the earliest deadline among
  the coders that are still working and sleeps on a condition variable with a
  timeout set to exactly that deadline, waking early on every state change. This
  keeps the detection (and the log) within the required 10 ms window.
- **Finished coders are ignored.** Once a coder has reached
  `number_of_compiles_required`, they are excluded from the burnout check, so a
  coder that has finished can never be reported as burned out while the others
  keep compiling.
- **Serialized logging.** All output goes through a single log mutex so two
  messages never interleave on one line. Once a burnout has been printed, a
  "stopped" flag seals the output so no further state message can appear after the
  `burned out` line.
- **Single coder.** With one coder there is a single dongle, so two-dongle
  compilation is impossible. The coder takes the one dongle and burns out, as
  required.

## Thread synchronization mechanisms

The simulation uses two kinds of synchronization primitives.

- **`pthread_mutex_t`**
  - one mutex per dongle, protecting the dongle's state (`taken`, `available_at`)
    and its request heap
  - `log_mutex`, serializing all writes to standard output
  - `state_mutex`, protecting the shared simulation state (`stop`,
    `compiles_done`, `last_compile_start`)
- **`pthread_cond_t`**
  - one condition variable per dongle, on which coders wait until they may take
    the dongle
  - `state_cond`, used by the monitor to sleep until the next deadline and by the
    coders to sleep during compile/debug/refactor, and broadcast on every state
    change

`state_mutex` and `state_cond` also form the coder/monitor channel: coders do not
communicate with each other. They publish their progress (`last_compile_start`,
`compiles_done`) under `state_mutex` and broadcast `state_cond`. The monitor reads
the same variables under `state_mutex` and recomputes the earliest deadline, so a
burnout is always based on up-to-date information.

How races are prevented:

- The dongle heap is only ever touched while holding that dongle's mutex, so the
  arbitration order is consistent and the heap cannot be corrupted by concurrent
  requests.
- Coder state and the `stop` flag are only read/written under `state_mutex`. The
  monitor and the coders therefore always see a consistent snapshot.
- Logging is atomic with respect to other threads due to `log_mutex`, and the
  burnout line is additionally protected by the `log_stopped` flag so that the
  stop/burnout sequence is race-free.
- A coder blocked in `take_dongle` checks the `stop` flag on every wake-up and
  returns immediately when the simulation has ended, so no thread stays blocked
  after a burnout.
