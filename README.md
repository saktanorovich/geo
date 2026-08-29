# geo

A set of computational-geometry problems, each with a C++ solution, and a harness that
runs those solutions against their test data.

---

## Layout

One directory per problem:

```
<problem>/
  main.cpp        the solution — reads stdin, writes stdout (never freopen)
  task-ru.docx    the problem statement, sometimes also as .pdf
  tests/          001.in + 001.out, 002.in + 002.out, ...
  tests.zip       instead of tests/ when the data is too large to keep unpacked
```

Exactly one solution is committed per problem: `main.cpp`. Drafts, alternative
implementations and notes stay out of the repo.

---

## Running

```bash
python3 check.py ./<problem> --tol=1e-6
python3 check.py ./<problem> --str
```

`check.py` compiles `<problem>/main.cpp` with `g++ -std=c++17 -O2`, feeds each
`tests/*.in` on stdin, and compares stdout against the matching `.out`, reporting a
verdict and a wall-clock time per test. The compiled binary is deleted when the run
ends, including after a failure.

> On Windows the interpreter is `python`, not `python3`. Run
> `set PYTHONIOENCODING=utf-8` first so the ✅/❌ summary prints correctly.

Each problem's required precision comes from its own statement, so the right `--tol` is
per-problem — see [REPORT.md](REPORT.md) for the current status, the exact command for
each problem, and per-test timings.

---

## Flags

| Flag | Default | Meaning |
|---|---|---|
| `--tol` | `1e-4` | numeric comparison tolerance |
| `--str` | off | compare tokens as exact strings instead |
| `--src` | `main.cpp` | source to compile, relative to the problem directory |
| `--out` | `program` | name of the compiled binary (`.exe` added on Windows) |

`--src` is how a local variant is checked against the same tests without committing it.

---

## Comparison modes

Output is compared token by token, in one of two modes implemented as separate
functions.

**Numeric** (default) — a token that parses as a number is compared with
`math.isclose(rel_tol=tol, abs_tol=tol)`; anything else falls back to exact equality.

**Exact strings** (`--str`) — every token must match verbatim, `--tol` is ignored, and
nothing is ever converted to a number.

Reach for `--str` whenever the expected output is text rather than numbers. It is not
merely a convenience: `float()` accepts `nan` and `inf`, so a text token spelled that
way is silently reinterpreted as a number, and `math.isclose(nan, nan)` is `False` — a
correct answer then fails with a nonsensical "expected nan, got nan". Symbolic output
such as LaTeX has the same problem from the other direction, since a tolerance is
meaningless for it.

---

## Packed test data

When a problem's tests are too large to keep unpacked, they are committed as
`tests.zip` instead of a `tests/` directory. No extra step is needed: `check.py`
unpacks the archive into a temporary directory, runs the suite from there, and deletes
it afterwards, so nothing unpacked is left in the working tree. A plain `tests/`
directory always takes precedence and is never touched.

---

## Other files

| File | Contents |
|---|---|
| [REPORT.md](REPORT.md) | current status: per-problem verdicts, per-test timings, and any open defects |
| `.claude/skills/problem-solver` | the workflow used to solve, verify and harden a problem here |
