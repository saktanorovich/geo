# geo — Test Report

**Machines:** every table below carries one column per machine.

| | Platform | Toolchain | Status |
|---|---|---|---|
| **Windows** | Windows 11 Pro (10.0.26200) | g++ 15.2.0 (MSYS2), `-std=c++17 -O2` · Python 3.13.13 | measured — all 8 problems, best of 5 runs per test, warm-up in place |
| **macOS** | macOS 26.6.2 (Darwin 25.6.0), Apple silicon | Apple clang 17.0.0, `-std=c++17 -O2` · Python 3.14.0 | measured — all 8 problems, best of 5 runs per test, warm-up in place |

## Verdict — 552/567 ❌

| Problem | Tests | Compare | Result | macOS max | Windows max | Status |
|---|---:|---|---|---:|---:|:---:|
| [`gsm`](#gsm) | 100 | `--tol=1e-5` | 100/100 | 82 ms | 110 ms | ✅ |
| [`parabola`](#parabola) | 79 | `--tol=1e-6` | 64/79 | 337 ms | 209 ms | ❌ |
| [`pool`](#pool) | 99 | `--tol=1e-3` | 99/99 | 3 ms | 5 ms | ✅ |
| [`quad`](#quad) | 60 | `--str --out md` | 60/60 | 3 ms | 7 ms | ✅ |
| [`slalom`](#slalom) | 10 | `--tol=1e-3` | 10/10 | 16 ms | 6 ms | ✅ |
| [`tournament`](#tournament) | 140 | `--str` | 140/140 | 72 ms | 197 ms | ✅ |
| [`triopt`](#triopt) | 65 | `--tol=1e-12` | 65/65 | 24 ms | 30 ms | ✅ |
| [`water`](#water) | 14 | `--tol=1e-3` | 14/14 | 4 ms | 7 ms | ✅ |
| **Total** | **567** | | **552/567** | — | — | **❌** |

The verdicts are machine-independent and identical on both: 552/567, with the same 15
`parabola` tests red. Only the timings differ.

Times are end-to-end including process spawn, so the 4-5 ms floor on Windows and the
1-2 ms floor on the Mac are spawn overhead rather than solve time. The first spawn of a
freshly compiled binary costs far more — the OS validates the new file and scans it,
then caches that verdict per file — some 180 ms on macOS against 30-49 ms on Windows.
`check.py` recompiles on every run, so that cost is unavoidable; `warm_up()` spends it
on an untimed run against a temporary `000` before the suite starts, and only the real
tests are reported.

Both columns are measured that way, so every maximum below is real work rather than a
cold start: `parabola` `060` costs 337 ms on the Mac and 209 ms on Windows, `gsm` `014`
82 ms and 110 ms, `tournament`'s heaviest test 72 ms and 197 ms. Each cell is the best of
five runs per test.

Tests use a 3-digit schema (`001.in` / `001.out`), except `quad`, whose expected outputs
are `001.md`. `quad` and `tournament` compare as **strings** (`--str`) because their
answers are not numeric — nine `tournament` tests hold a sith named `inf` or `nan`,
which `float()` would reinterpret and fail on as `nan != nan`.

**`parabola` (64/79) fails on purpose.** Those 15 tests document a known defect its
original tests never exercised, kept red so it stays visible; each is marked ❌ in the
grid and listed with its error in the section.

---

## Harness

`check.py` compiles each `<folder>/main.cpp` with `g++ -std=c++17 -O2`, feeds every
`tests/*.in` on stdin and compares stdout against the matching expected file, timing
each test end-to-end. Solutions read stdin and write stdout — `freopen` produces no
output under it.

The flags, the two comparison modes and the packed-`tests.zip` handling are documented
in [README.md](README.md), along with the command each problem runs with.

---

## <a id="gsm"></a>`gsm` — 100/100 ✅

Shortest path between two points that stays inside GSM coverage circles.

```bash
python3 check.py ./gsm --tol=1e-5
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `051` ✅ | 2 ms | 6 ms |
| `002` ✅ | 2 ms | 5 ms | `052` ✅ | 1 ms | 5 ms |
| `003` ✅ | 3 ms | 5 ms | `053` ✅ | 1 ms | 5 ms |
| `004` ✅ | 2 ms | 5 ms | `054` ✅ | 2 ms | 6 ms |
| `005` ✅ | 2 ms | 5 ms | `055` ✅ | 1 ms | 5 ms |
| `006` ✅ | 2 ms | 5 ms | `056` ✅ | 1 ms | 5 ms |
| `007` ✅ | 2 ms | 5 ms | `057` ✅ | 1 ms | 5 ms |
| `008` ✅ | 2 ms | 5 ms | `058` ✅ | 1 ms | 5 ms |
| `009` ✅ | 2 ms | 5 ms | `059` ✅ | 1 ms | 5 ms |
| `010` ✅ | 2 ms | 5 ms | `060` ✅ | 1 ms | 5 ms |
| `011` ✅ | 2 ms | 5 ms | `061` ✅ | 1 ms | 5 ms |
| `012` ✅ | 2 ms | 5 ms | `062` ✅ | 1 ms | 5 ms |
| `013` ✅ | 2 ms | 5 ms | `063` ✅ | 1 ms | 5 ms |
| `014` ✅ | 82 ms | 110 ms | `064` ✅ | 1 ms | 5 ms |
| `015` ✅ | 80 ms | 110 ms | `065` ✅ | 1 ms | 4 ms |
| `016` ✅ | 2 ms | 5 ms | `066` ✅ | 1 ms | 5 ms |
| `017` ✅ | 2 ms | 5 ms | `067` ✅ | 1 ms | 5 ms |
| `018` ✅ | 1 ms | 5 ms | `068` ✅ | 1 ms | 5 ms |
| `019` ✅ | 1 ms | 5 ms | `069` ✅ | 1 ms | 5 ms |
| `020` ✅ | 1 ms | 5 ms | `070` ✅ | 1 ms | 5 ms |
| `021` ✅ | 1 ms | 5 ms | `071` ✅ | 1 ms | 5 ms |
| `022` ✅ | 1 ms | 5 ms | `072` ✅ | 1 ms | 5 ms |
| `023` ✅ | 1 ms | 5 ms | `073` ✅ | 1 ms | 5 ms |
| `024` ✅ | 1 ms | 5 ms | `074` ✅ | 1 ms | 5 ms |
| `025` ✅ | 1 ms | 5 ms | `075` ✅ | 1 ms | 5 ms |
| `026` ✅ | 2 ms | 6 ms | `076` ✅ | 1 ms | 5 ms |
| `027` ✅ | 6 ms | 13 ms | `077` ✅ | 1 ms | 5 ms |
| `028` ✅ | 51 ms | 89 ms | `078` ✅ | 1 ms | 5 ms |
| `029` ✅ | 2 ms | 7 ms | `079` ✅ | 1 ms | 5 ms |
| `030` ✅ | 1 ms | 5 ms | `080` ✅ | 1 ms | 5 ms |
| `031` ✅ | 1 ms | 5 ms | `081` ✅ | 1 ms | 5 ms |
| `032` ✅ | 1 ms | 5 ms | `082` ✅ | 1 ms | 5 ms |
| `033` ✅ | 1 ms | 5 ms | `083` ✅ | 1 ms | 5 ms |
| `034` ✅ | 1 ms | 5 ms | `084` ✅ | 1 ms | 5 ms |
| `035` ✅ | 1 ms | 5 ms | `085` ✅ | 1 ms | 5 ms |
| `036` ✅ | 1 ms | 5 ms | `086` ✅ | 1 ms | 5 ms |
| `037` ✅ | 1 ms | 5 ms | `087` ✅ | 1 ms | 5 ms |
| `038` ✅ | 1 ms | 5 ms | `088` ✅ | 1 ms | 5 ms |
| `039` ✅ | 1 ms | 5 ms | `089` ✅ | 1 ms | 5 ms |
| `040` ✅ | 2 ms | 6 ms | `090` ✅ | 1 ms | 5 ms |
| `041` ✅ | 1 ms | 5 ms | `091` ✅ | 1 ms | 5 ms |
| `042` ✅ | 1 ms | 5 ms | `092` ✅ | 1 ms | 4 ms |
| `043` ✅ | 1 ms | 5 ms | `093` ✅ | 1 ms | 5 ms |
| `044` ✅ | 1 ms | 5 ms | `094` ✅ | 1 ms | 5 ms |
| `045` ✅ | 1 ms | 5 ms | `095` ✅ | 1 ms | 5 ms |
| `046` ✅ | 1 ms | 5 ms | `096` ✅ | 1 ms | 5 ms |
| `047` ✅ | 2 ms | 6 ms | `097` ✅ | 1 ms | 5 ms |
| `048` ✅ | 1 ms | 5 ms | `098` ✅ | 1 ms | 5 ms |
| `049` ✅ | 1 ms | 5 ms | `099` ✅ | 1 ms | 5 ms |
| `050` ✅ | 1 ms | 5 ms | `100` ✅ | 1 ms | 4 ms |

---
## <a id="parabola"></a>`parabola` — 64/79 ❌

Shortest path through parabolic teleporters.

```bash
python3 check.py ./parabola --tol=1e-6
```

The 15 failing tests, every one overshooting:

| Test | Expected | Reported | Error |
|---|---:|---:|---:|
| `065` | 18.964406 | 25.578710 | +6.614304 |
| `066` | 95.523737 | 97.583898 | +2.060161 |
| `067` | 73.703972 | 75.737242 | +2.033270 |
| `068` | 90.935892 | 92.709621 | +1.773729 |
| `069` | 103.809467 | 105.523070 | +1.713603 |
| `070` | 54.448698 | 55.990833 | +1.542135 |
| `071` | 124.094575 | 125.525730 | +1.431155 |
| `072` | 797483.531503 | 797484.483125 | +0.951622 |
| `073` | 108.190780 | 109.089875 | +0.899095 |
| `074` | 113.254390 | 113.906590 | +0.652200 |
| `075` | 115.842903 | 116.437157 | +0.594255 |
| `076` | 66.722611 | 67.266643 | +0.544032 |
| `077` | 19.222906 | 19.692521 | +0.469615 |
| `078` | 57.253445 | 57.701341 | +0.447896 |
| `079` | 55.188396 | 55.511676 | +0.323280 |

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `041` ✅ | 1 ms | 5 ms |
| `002` ✅ | 3 ms | 5 ms | `042` ✅ | 1 ms | 5 ms |
| `003` ✅ | 2 ms | 5 ms | `043` ✅ | 1 ms | 5 ms |
| `004` ✅ | 2 ms | 5 ms | `044` ✅ | 1 ms | 5 ms |
| `005` ✅ | 2 ms | 5 ms | `045` ✅ | 1 ms | 5 ms |
| `006` ✅ | 2 ms | 5 ms | `046` ✅ | 1 ms | 5 ms |
| `007` ✅ | 2 ms | 5 ms | `047` ✅ | 1 ms | 5 ms |
| `008` ✅ | 2 ms | 5 ms | `048` ✅ | 1 ms | 5 ms |
| `009` ✅ | 2 ms | 5 ms | `049` ✅ | 1 ms | 5 ms |
| `010` ✅ | 1 ms | 5 ms | `050` ✅ | 3 ms | 5 ms |
| `011` ✅ | 2 ms | 5 ms | `051` ✅ | 10 ms | 10 ms |
| `012` ✅ | 2 ms | 5 ms | `052` ✅ | 15 ms | 11 ms |
| `013` ✅ | 1 ms | 5 ms | `053` ✅ | 29 ms | 18 ms |
| `014` ✅ | 1 ms | 5 ms | `054` ✅ | 46 ms | 25 ms |
| `015` ✅ | 2 ms | 5 ms | `055` ✅ | 83 ms | 47 ms |
| `016` ✅ | 1 ms | 5 ms | `056` ✅ | 112 ms | 60 ms |
| `017` ✅ | 1 ms | 5 ms | `057` ✅ | 132 ms | 66 ms |
| `018` ✅ | 1 ms | 5 ms | `058` ✅ | 152 ms | 70 ms |
| `019` ✅ | 1 ms | 5 ms | `059` ✅ | 193 ms | 89 ms |
| `020` ✅ | 1 ms | 5 ms | `060` ✅ | 337 ms | 209 ms |
| `021` ✅ | 1 ms | 5 ms | `061` ✅ | 337 ms | 187 ms |
| `022` ✅ | 1 ms | 5 ms | `062` ✅ | 86 ms | 28 ms |
| `023` ✅ | 1 ms | 5 ms | `063` ✅ | 92 ms | 44 ms |
| `024` ✅ | 1 ms | 5 ms | `064` ✅ | 86 ms | 28 ms |
| `025` ✅ | 1 ms | 5 ms | `065` ❌ | 1 ms | 5 ms |
| `026` ✅ | 1 ms | 5 ms | `066` ❌ | 1 ms | 5 ms |
| `027` ✅ | 1 ms | 5 ms | `067` ❌ | 1 ms | 5 ms |
| `028` ✅ | 1 ms | 5 ms | `068` ❌ | 1 ms | 5 ms |
| `029` ✅ | 1 ms | 5 ms | `069` ❌ | 1 ms | 5 ms |
| `030` ✅ | 1 ms | 5 ms | `070` ❌ | 1 ms | 5 ms |
| `031` ✅ | 1 ms | 5 ms | `071` ❌ | 1 ms | 5 ms |
| `032` ✅ | 1 ms | 5 ms | `072` ❌ | 1 ms | 5 ms |
| `033` ✅ | 1 ms | 5 ms | `073` ❌ | 1 ms | 5 ms |
| `034` ✅ | 1 ms | 5 ms | `074` ❌ | 1 ms | 5 ms |
| `035` ✅ | 1 ms | 5 ms | `075` ❌ | 1 ms | 5 ms |
| `036` ✅ | 1 ms | 5 ms | `076` ❌ | 1 ms | 5 ms |
| `037` ✅ | 1 ms | 5 ms | `077` ❌ | 1 ms | 5 ms |
| `038` ✅ | 1 ms | 5 ms | `078` ❌ | 1 ms | 5 ms |
| `039` ✅ | 1 ms | 5 ms | `079` ❌ | 1 ms | 5 ms |
| `040` ✅ | 1 ms | 5 ms |  |  |  |

---
## <a id="pool"></a>`pool` — 99/99 ✅

Radius of water spread from a source point in an `n x m` container.

```bash
python3 check.py ./pool --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `051` ✅ | 1 ms | 5 ms |
| `002` ✅ | 2 ms | 5 ms | `052` ✅ | 1 ms | 5 ms |
| `003` ✅ | 2 ms | 5 ms | `053` ✅ | 1 ms | 5 ms |
| `004` ✅ | 2 ms | 5 ms | `054` ✅ | 1 ms | 5 ms |
| `005` ✅ | 2 ms | 5 ms | `055` ✅ | 1 ms | 5 ms |
| `006` ✅ | 2 ms | 5 ms | `056` ✅ | 1 ms | 5 ms |
| `007` ✅ | 1 ms | 5 ms | `057` ✅ | 1 ms | 5 ms |
| `008` ✅ | 2 ms | 5 ms | `058` ✅ | 1 ms | 5 ms |
| `009` ✅ | 1 ms | 5 ms | `059` ✅ | 1 ms | 5 ms |
| `010` ✅ | 1 ms | 5 ms | `060` ✅ | 1 ms | 5 ms |
| `011` ✅ | 1 ms | 5 ms | `061` ✅ | 1 ms | 5 ms |
| `012` ✅ | 1 ms | 5 ms | `062` ✅ | 1 ms | 5 ms |
| `013` ✅ | 1 ms | 5 ms | `063` ✅ | 1 ms | 5 ms |
| `014` ✅ | 1 ms | 5 ms | `064` ✅ | 1 ms | 5 ms |
| `015` ✅ | 1 ms | 5 ms | `065` ✅ | 1 ms | 5 ms |
| `016` ✅ | 1 ms | 5 ms | `066` ✅ | 1 ms | 5 ms |
| `017` ✅ | 1 ms | 5 ms | `067` ✅ | 1 ms | 5 ms |
| `018` ✅ | 1 ms | 5 ms | `068` ✅ | 1 ms | 5 ms |
| `019` ✅ | 1 ms | 5 ms | `069` ✅ | 1 ms | 5 ms |
| `020` ✅ | 1 ms | 5 ms | `070` ✅ | 1 ms | 5 ms |
| `021` ✅ | 1 ms | 5 ms | `071` ✅ | 1 ms | 5 ms |
| `022` ✅ | 1 ms | 5 ms | `072` ✅ | 1 ms | 5 ms |
| `023` ✅ | 1 ms | 5 ms | `073` ✅ | 1 ms | 5 ms |
| `024` ✅ | 1 ms | 5 ms | `074` ✅ | 1 ms | 5 ms |
| `025` ✅ | 1 ms | 5 ms | `075` ✅ | 1 ms | 5 ms |
| `026` ✅ | 1 ms | 5 ms | `076` ✅ | 1 ms | 5 ms |
| `027` ✅ | 1 ms | 5 ms | `077` ✅ | 1 ms | 5 ms |
| `028` ✅ | 1 ms | 5 ms | `078` ✅ | 1 ms | 5 ms |
| `029` ✅ | 1 ms | 5 ms | `079` ✅ | 1 ms | 5 ms |
| `030` ✅ | 1 ms | 5 ms | `080` ✅ | 1 ms | 5 ms |
| `031` ✅ | 1 ms | 5 ms | `081` ✅ | 1 ms | 5 ms |
| `032` ✅ | 1 ms | 5 ms | `082` ✅ | 1 ms | 5 ms |
| `033` ✅ | 1 ms | 5 ms | `083` ✅ | 1 ms | 5 ms |
| `034` ✅ | 1 ms | 5 ms | `084` ✅ | 1 ms | 5 ms |
| `035` ✅ | 1 ms | 5 ms | `085` ✅ | 1 ms | 5 ms |
| `036` ✅ | 1 ms | 5 ms | `086` ✅ | 1 ms | 5 ms |
| `037` ✅ | 1 ms | 5 ms | `087` ✅ | 1 ms | 5 ms |
| `038` ✅ | 1 ms | 5 ms | `088` ✅ | 1 ms | 5 ms |
| `039` ✅ | 1 ms | 5 ms | `089` ✅ | 1 ms | 5 ms |
| `040` ✅ | 1 ms | 5 ms | `090` ✅ | 1 ms | 5 ms |
| `041` ✅ | 1 ms | 5 ms | `091` ✅ | 1 ms | 5 ms |
| `042` ✅ | 1 ms | 5 ms | `092` ✅ | 1 ms | 5 ms |
| `043` ✅ | 1 ms | 5 ms | `093` ✅ | 1 ms | 5 ms |
| `044` ✅ | 1 ms | 5 ms | `094` ✅ | 1 ms | 5 ms |
| `045` ✅ | 1 ms | 5 ms | `095` ✅ | 1 ms | 5 ms |
| `046` ✅ | 1 ms | 5 ms | `096` ✅ | 1 ms | 5 ms |
| `047` ✅ | 1 ms | 5 ms | `097` ✅ | 1 ms | 5 ms |
| `048` ✅ | 1 ms | 5 ms | `098` ✅ | 1 ms | 5 ms |
| `049` ✅ | 1 ms | 5 ms | `099` ✅ | 1 ms | 5 ms |
| `050` ✅ | 1 ms | 5 ms |  |  |  |

---
## <a id="quad"></a>`quad` — 60/60 ✅

Solve `ax² + bx + c = 0` over the integers, print roots in LaTeX.

```bash
python3 check.py ./quad --str --out md
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `031` ✅ | 1 ms | 5 ms |
| `002` ✅ | 3 ms | 5 ms | `032` ✅ | 1 ms | 4 ms |
| `003` ✅ | 2 ms | 5 ms | `033` ✅ | 1 ms | 5 ms |
| `004` ✅ | 2 ms | 5 ms | `034` ✅ | 1 ms | 5 ms |
| `005` ✅ | 2 ms | 5 ms | `035` ✅ | 1 ms | 5 ms |
| `006` ✅ | 2 ms | 5 ms | `036` ✅ | 1 ms | 5 ms |
| `007` ✅ | 2 ms | 5 ms | `037` ✅ | 1 ms | 5 ms |
| `008` ✅ | 2 ms | 4 ms | `038` ✅ | 1 ms | 4 ms |
| `009` ✅ | 1 ms | 4 ms | `039` ✅ | 1 ms | 4 ms |
| `010` ✅ | 1 ms | 4 ms | `040` ✅ | 1 ms | 5 ms |
| `011` ✅ | 1 ms | 5 ms | `041` ✅ | 1 ms | 4 ms |
| `012` ✅ | 1 ms | 5 ms | `042` ✅ | 1 ms | 5 ms |
| `013` ✅ | 1 ms | 4 ms | `043` ✅ | 1 ms | 4 ms |
| `014` ✅ | 1 ms | 4 ms | `044` ✅ | 1 ms | 4 ms |
| `015` ✅ | 1 ms | 4 ms | `045` ✅ | 1 ms | 5 ms |
| `016` ✅ | 1 ms | 5 ms | `046` ✅ | 1 ms | 5 ms |
| `017` ✅ | 1 ms | 4 ms | `047` ✅ | 1 ms | 4 ms |
| `018` ✅ | 1 ms | 4 ms | `048` ✅ | 1 ms | 5 ms |
| `019` ✅ | 1 ms | 5 ms | `049` ✅ | 1 ms | 4 ms |
| `020` ✅ | 1 ms | 4 ms | `050` ✅ | 1 ms | 4 ms |
| `021` ✅ | 1 ms | 5 ms | `051` ✅ | 1 ms | 5 ms |
| `022` ✅ | 1 ms | 5 ms | `052` ✅ | 1 ms | 4 ms |
| `023` ✅ | 1 ms | 5 ms | `053` ✅ | 1 ms | 4 ms |
| `024` ✅ | 1 ms | 5 ms | `054` ✅ | 1 ms | 5 ms |
| `025` ✅ | 1 ms | 5 ms | `055` ✅ | 1 ms | 4 ms |
| `026` ✅ | 1 ms | 5 ms | `056` ✅ | 1 ms | 4 ms |
| `027` ✅ | 1 ms | 5 ms | `057` ✅ | 1 ms | 5 ms |
| `028` ✅ | 1 ms | 5 ms | `058` ✅ | 1 ms | 4 ms |
| `029` ✅ | 1 ms | 5 ms | `059` ✅ | 1 ms | 4 ms |
| `030` ✅ | 1 ms | 4 ms | `060` ✅ | 2 ms | 7 ms |

---
## <a id="slalom"></a>`slalom` — 10/10 ✅

Minimum travel distance through ordered slalom gates.

```bash
python3 check.py ./slalom --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `006` ✅ | 2 ms | 5 ms |
| `002` ✅ | 3 ms | 5 ms | `007` ✅ | 3 ms | 5 ms |
| `003` ✅ | 2 ms | 5 ms | `008` ✅ | 4 ms | 5 ms |
| `004` ✅ | 2 ms | 5 ms | `009` ✅ | 16 ms | 6 ms |
| `005` ✅ | 2 ms | 5 ms | `010` ✅ | 1 ms | 5 ms |

---
## <a id="tournament"></a>`tournament` — 140/140 ✅

Which sith can win a tournament where each duel is won on 2 of 3 stats.

```bash
python3 check.py ./tournament --str
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 2 ms | 5 ms | `071` ✅ | 1 ms | 6 ms |
| `002` ✅ | 1 ms | 5 ms | `072` ✅ | 1 ms | 6 ms |
| `003` ✅ | 1 ms | 5 ms | `073` ✅ | 1 ms | 6 ms |
| `004` ✅ | 1 ms | 5 ms | `074` ✅ | 1 ms | 6 ms |
| `005` ✅ | 2 ms | 5 ms | `075` ✅ | 1 ms | 6 ms |
| `006` ✅ | 1 ms | 5 ms | `076` ✅ | 11 ms | 34 ms |
| `007` ✅ | 1 ms | 5 ms | `077` ✅ | 12 ms | 35 ms |
| `008` ✅ | 1 ms | 5 ms | `078` ✅ | 11 ms | 32 ms |
| `009` ✅ | 1 ms | 5 ms | `079` ✅ | 12 ms | 35 ms |
| `010` ✅ | 1 ms | 5 ms | `080` ✅ | 11 ms | 34 ms |
| `011` ✅ | 1 ms | 5 ms | `081` ✅ | 11 ms | 36 ms |
| `012` ✅ | 1 ms | 5 ms | `082` ✅ | 12 ms | 36 ms |
| `013` ✅ | 1 ms | 5 ms | `083` ✅ | 12 ms | 36 ms |
| `014` ✅ | 1 ms | 5 ms | `084` ✅ | 12 ms | 38 ms |
| `015` ✅ | 1 ms | 5 ms | `085` ✅ | 11 ms | 33 ms |
| `016` ✅ | 1 ms | 5 ms | `086` ✅ | 7 ms | 28 ms |
| `017` ✅ | 1 ms | 5 ms | `087` ✅ | 7 ms | 26 ms |
| `018` ✅ | 1 ms | 5 ms | `088` ✅ | 7 ms | 24 ms |
| `019` ✅ | 1 ms | 5 ms | `089` ✅ | 7 ms | 28 ms |
| `020` ✅ | 1 ms | 5 ms | `090` ✅ | 6 ms | 25 ms |
| `021` ✅ | 1 ms | 5 ms | `091` ✅ | 64 ms | 176 ms |
| `022` ✅ | 1 ms | 5 ms | `092` ✅ | 65 ms | 170 ms |
| `023` ✅ | 1 ms | 5 ms | `093` ✅ | 61 ms | 160 ms |
| `024` ✅ | 1 ms | 5 ms | `094` ✅ | 63 ms | 165 ms |
| `025` ✅ | 1 ms | 5 ms | `095` ✅ | 64 ms | 173 ms |
| `026` ✅ | 1 ms | 5 ms | `096` ✅ | 69 ms | 193 ms |
| `027` ✅ | 1 ms | 5 ms | `097` ✅ | 68 ms | 191 ms |
| `028` ✅ | 1 ms | 5 ms | `098` ✅ | 68 ms | 191 ms |
| `029` ✅ | 1 ms | 5 ms | `099` ✅ | 70 ms | 194 ms |
| `030` ✅ | 1 ms | 5 ms | `100` ✅ | 66 ms | 183 ms |
| `031` ✅ | 1 ms | 5 ms | `101` ✅ | 35 ms | 115 ms |
| `032` ✅ | 1 ms | 5 ms | `102` ✅ | 36 ms | 120 ms |
| `033` ✅ | 1 ms | 5 ms | `103` ✅ | 34 ms | 114 ms |
| `034` ✅ | 1 ms | 5 ms | `104` ✅ | 39 ms | 129 ms |
| `035` ✅ | 1 ms | 5 ms | `105` ✅ | 34 ms | 120 ms |
| `036` ✅ | 1 ms | 5 ms | `106` ✅ | 66 ms | 172 ms |
| `037` ✅ | 1 ms | 5 ms | `107` ✅ | 66 ms | 172 ms |
| `038` ✅ | 1 ms | 5 ms | `108` ✅ | 64 ms | 173 ms |
| `039` ✅ | 1 ms | 5 ms | `109` ✅ | 65 ms | 171 ms |
| `040` ✅ | 1 ms | 5 ms | `110` ✅ | 65 ms | 172 ms |
| `041` ✅ | 1 ms | 5 ms | `111` ✅ | 70 ms | 195 ms |
| `042` ✅ | 1 ms | 5 ms | `112` ✅ | 71 ms | 197 ms |
| `043` ✅ | 1 ms | 5 ms | `113` ✅ | 70 ms | 197 ms |
| `044` ✅ | 1 ms | 5 ms | `114` ✅ | 72 ms | 196 ms |
| `045` ✅ | 1 ms | 5 ms | `115` ✅ | 70 ms | 195 ms |
| `046` ✅ | 1 ms | 5 ms | `116` ✅ | 38 ms | 129 ms |
| `047` ✅ | 1 ms | 5 ms | `117` ✅ | 39 ms | 129 ms |
| `048` ✅ | 1 ms | 5 ms | `118` ✅ | 39 ms | 133 ms |
| `049` ✅ | 1 ms | 5 ms | `119` ✅ | 38 ms | 131 ms |
| `050` ✅ | 1 ms | 5 ms | `120` ✅ | 39 ms | 130 ms |
| `051` ✅ | 1 ms | 5 ms | `121` ✅ | 2 ms | 5 ms |
| `052` ✅ | 1 ms | 5 ms | `122` ✅ | 47 ms | 148 ms |
| `053` ✅ | 1 ms | 5 ms | `123` ✅ | 37 ms | 130 ms |
| `054` ✅ | 1 ms | 5 ms | `124` ✅ | 34 ms | 122 ms |
| `055` ✅ | 1 ms | 5 ms | `125` ✅ | 34 ms | 123 ms |
| `056` ✅ | 1 ms | 5 ms | `126` ✅ | 26 ms | 119 ms |
| `057` ✅ | 1 ms | 5 ms | `127` ✅ | 29 ms | 116 ms |
| `058` ✅ | 1 ms | 5 ms | `128` ✅ | 31 ms | 125 ms |
| `059` ✅ | 1 ms | 5 ms | `129` ✅ | 28 ms | 128 ms |
| `060` ✅ | 1 ms | 5 ms | `130` ✅ | 31 ms | 118 ms |
| `061` ✅ | 1 ms | 6 ms | `131` ✅ | 41 ms | 141 ms |
| `062` ✅ | 1 ms | 6 ms | `132` ✅ | 52 ms | 162 ms |
| `063` ✅ | 1 ms | 6 ms | `133` ✅ | 41 ms | 137 ms |
| `064` ✅ | 1 ms | 6 ms | `134` ✅ | 41 ms | 138 ms |
| `065` ✅ | 1 ms | 6 ms | `135` ✅ | 52 ms | 158 ms |
| `066` ✅ | 1 ms | 7 ms | `136` ✅ | 32 ms | 129 ms |
| `067` ✅ | 1 ms | 7 ms | `137` ✅ | 41 ms | 152 ms |
| `068` ✅ | 1 ms | 7 ms | `138` ✅ | 42 ms | 151 ms |
| `069` ✅ | 2 ms | 7 ms | `139` ✅ | 41 ms | 150 ms |
| `070` ✅ | 1 ms | 7 ms | `140` ✅ | 43 ms | 146 ms |

---
## <a id="triopt"></a>`triopt` — 65/65 ✅

Optimize `F1(x) + F2(y) + F3(z)` with `x + y + z = S`, each `Fi` being sin or cos.

```bash
python3 check.py ./triopt --tol=1e-12
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 10 ms | 17 ms | `034` ✅ | 19 ms | 29 ms |
| `002` ✅ | 9 ms | 16 ms | `035` ✅ | 16 ms | 27 ms |
| `003` ✅ | 8 ms | 16 ms | `036` ✅ | 18 ms | 30 ms |
| `004` ✅ | 8 ms | 16 ms | `037` ✅ | 13 ms | 16 ms |
| `005` ✅ | 7 ms | 16 ms | `038` ✅ | 21 ms | 29 ms |
| `006` ✅ | 7 ms | 17 ms | `039` ✅ | 20 ms | 28 ms |
| `007` ✅ | 7 ms | 17 ms | `040` ✅ | 23 ms | 29 ms |
| `008` ✅ | 7 ms | 16 ms | `041` ✅ | 21 ms | 29 ms |
| `009` ✅ | 7 ms | 16 ms | `042` ✅ | 10 ms | 19 ms |
| `010` ✅ | 7 ms | 16 ms | `043` ✅ | 19 ms | 29 ms |
| `011` ✅ | 6 ms | 17 ms | `044` ✅ | 18 ms | 30 ms |
| `012` ✅ | 6 ms | 16 ms | `045` ✅ | 21 ms | 29 ms |
| `013` ✅ | 7 ms | 16 ms | `046` ✅ | 17 ms | 29 ms |
| `014` ✅ | 7 ms | 15 ms | `047` ✅ | 12 ms | 24 ms |
| `015` ✅ | 6 ms | 16 ms | `048` ✅ | 8 ms | 17 ms |
| `016` ✅ | 7 ms | 16 ms | `049` ✅ | 18 ms | 28 ms |
| `017` ✅ | 7 ms | 16 ms | `050` ✅ | 20 ms | 29 ms |
| `018` ✅ | 7 ms | 16 ms | `051` ✅ | 24 ms | 30 ms |
| `019` ✅ | 7 ms | 16 ms | `052` ✅ | 22 ms | 28 ms |
| `020` ✅ | 7 ms | 16 ms | `053` ✅ | 9 ms | 19 ms |
| `021` ✅ | 7 ms | 16 ms | `054` ✅ | 12 ms | 16 ms |
| `022` ✅ | 7 ms | 16 ms | `055` ✅ | 18 ms | 29 ms |
| `023` ✅ | 7 ms | 16 ms | `056` ✅ | 18 ms | 28 ms |
| `024` ✅ | 7 ms | 16 ms | `057` ✅ | 20 ms | 30 ms |
| `025` ✅ | 7 ms | 16 ms | `058` ✅ | 20 ms | 29 ms |
| `026` ✅ | 7 ms | 16 ms | `059` ✅ | 13 ms | 22 ms |
| `027` ✅ | 17 ms | 29 ms | `060` ✅ | 14 ms | 15 ms |
| `028` ✅ | 18 ms | 30 ms | `061` ✅ | 18 ms | 28 ms |
| `029` ✅ | 18 ms | 30 ms | `062` ✅ | 18 ms | 29 ms |
| `030` ✅ | 18 ms | 27 ms | `063` ✅ | 18 ms | 30 ms |
| `031` ✅ | 10 ms | 19 ms | `064` ✅ | 16 ms | 28 ms |
| `032` ✅ | 24 ms | 28 ms | `065` ✅ | 9 ms | 19 ms |
| `033` ✅ | 21 ms | 29 ms |  |  |  |

---
## <a id="water"></a>`water` — 14/14 ✅

Water levels in `N` spherical tanks after pouring in volume `V`.

```bash
python3 check.py ./water --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` ✅ | 3 ms | 5 ms | `008` ✅ | 2 ms | 5 ms |
| `002` ✅ | 2 ms | 5 ms | `009` ✅ | 2 ms | 5 ms |
| `003` ✅ | 2 ms | 5 ms | `010` ✅ | 2 ms | 5 ms |
| `004` ✅ | 2 ms | 4 ms | `011` ✅ | 1 ms | 5 ms |
| `005` ✅ | 2 ms | 5 ms | `012` ✅ | 2 ms | 5 ms |
| `006` ✅ | 2 ms | 4 ms | `013` ✅ | 4 ms | 6 ms |
| `007` ✅ | 2 ms | 5 ms | `014` ✅ | 3 ms | 7 ms |
