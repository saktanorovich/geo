# 📐 geo

A set of computational-geometry problems, each with a C++ solution, and a harness that
runs those solutions against their test data.

---

## 📁 Layout

One directory per problem:

```
<problem>/
  main.cpp        the solution — reads stdin, writes stdout (never freopen)
  task-ru.docx    the problem statement, sometimes also as .pdf
  tests/          001.in + 001.out, 002.in + 002.out, ...  (quad uses 001.md)
                  or 001-010.zip, 011-020.zip, ... when the data is too large to keep unpacked
```

Exactly one solution is committed per problem: `main.cpp`. Drafts, alternative
implementations and notes stay out of the repo.

---

## ▶️ Running

`check.py` compiles `<problem>/main.cpp` with `g++ -std=c++17 -O2`, feeds each input on
stdin and compares stdout against the matching expected file, reporting a verdict and a
wall-clock time per test. The binary it builds is always `program`, deleted when the run
ends, including after a failure.

Before the first timed test it runs that binary once against a temporary `000` input,
untimed and unreported. The first run of a newly built executable pays a one-off cost
the OS charges for validating and scanning it — some 180 ms on macOS, 25-45 ms for
Defender on Windows, cached per file afterwards — and since every run recompiles, this
keeps that cost off test `001` instead of letting it pass for solve time. The verdict is
cached per binary, so only a run of the same executable absorbs it; a separate warm-up
program would leave the real one just as cold.

The precision each problem needs comes from its own statement, so there is one command
per problem rather than a single suite-wide one:

```bash
set PYTHONIOENCODING=utf-8        # Windows only, for the emoji summary line
python check.py ./gsm         --tol=1e-5
python check.py ./parabola    --tol=1e-6
python check.py ./pool        --tol=1e-3
python check.py ./quad        --str --out md
python check.py ./slalom      --tol=1e-3
python check.py ./tournament  --str
python check.py ./triopt      --tol=1e-12
python check.py ./water       --tol=1e-3
```

Or in one go, from the repo root, in bash or zsh (`eval` splits each entry into its
arguments, which zsh would otherwise pass as one):

```bash
for p in "gsm --tol=1e-5" "parabola --tol=1e-6" "pool --tol=1e-3" \
         "quad --str --out md" "slalom --tol=1e-3" "tournament --str" \
         "triopt --tol=1e-12" "water --tol=1e-3"; do
  eval "python check.py ./$p"
done
```

Use `python3` where the interpreter is not on PATH as `python`. Expected verdicts and
per-test timings are in [REPORT.md](REPORT.md); `parabola` is the one red suite, and
deliberately so — its 15 failing tests document an open defect, not a regression.

### 🍎 On macOS

macOS ships `python3` but no `python`, and its default shell is zsh. The commands above
work there as written once `python` becomes `python3` (and `PYTHONIOENCODING` is not
needed). One problem at a time:

```bash
python3 check.py ./gsm         --tol=1e-5
python3 check.py ./parabola    --tol=1e-6
python3 check.py ./pool        --tol=1e-3
python3 check.py ./quad        --str --out md
python3 check.py ./slalom      --tol=1e-3
python3 check.py ./tournament  --str
python3 check.py ./triopt      --tol=1e-12
python3 check.py ./water       --tol=1e-3
```

Or in one go, from the repo root:

```bash
for p in "gsm --tol=1e-5" "parabola --tol=1e-6" "pool --tol=1e-3" \
         "quad --str --out md" "slalom --tol=1e-3" "tournament --str" \
         "triopt --tol=1e-12" "water --tol=1e-3"; do
  eval "python3 check.py ./$p"
done
```

`g++` is Apple clang, from Xcode or the Command Line Tools (`xcode-select --install`).
If every problem stops at `Compilation failed ❌` with a linker error such as
`tapi error: malformed file … unknown architecture arm64e.x1`, the compiler and the
macOS SDK come from different releases — typically an older Xcode next to newer
Command Line Tools. Point the compiler at an SDK it understands, for the session or in
`~/.zshrc`:

```bash
ls /Library/Developer/CommandLineTools/SDKs/            # pick an older MacOSX<ver>.sdk
export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk
```

The lasting fix is to bring the two back in step: update Xcode, or switch to the
Command Line Tools' own compiler with
`sudo xcode-select -s /Library/Developer/CommandLineTools`.

---

## 🚩 Flags

| Flag | Default | Meaning |
|---|---|---|
| `--tol` | `1e-4` | numeric comparison tolerance |
| `--str` | off | compare tokens as exact strings instead |
| `--src` | `main.cpp` | source to compile, relative to the problem directory |
| `--in` | `in` | extension of the input files (`in`, `inp`, ...) |
| `--out` | `out` | extension of the expected-output files (`out`, `md`, ...) |

`--src` checks a local variant against the same tests without committing it. `--in` and
`--out` name the test-file extensions when a problem departs from the `.in` / `.out`
default, as `quad` does with Markdown; a leading dot is accepted either way.

---

## ⚖️ Comparison modes

Output is compared token by token, in one of two modes. **Numeric** (default) compares
anything that parses as a number with `math.isclose(rel_tol=tol, abs_tol=tol)` and falls
back to exact equality for the rest. **Exact strings** (`--str`) matches every token
verbatim, ignores `--tol`, and never calls `float()`.

Reach for `--str` whenever the expected output is text. `float()` accepts `nan` and
`inf`, so a name spelled that way is silently read as a number and `isclose(nan, nan)`
is `False` — a correct answer then fails as "expected nan, got nan". Symbolic output
such as LaTeX has the same problem from the other side, a tolerance being meaningless
for it.

---

## 📦 Packed test data

Tests too large to keep unpacked are committed packed, as archives of ten tests each
inside `tests/` (`001-010.zip`, `011-020.zip`, ...), which keeps every file well under
GitHub's 50 MB warning. No extra step is needed: `check.py` unpacks all of them to a
temporary directory, runs the suite from there and deletes it afterwards. Plain
`.in` files in `tests/` always take precedence and are never touched; a single
`tests.zip` next to the solution is still accepted too.

---

## 📄 Other files

| File | Contents |
|---|---|
| [REPORT.md](REPORT.md) | current status: per-problem verdicts, per-test timings, and any open defects |
| `.claude/skills/problem-solver` | the workflow used to solve, verify and harden a problem here |
