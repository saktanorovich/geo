# geo — Test Report

**Date:** 2026-08-29  
**Machines:** every table below carries one column per machine.

| | Platform | Toolchain | Status |
|---|---|---|---|
| **Windows** | Windows 11 Pro (10.0.26200) | g++ 15.2.0 (MSYS2), `-std=c++17 -O2` · Python 3.13.13 | measured — all 8 problems, 13.5 s total |
| **macOS** | macOS (Darwin 25.5.0) | Apple clang 17.0.0, `-std=c++17 -O2` · Python 3.14.0 | measured — all 8 problems, 12.7 s total |

Both columns are a fresh sweep of the current binaries (2026-08-29): macOS on Apple
clang 17.0.0 with Python 3.14.0, Windows on g++ 15.2.0 with Python 3.13.13. Each is a
single run per test, so the first test of each problem is a cold spawn — see the note
below — and the columns should not be compared cell for cell. Both now run the same
committed `main.cpp` for every problem, `tournament` included, where the Windows column
previously lagged on the older recursive segment tree.

## Verdict — 506/521 ❌

| Problem | Tests | Compare | Result | macOS max | Windows max | Status |
|---|---:|---|---|---:|---:|:---:|
| [`gsm`](#gsm) | 100 | `--tol=1e-5` | 100/100 | 392 ms | 114 ms | ✅ |
| [`parabola`](#parabola) | 79 | `--tol=1e-6` | 64/79 | 339 ms | 211 ms | ❌ |
| [`pool`](#pool) | 99 | `--tol=1e-3` | 99/99 | 277 ms | 43 ms | ✅ |
| [`quad`](#quad) | 14 | `--str` | 14/14 | 285 ms | 33 ms | ✅ |
| [`slalom`](#slalom) | 10 | `--tol=1e-3` | 10/10 | 284 ms | 53 ms | ✅ |
| [`tournament`](#tournament) | 140 | `--str` | 140/140 | 390 ms | 201 ms | ✅ |
| [`triopt`](#triopt) | 65 | `--tol=1e-12` | 65/65 | 301 ms | 48 ms | ✅ |
| [`water`](#water) | 14 | `--tol=1e-3` | 14/14 | 288 ms | 59 ms | ✅ |
| **Total** | **521** | | **506/521** | **12.7 s** | **13.5 s** | **❌** |

The verdicts are the machine-independent part and are the same on both: 506/521, with
the same 15 `parabola` tests red. Only the timings differ between the machines.

Tests are named with a 3-digit schema (`001.in` / `001.out`) throughout. Times are
measured end-to-end including process spawn, so the ~5-6 ms floor on small Windows
tests is spawn overhead rather than solve time — and the first test of each problem is
higher still, being the first spawn of a freshly compiled binary. That cold-spawn cost
is far larger on this Mac (~280-390 ms) than on Windows (~33-60 ms), so for every
problem except `parabola` the **macOS max** column *is* test `001`'s warm-up rather than
the peak solve time — `parabola` is the one exception, where the genuine slow test `060`
(339 ms) tops its own warm-up. The Windows warm-up is small enough that only five of the
eight problems (`pool`, `quad`, `slalom`, `triopt`, `water`, all of whose remaining tests
finish in under 35 ms) peak on test `001`; for `gsm`, `parabola` and `tournament` the
**Windows max** is genuine solve time. Both columns are a single run per test; don't
compare them cell for cell. Where real work dominates the picture is clearer: on the
largest `tournament` tests macOS runs about 2.7x faster than Windows (64-71 ms vs
170-201 ms at `n = 100,000`), both machines now on the same iterative segment tree.

Two problems compare output as **strings** rather than numbers (`--str`), because their
answers are not numeric: `quad` emits LaTeX, and `tournament` emits sith names — see
[Why `--str`](#why---str).

**`parabola` (64/79) is failing, and that is deliberate.** Those tests document a known defect in
the solution that its original tests never exercised; they are kept red so the defect
stays visible instead of being forgotten. A failing test is marked ❌ next to its
number in the grids below, and listed with its error in the section itself. `gsm`
carries two defects of the same kind, both fixed — see its section for the contrast.

---

## Harness

`check.py` compiles `<folder>/main.cpp` with `g++ -std=c++17 -O2`, feeds each
`tests/*.in` on stdin, and compares stdout against the matching `.out`. Solutions read
stdin and write stdout — `freopen` produces no output under the harness.

| Flag | Default | Meaning |
|---|---|---|
| `--tol` | `1e-4` | numeric comparison tolerance |
| `--str` | off | compare tokens as exact strings instead |
| `--src` | `main.cpp` | source to compile, relative to the problem folder |
| `--out` | `program` | name of the compiled binary (`.exe` on Windows) |

Two comparators exist as separate functions: `compare_numbers()` applies the tolerance
and falls back to string equality for non-numeric tokens, while `compare_strings()`
never calls `float()` at all. The tolerances in [Reproduce](#reproduce) are the
precision each statement asks for, not arbitrary choices.

A problem whose tests are packed — currently only `tournament` — needs no extra step:
when there is no `tests/` directory but a `tests.zip` is present, the archive is
unpacked into a temporary directory, the suite runs from there, and the directory is
removed afterwards. A plain `tests/` directory always takes precedence and is never
touched. The compiled binary is removed when the run ends, including after a failure.

See [README.md](README.md) for usage.

---

## <a id="gsm"></a>`gsm` — 100/100 ✅

Shortest path between two points that stays inside GSM coverage circles.

**Algorithm:** Graph over zone-boundary intersection points, then SPFA.

```bash
python3 check.py ./gsm --tol=1e-5
```

The suite grew from 15 tests to 100 while fixing two defects in `main.cpp`. Neither
was visible to the original 15 — both passed all of them.

**Ordering along the segment.** `contains()` splits a segment at every zone-boundary
crossing and checks that each piece lies inside a single zone, which requires the
break points ordered *along* the segment. Sorting by `x` alone is only a valid proxy
when `dx != 0`; on a vertical segment every `x` ties, `std::sort` returns an arbitrary
permutation, and the pieces tested are not the segment's pieces. Vertical segments are
not exotic here — two zones whose centres share a `y` produce intersection points with
identical `x`. Fixed by sorting on the projection onto the segment direction, computed
once per point rather than inside the comparator.

**Partial intersections were dropped.** `intersect(zone, segment)` reported a crossing
only when *both* quadratic roots fell in `[0,1]`, so a zone clipping the segment once
contributed no break point. Two zones each covering one end of a segment produced zero
break points, and a fully covered segment was rejected: `0 0 10 0` with zones
`(0,0,6)` and `(10,0,6)` returned 12.0 where the answer is 10.0. Fixed by reporting
each in-range root; between consecutive crossings no boundary is crossed, so the set of
covering zones is constant and the check becomes exact rather than conservative.

Expected values for the new tests never came from the solution itself. 34 are straight
lines provably covered by the zone union, so the answer is exactly `|AB|`; 20 are chains
of externally tangent zones where every tangency point is a cut vertex, forcing the sum
of chords; 21 come from an independent oracle that computes each zone's parameter
interval along the segment and asks whether the interval union covers `[0,1]`, then runs
Dijkstra — a different principle from the reference, so agreement is evidence. The
remainder are hand-built corner cases: `K=1`, endpoints exactly on boundaries, a forced
bend through a tangency point, coordinates at 1e6, a zone tangent to the path, and
`K=200` at the constraint limit.

Reverting each fix shows every group of tests is load-bearing:

| Variant | Passed | Failing |
|---|---|---|
| both fixes (current) | 100/100 | — |
| partial intersections dropped | 87/100 | 13 tests |
| ordering by `x` | 85/100 | 15 tests |
| neither fix | 76/100 | 24 tests |

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 392 ms | 60 ms | `051` | 2 ms | 7 ms |
| `002` | 5 ms | 6 ms | `052` | 1 ms | 6 ms |
| `003` | 5 ms | 6 ms | `053` | 1 ms | 8 ms |
| `004` | 4 ms | 6 ms | `054` | 2 ms | 7 ms |
| `005` | 3 ms | 6 ms | `055` | 1 ms | 6 ms |
| `006` | 2 ms | 6 ms | `056` | 1 ms | 6 ms |
| `007` | 2 ms | 6 ms | `057` | 1 ms | 6 ms |
| `008` | 2 ms | 6 ms | `058` | 1 ms | 6 ms |
| `009` | 3 ms | 6 ms | `059` | 1 ms | 6 ms |
| `010` | 2 ms | 5 ms | `060` | 1 ms | 8 ms |
| `011` | 2 ms | 7 ms | `061` | 1 ms | 6 ms |
| `012` | 2 ms | 7 ms | `062` | 1 ms | 6 ms |
| `013` | 2 ms | 8 ms | `063` | 1 ms | 6 ms |
| `014` | 88 ms | 114 ms | `064` | 1 ms | 6 ms |
| `015` | 80 ms | 114 ms | `065` | 1 ms | 6 ms |
| `016` | 2 ms | 8 ms | `066` | 1 ms | 5 ms |
| `017` | 2 ms | 6 ms | `067` | 1 ms | 6 ms |
| `018` | 1 ms | 7 ms | `068` | 1 ms | 8 ms |
| `019` | 1 ms | 6 ms | `069` | 1 ms | 6 ms |
| `020` | 1 ms | 6 ms | `070` | 1 ms | 6 ms |
| `021` | 1 ms | 6 ms | `071` | 1 ms | 7 ms |
| `022` | 1 ms | 6 ms | `072` | 1 ms | 7 ms |
| `023` | 1 ms | 6 ms | `073` | 1 ms | 6 ms |
| `024` | 1 ms | 6 ms | `074` | 1 ms | 6 ms |
| `025` | 1 ms | 6 ms | `075` | 1 ms | 6 ms |
| `026` | 2 ms | 7 ms | `076` | 1 ms | 6 ms |
| `027` | 6 ms | 15 ms | `077` | 1 ms | 6 ms |
| `028` | 58 ms | 93 ms | `078` | 1 ms | 6 ms |
| `029` | 2 ms | 8 ms | `079` | 1 ms | 6 ms |
| `030` | 1 ms | 6 ms | `080` | 1 ms | 6 ms |
| `031` | 1 ms | 6 ms | `081` | 1 ms | 6 ms |
| `032` | 1 ms | 7 ms | `082` | 1 ms | 6 ms |
| `033` | 1 ms | 5 ms | `083` | 1 ms | 6 ms |
| `034` | 1 ms | 6 ms | `084` | 1 ms | 6 ms |
| `035` | 1 ms | 7 ms | `085` | 1 ms | 5 ms |
| `036` | 1 ms | 6 ms | `086` | 1 ms | 5 ms |
| `037` | 1 ms | 6 ms | `087` | 1 ms | 6 ms |
| `038` | 1 ms | 7 ms | `088` | 1 ms | 5 ms |
| `039` | 1 ms | 6 ms | `089` | 1 ms | 6 ms |
| `040` | 2 ms | 7 ms | `090` | 1 ms | 6 ms |
| `041` | 1 ms | 6 ms | `091` | 1 ms | 6 ms |
| `042` | 1 ms | 6 ms | `092` | 1 ms | 6 ms |
| `043` | 1 ms | 6 ms | `093` | 1 ms | 6 ms |
| `044` | 1 ms | 6 ms | `094` | 1 ms | 6 ms |
| `045` | 1 ms | 6 ms | `095` | 1 ms | 6 ms |
| `046` | 1 ms | 6 ms | `096` | 1 ms | 6 ms |
| `047` | 2 ms | 7 ms | `097` | 1 ms | 5 ms |
| `048` | 1 ms | 6 ms | `098` | 1 ms | 6 ms |
| `049` | 1 ms | 6 ms | `099` | 1 ms | 5 ms |
| `050` | 1 ms | 6 ms | `100` | 1 ms | 5 ms |


---

## <a id="parabola"></a>`parabola` — 64/79 ❌

Shortest path through parabolic teleporters.

**Algorithm:** Golden-section search for point-to-arc distance, then SPFA on the complete graph.

```bash
python3 check.py ./parabola --tol=1e-6
```

> **Open defect.** Tests `065`–`079` fail against the current `main.cpp`. They are not
> broken tests — each expected value is independently verified, and the 15 of them
> exist to document a real flaw in the search. The suite is red on purpose.

The suite grew from 64 tests to 79 to pin down a defect in `main.cpp`. All 64 original
tests pass, then and now — none of them exercised it.

**Golden-section search on a profile that is not unimodal.** Every edge weight is a
minimum distance found by `gold()`, which is only valid on a unimodal interval. The
squared distance from a point to an arc is a quartic, so its stationary equation is a
cubic; written in vertex coordinates that cubic has no quadratic term, so its three
roots sum to zero and the two local minima provably straddle the vertex. The `-b/2a`
split therefore handles arcs that *contain* their vertex — and does nothing for arcs
that do not. Such an arc can hold a local minimum *and* a local maximum, and
golden-section's first probes at 38.2% / 61.8% can bracket the wrong basin.

Concretely, arc `A=1 B=82 C=-93` on `[-57.037, -41.074]` with the point
`(97.1618, -1740.8348)`: the search returns 143.191 where the answer is 142.157 — an
error of **1.03** against a required precision of `1e-4`.

**Known remedy, not currently applied.** Point-to-arc can be solved exactly: the
depressed cubic has a closed form, so evaluating its real roots inside the arc plus both
endpoints removes the search entirely. Arc-to-arc has no practical closed form (it is a
resultant of two cubics), so its profile needs sampling with every sampled local minimum
refined, rather than one search over the whole span. That combination measured 79/79 with
0 failures across all 7002 adversarial candidates, and ran *faster* than the current code,
an exact solve beating 50 golden iterations — on the machine of the original report its
slowest test fell from 224 ms to 74 ms. It is not in the tree.

These cases have to be constructed; they are not found by chance. Of 7002 generated
candidates, uniform-random geometry produced **1** failure while the constructed families
produced **195**. The construction picks the stationary cubic's roots `r1 < r2 < 0 < r3`
(their sum is forced to zero), back-solves the point from the elementary symmetric
functions, and places the arc entirely on one side of the vertex so no split happens.

Expected values come from an independent solver — exact cubic for point-to-arc, a
multi-level scan for arc-to-arc — which reproduces all 64 original expected outputs
to `2.3e-10` and agrees with a two-million-sample brute-force scan.

| Variant | Passed | Failing |
|---|---|---|
| golden-section search (**current**) | 64/79 | 065–079 |
| exact cubic + multi-start (measured, not applied) | 79/79 | — |

The 15 failing tests fail *only* under the current search — nothing else in the suite
is affected, so each one is load-bearing evidence for exactly this defect.

**Failing tests** — every one overshoots, i.e. the solution reports a
longer path than exists:

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
| `001` | 283 ms | 45 ms | `041` | 1 ms | 6 ms |
| `002` | 6 ms | 6 ms | `042` | 1 ms | 6 ms |
| `003` | 5 ms | 6 ms | `043` | 1 ms | 6 ms |
| `004` | 3 ms | 6 ms | `044` | 1 ms | 7 ms |
| `005` | 2 ms | 6 ms | `045` | 1 ms | 6 ms |
| `006` | 2 ms | 7 ms | `046` | 1 ms | 6 ms |
| `007` | 2 ms | 6 ms | `047` | 1 ms | 6 ms |
| `008` | 2 ms | 6 ms | `048` | 1 ms | 7 ms |
| `009` | 2 ms | 6 ms | `049` | 1 ms | 6 ms |
| `010` | 2 ms | 5 ms | `050` | 3 ms | 7 ms |
| `011` | 2 ms | 6 ms | `051` | 11 ms | 11 ms |
| `012` | 2 ms | 6 ms | `052` | 15 ms | 13 ms |
| `013` | 2 ms | 8 ms | `053` | 29 ms | 20 ms |
| `014` | 2 ms | 6 ms | `054` | 45 ms | 26 ms |
| `015` | 2 ms | 6 ms | `055` | 83 ms | 48 ms |
| `016` | 2 ms | 6 ms | `056` | 112 ms | 60 ms |
| `017` | 2 ms | 6 ms | `057` | 132 ms | 67 ms |
| `018` | 2 ms | 6 ms | `058` | 152 ms | 73 ms |
| `019` | 2 ms | 6 ms | `059` | 193 ms | 92 ms |
| `020` | 1 ms | 8 ms | `060` | 339 ms | 211 ms |
| `021` | 2 ms | 6 ms | `061` | 338 ms | 190 ms |
| `022` | 1 ms | 6 ms | `062` | 86 ms | 28 ms |
| `023` | 2 ms | 6 ms | `063` | 92 ms | 45 ms |
| `024` | 1 ms | 6 ms | `064` | 86 ms | 29 ms |
| `025` | 1 ms | 6 ms | `065` ❌ | 2 ms | 6 ms |
| `026` | 1 ms | 6 ms | `066` ❌ | 1 ms | 6 ms |
| `027` | 1 ms | 6 ms | `067` ❌ | 1 ms | 6 ms |
| `028` | 1 ms | 6 ms | `068` ❌ | 2 ms | 5 ms |
| `029` | 1 ms | 6 ms | `069` ❌ | 1 ms | 6 ms |
| `030` | 1 ms | 7 ms | `070` ❌ | 1 ms | 6 ms |
| `031` | 1 ms | 6 ms | `071` ❌ | 2 ms | 6 ms |
| `032` | 1 ms | 6 ms | `072` ❌ | 1 ms | 6 ms |
| `033` | 1 ms | 7 ms | `073` ❌ | 1 ms | 5 ms |
| `034` | 1 ms | 6 ms | `074` ❌ | 1 ms | 5 ms |
| `035` | 1 ms | 6 ms | `075` ❌ | 1 ms | 5 ms |
| `036` | 1 ms | 6 ms | `076` ❌ | 1 ms | 5 ms |
| `037` | 1 ms | 6 ms | `077` ❌ | 1 ms | 6 ms |
| `038` | 1 ms | 6 ms | `078` ❌ | 1 ms | 6 ms |
| `039` | 1 ms | 6 ms | `079` ❌ | 1 ms | 6 ms |
| `040` | 1 ms | 6 ms |  |  |  |


---

## <a id="pool"></a>`pool` — 99/99 ✅

Radius of water spread from a source point in an `n x m` container.

**Algorithm:** Binary search on the radius; area accumulated from 8 triangular sectors.

```bash
python3 check.py ./pool --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 277 ms | 43 ms | `051` | 1 ms | 6 ms |
| `002` | 3 ms | 7 ms | `052` | 1 ms | 6 ms |
| `003` | 2 ms | 6 ms | `053` | 1 ms | 6 ms |
| `004` | 2 ms | 7 ms | `054` | 1 ms | 6 ms |
| `005` | 2 ms | 6 ms | `055` | 1 ms | 6 ms |
| `006` | 2 ms | 6 ms | `056` | 1 ms | 6 ms |
| `007` | 2 ms | 6 ms | `057` | 1 ms | 6 ms |
| `008` | 2 ms | 6 ms | `058` | 1 ms | 6 ms |
| `009` | 2 ms | 6 ms | `059` | 1 ms | 6 ms |
| `010` | 2 ms | 6 ms | `060` | 1 ms | 5 ms |
| `011` | 2 ms | 6 ms | `061` | 1 ms | 6 ms |
| `012` | 2 ms | 6 ms | `062` | 1 ms | 6 ms |
| `013` | 2 ms | 6 ms | `063` | 1 ms | 6 ms |
| `014` | 1 ms | 6 ms | `064` | 1 ms | 6 ms |
| `015` | 1 ms | 6 ms | `065` | 1 ms | 6 ms |
| `016` | 1 ms | 6 ms | `066` | 1 ms | 6 ms |
| `017` | 1 ms | 6 ms | `067` | 1 ms | 6 ms |
| `018` | 1 ms | 6 ms | `068` | 1 ms | 6 ms |
| `019` | 1 ms | 6 ms | `069` | 1 ms | 6 ms |
| `020` | 1 ms | 6 ms | `070` | 1 ms | 8 ms |
| `021` | 1 ms | 6 ms | `071` | 1 ms | 7 ms |
| `022` | 1 ms | 5 ms | `072` | 1 ms | 6 ms |
| `023` | 1 ms | 6 ms | `073` | 1 ms | 6 ms |
| `024` | 1 ms | 6 ms | `074` | 1 ms | 6 ms |
| `025` | 1 ms | 5 ms | `075` | 1 ms | 6 ms |
| `026` | 1 ms | 6 ms | `076` | 1 ms | 6 ms |
| `027` | 1 ms | 6 ms | `077` | 1 ms | 6 ms |
| `028` | 1 ms | 6 ms | `078` | 1 ms | 6 ms |
| `029` | 1 ms | 6 ms | `079` | 1 ms | 6 ms |
| `030` | 1 ms | 6 ms | `080` | 1 ms | 6 ms |
| `031` | 1 ms | 6 ms | `081` | 1 ms | 6 ms |
| `032` | 1 ms | 6 ms | `082` | 1 ms | 6 ms |
| `033` | 2 ms | 7 ms | `083` | 1 ms | 6 ms |
| `034` | 1 ms | 6 ms | `084` | 1 ms | 6 ms |
| `035` | 1 ms | 6 ms | `085` | 1 ms | 6 ms |
| `036` | 1 ms | 6 ms | `086` | 1 ms | 6 ms |
| `037` | 2 ms | 6 ms | `087` | 1 ms | 6 ms |
| `038` | 1 ms | 6 ms | `088` | 1 ms | 6 ms |
| `039` | 1 ms | 6 ms | `089` | 1 ms | 6 ms |
| `040` | 1 ms | 6 ms | `090` | 1 ms | 6 ms |
| `041` | 1 ms | 5 ms | `091` | 1 ms | 5 ms |
| `042` | 1 ms | 6 ms | `092` | 1 ms | 9 ms |
| `043` | 1 ms | 6 ms | `093` | 1 ms | 6 ms |
| `044` | 1 ms | 6 ms | `094` | 1 ms | 6 ms |
| `045` | 1 ms | 6 ms | `095` | 1 ms | 6 ms |
| `046` | 1 ms | 6 ms | `096` | 1 ms | 6 ms |
| `047` | 1 ms | 6 ms | `097` | 1 ms | 6 ms |
| `048` | 1 ms | 5 ms | `098` | 1 ms | 7 ms |
| `049` | 1 ms | 5 ms | `099` | 1 ms | 6 ms |
| `050` | 1 ms | 6 ms |  |  |  |


---

## <a id="quad"></a>`quad` — 14/14 ✅

Solve `ax² + bx + c = 0` over the integers, print roots in LaTeX.

**Algorithm:** Exact rational/surd arithmetic: GCD reduction plus square-factor extraction.

```bash
python3 check.py ./quad --str
```

Output is a LaTeX string, so tokens are matched exactly — a numeric tolerance
would be meaningless here. `D = b² - 4ac` is computed in `long long`. Results are
fully simplified: fractions reduced by GCD, square factors pulled out of surds
(`√D = g√r` with `r` square-free), and trivial parts (`±1`, `/1`, `+0`) omitted.
All literals are ASCII, so there are no output-encoding issues.

| Condition | Result type | Output form |
|---|---|---|
| `a=0, b=0, c=0` | Infinitely many | `x \in \mathbb{R}` |
| `a=0, b=0, c≠0` | No solutions | `x \in \varnothing` |
| `a=0, b≠0` | Linear | `x = -c/b` (reduced fraction) |
| `a≠0, D>0`, D a square | Two rational roots | `x_1 = …, \quad x_2 = …` |
| `a≠0, D>0`, D non-square | Two irrational roots | `x_{1,2} = \frac{p \pm q\sqrt{r}}{s}` |
| `a≠0, D=0` | Double root | `x = -b/2a` (reduced fraction) |
| `a≠0, D<0` | Complex conjugate pair | `x_{1,2} = \frac{p \pm qi\sqrt{r}}{s}` |

All 14 tests are hand-written, one per branch above:

| # | Input | Expected output | Case covered |
|---|---|---|---|
| 001 | `0 0 0` | `x \in \mathbb{R}` | infinite |
| 002 | `0 0 5` | `x \in \varnothing` | none |
| 003 | `0 2 -6` | `x = 3` | linear, integer |
| 004 | `0 3 1` | `x = -\frac{1}{3}` | linear, fraction |
| 005 | `1 -5 6` | `x_1 = 3, \quad x_2 = 2` | two rational (task example) |
| 006 | `1 -3 0` | `x_1 = 3, \quad x_2 = 0` | rational root at 0 |
| 007 | `2 -4 -3` | `x_{1,2} = \frac{2 \pm \sqrt{10}}{2}` | irrational + GCD reduction |
| 008 | `1 0 -2` | `x_{1,2} = \pm \sqrt{2}` | irrational, p=0, s=1 |
| 009 | `1 -4 4` | `x = 2` | double root (task example) |
| 010 | `1 -2 1` | `x = 1` | double root |
| 011 | `1 1 1` | `x_{1,2} = \frac{-1 \pm i\sqrt{3}}{2}` | complex (task example) |
| 012 | `1 0 1` | `x_{1,2} = \pm i` | complex, fully reduced (task example) |
| 013 | `1 2 5` | `x_{1,2} = -1 \pm 2i` | complex, perfect-square \|D\| |
| 014 | `-1 5 -6` | `x_1 = 2, \quad x_2 = 3` | negative `a` |

All four worked examples from `quad/task-ru.md` reproduce exactly.

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 285 ms | 33 ms | `008` | 2 ms | 6 ms |
| `002` | 6 ms | 6 ms | `009` | 2 ms | 6 ms |
| `003` | 5 ms | 7 ms | `010` | 2 ms | 6 ms |
| `004` | 3 ms | 6 ms | `011` | 2 ms | 7 ms |
| `005` | 3 ms | 6 ms | `012` | 2 ms | 6 ms |
| `006` | 2 ms | 6 ms | `013` | 2 ms | 6 ms |
| `007` | 2 ms | 6 ms | `014` | 2 ms | 6 ms |


---

## <a id="slalom"></a>`slalom` — 10/10 ✅

Minimum travel distance through ordered slalom gates.

**Algorithm:** Recursive funnel algorithm with integer cross-product comparisons.

```bash
python3 check.py ./slalom --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 284 ms | 53 ms | `006` | 2 ms | 7 ms |
| `002` | 5 ms | 6 ms | `007` | 4 ms | 6 ms |
| `003` | 4 ms | 7 ms | `008` | 6 ms | 8 ms |
| `004` | 3 ms | 6 ms | `009` | 19 ms | 7 ms |
| `005` | 3 ms | 6 ms | `010` | 2 ms | 6 ms |


---

## <a id="tournament"></a>`tournament` — 140/140 ✅

Which sith can win a tournament where each duel is won on 2 of 3 stats.

**Algorithm:** Rank-compress each stat, then BFS the winner set using three iterative (non-recursive) max segment trees sized to `N`.

```bash
python3 check.py ./tournament --str
```

Names are output in input order. The test data is large (154 MB expanded), so it ships
as `tournament/tests.zip`; `check.py` unpacks it for the run and cleans up afterwards.

### Runs

| Machine | Toolchain | Result | Max time | Total | Status |
|---|---|---|---:|---:|:---:|
| macOS | Apple clang 17.0.0 · Python 3.14.0 | 140/140 | 390 ms | 3.1 s | ✅ |
| Windows 11 Pro (10.0.26200) | g++ 15.2.0 (MSYS2) · Python 3.13.13 | 140/140 | 201 ms | 8.6 s | ✅ |

### Coverage against the stated constraints

| Constraint | Bound | Exercised |
|---|---|---|
| `N` | `1 … 10^5` | ✅ both ends — `n = 1` (`006`), `n = 100,000` (15 tests) |
| attributes | `−10^9 … 10^9` | ✅ `−1,000,000,000` (`121`) up to `999,992,795` (`115`); 31 tests use negatives |
| attributes distinct per column | guaranteed | ✅ holds in all 140 tests — no ties anywhere, as the statement promises |
| names pairwise distinct | guaranteed | ✅ holds in all 140 tests |
| name length | `≤ 10` | tests top out at 8 (`Grievous`); the `char name[12]` buffer holds the full 10 either way |
| answer size | `1 … n` | ✅ 38 tests answer with exactly one winner, 58 with all `n` |

The suite ramps scale (n = 20 → 1k → 17k → 100k) and at each scale alternates
between *nearly every sith can win* and *only a handful can* — the two shapes that
stress different parts of the BFS. Timing shows it clearly: at n = 100,000 the
all-winners tests run about 1.4x as long as the few-winners tests on Windows
(170-201 ms vs 129-131 ms) and about 1.8x on macOS (64-71 ms vs 37-39 ms).

| Tests | n | Winners | macOS | Windows | Verdict | Shape |
|---|---|---|---:|---:|:---:|---|
| `001-060` | 1-20 | 1-18 | 1-390 ms | 5-52 ms | 60/60 ✅ | hand-made / brute-force scale |
| `061-075` | 503-977 | 1-942 | 2 ms | 6-9 ms | 15/15 ✅ | small random |
| `076-085` | 15,345-19,281 | all *n*, one off by 1 | 11-13 ms | 34-40 ms | 10/10 ✅ | medium, most sith win |
| `086-090` | 15,281-18,148 | 1-2,921 | 7-8 ms | 25-28 ms | 5/5 ✅ | medium, few sith win |
| `091-100` | 93,610-99,985 | all *n* | 61-71 ms | 165-201 ms | 10/10 ✅ | large, all sith win |
| `101-105` | 90,336-99,986 | 169-704 | 34-38 ms | 119-165 ms | 5/5 ✅ | large, few sith win |
| `106-115` | 100,000 | all *n* | 64-71 ms | 170-201 ms | 10/10 ✅ | max n, all sith win |
| `116-120` | 100,000 | 1-1,270 | 37-39 ms | 129-131 ms | 5/5 ✅ | max n, few sith win |
| `121` | 3 | 1 | 2 ms | 5 ms | 1/1 ✅ | 2 ms |
| `122-130` | 90,235-97,071 | 23,839-92,292 | 26-46 ms | 117-148 ms | 9/9 ✅ | large, partial winner set |
| `131-140` | 100,000 | 50,000-100,000 | 33-52 ms | 136-165 ms | 10/10 ✅ | max n, half / all winners |

<details>
<summary>All 140 tests individually</summary>

<a id="tournament-per-test"></a>

| Test | n | Input | Winners | % of n | Powers | macOS | Windows | |
|---|---:|---:|---:|---:|:--|---:|---:|:--:|
| `001` | 5 | 84 B | 3 | 60.0% | 0..125 | 390 ms | 52 ms | ✅ |
| `002` | 3 | 30 B | 3 | 100.0% | 1..3 | 6 ms | 6 ms | ✅ |
| `003` | 3 | 30 B | 1 | 33.3% | 1..4 | 6 ms | 6 ms | ✅ |
| `004` | 3 | 30 B | 1 | 33.3% | 1..5 | 4 ms | 6 ms | ✅ |
| `005` | 3 | 30 B | 1 | 33.3% | 1..6 | 4 ms | 5 ms | ✅ |
| `006` | 1 | 12 B | 1 | 100.0% | 0..0 | 3 ms | 6 ms | ✅ |
| `007` | 2 | 21 B | 1 | 50.0% | 1..2 | 2 ms | 6 ms | ✅ |
| `008` | 2 | 26 B | 1 | 50.0% | 1..100,500 | 3 ms | 5 ms | ✅ |
| `009` | 4 | 39 B | 4 | 100.0% | 3..9 | 3 ms | 5 ms | ✅ |
| `010` | 4 | 39 B | 1 | 25.0% | 2..9 | 3 ms | 6 ms | ✅ |
| `011` | 4 | 39 B | 4 | 100.0% | 2..9 | 2 ms | 6 ms | ✅ |
| `012` | 4 | 39 B | 4 | 100.0% | 1..7 | 3 ms | 6 ms | ✅ |
| `013` | 4 | 39 B | 4 | 100.0% | 3..8 | 3 ms | 6 ms | ✅ |
| `014` | 4 | 39 B | 3 | 75.0% | 3..8 | 2 ms | 6 ms | ✅ |
| `015` | 4 | 39 B | 4 | 100.0% | 3..8 | 2 ms | 6 ms | ✅ |
| `016` | 4 | 39 B | 1 | 25.0% | 1..7 | 2 ms | 6 ms | ✅ |
| `017` | 4 | 39 B | 3 | 75.0% | 1..7 | 3 ms | 6 ms | ✅ |
| `018` | 4 | 39 B | 4 | 100.0% | 1..8 | 2 ms | 6 ms | ✅ |
| `019` | 5 | 48 B | 5 | 100.0% | 1..5 | 2 ms | 6 ms | ✅ |
| `020` | 5 | 48 B | 3 | 60.0% | 1..5 | 2 ms | 7 ms | ✅ |
| `021` | 5 | 48 B | 4 | 80.0% | 1..6 | 2 ms | 6 ms | ✅ |
| `022` | 5 | 48 B | 5 | 100.0% | 1..8 | 2 ms | 6 ms | ✅ |
| `023` | 5 | 48 B | 1 | 20.0% | 1..8 | 2 ms | 6 ms | ✅ |
| `024` | 5 | 48 B | 1 | 20.0% | 1..8 | 2 ms | 6 ms | ✅ |
| `025` | 5 | 48 B | 4 | 80.0% | 1..7 | 2 ms | 6 ms | ✅ |
| `026` | 16 | 177 B | 1 | 6.2% | 1..16 | 2 ms | 6 ms | ✅ |
| `027` | 10 | 109 B | 1 | 10.0% | 1..16 | 2 ms | 6 ms | ✅ |
| `028` | 12 | 131 B | 12 | 100.0% | 1..16 | 2 ms | 6 ms | ✅ |
| `029` | 13 | 144 B | 1 | 7.7% | 1..16 | 2 ms | 6 ms | ✅ |
| `030` | 4 | 41 B | 1 | 25.0% | 2..14 | 2 ms | 5 ms | ✅ |
| `031` | 15 | 167 B | 1 | 6.7% | 1..16 | 2 ms | 6 ms | ✅ |
| `032` | 14 | 156 B | 12 | 85.7% | 1..16 | 2 ms | 6 ms | ✅ |
| `033` | 18 | 203 B | 18 | 100.0% | 1..18 | 2 ms | 6 ms | ✅ |
| `034` | 11 | 120 B | 9 | 81.8% | 1..18 | 2 ms | 6 ms | ✅ |
| `035` | 7 | 75 B | 4 | 57.1% | 1..18 | 2 ms | 6 ms | ✅ |
| `036` | 13 | 142 B | 11 | 84.6% | 1..18 | 2 ms | 6 ms | ✅ |
| `037` | 6 | 62 B | 1 | 16.7% | 1..18 | 2 ms | 6 ms | ✅ |
| `038` | 15 | 167 B | 11 | 73.3% | 1..18 | 2 ms | 7 ms | ✅ |
| `039` | 13 | 144 B | 5 | 38.5% | 1..18 | 2 ms | 6 ms | ✅ |
| `040` | 16 | 179 B | 1 | 6.2% | 1..18 | 2 ms | 6 ms | ✅ |
| `041` | 15 | 533 B | 15 | 100.0% | ±1e9 | 2 ms | 6 ms | ✅ |
| `042` | 6 | 213 B | 1 | 16.7% | ±1e9 | 2 ms | 6 ms | ✅ |
| `043` | 13 | 460 B | 1 | 7.7% | ±1e9 | 1 ms | 5 ms | ✅ |
| `044` | 8 | 279 B | 1 | 12.5% | ±1e9 | 2 ms | 6 ms | ✅ |
| `045` | 18 | 635 B | 1 | 5.6% | ±1e9 | 2 ms | 6 ms | ✅ |
| `046` | 17 | 599 B | 1 | 5.9% | ±1e9 | 2 ms | 7 ms | ✅ |
| `047` | 13 | 460 B | 8 | 61.5% | ±1e9 | 2 ms | 6 ms | ✅ |
| `048` | 17 | 599 B | 16 | 94.1% | ±1e9 | 2 ms | 7 ms | ✅ |
| `049` | 9 | 316 B | 9 | 100.0% | ±1e9 | 2 ms | 6 ms | ✅ |
| `050` | 11 | 387 B | 1 | 9.1% | ±1e9 | 2 ms | 8 ms | ✅ |
| `051` | 17 | 191 B | 1 | 5.9% | 1..17 | 2 ms | 6 ms | ✅ |
| `052` | 20 | 230 B | 3 | 15.0% | 1..20 | 2 ms | 6 ms | ✅ |
| `053` | 20 | 230 B | 1 | 5.0% | 1..20 | 2 ms | 6 ms | ✅ |
| `054` | 14 | 152 B | 1 | 7.1% | 1..14 | 2 ms | 6 ms | ✅ |
| `055` | 18 | 204 B | 1 | 5.6% | 1..18 | 2 ms | 6 ms | ✅ |
| `056` | 16 | 178 B | 1 | 6.2% | 1..16 | 2 ms | 6 ms | ✅ |
| `057` | 17 | 191 B | 1 | 5.9% | 1..17 | 2 ms | 6 ms | ✅ |
| `058` | 18 | 204 B | 1 | 5.6% | 1..18 | 2 ms | 6 ms | ✅ |
| `059` | 18 | 204 B | 1 | 5.6% | 1..18 | 2 ms | 6 ms | ✅ |
| `060` | 12 | 126 B | 1 | 8.3% | 1..12 | 2 ms | 7 ms | ✅ |
| `061` | 977 | 16.3 KB | 1 | 0.1% | 1..977 | 2 ms | 7 ms | ✅ |
| `062` | 646 | 10.6 KB | 645 | 99.8% | 1..977 | 2 ms | 7 ms | ✅ |
| `063` | 535 | 8.7 KB | 535 | 100.0% | 1..977 | 2 ms | 7 ms | ✅ |
| `064` | 503 | 8.1 KB | 503 | 100.0% | 1..977 | 2 ms | 6 ms | ✅ |
| `065` | 713 | 11.7 KB | 713 | 100.0% | 1..977 | 2 ms | 7 ms | ✅ |
| `066` | 780 | 28.0 KB | 780 | 100.0% | ±1e9 | 2 ms | 7 ms | ✅ |
| `067` | 866 | 31.0 KB | 866 | 100.0% | ±1e9 | 2 ms | 7 ms | ✅ |
| `068` | 773 | 27.6 KB | 773 | 100.0% | ±1e9 | 2 ms | 7 ms | ✅ |
| `069` | 942 | 33.7 KB | 942 | 100.0% | ±1e9 | 2 ms | 7 ms | ✅ |
| `070` | 827 | 29.5 KB | 827 | 100.0% | ±1e9 | 2 ms | 9 ms | ✅ |
| `071` | 792 | 13.0 KB | 1 | 0.1% | 1..792 | 2 ms | 7 ms | ✅ |
| `072` | 726 | 11.9 KB | 19 | 2.6% | 1..726 | 2 ms | 7 ms | ✅ |
| `073` | 593 | 9.5 KB | 1 | 0.2% | 1..593 | 2 ms | 7 ms | ✅ |
| `074` | 971 | 16.2 KB | 119 | 12.3% | 1..971 | 2 ms | 8 ms | ✅ |
| `075` | 658 | 10.7 KB | 75 | 11.4% | 1..658 | 2 ms | 7 ms | ✅ |
| `076` | 17,948 | 401.1 KB | 17,948 | 100.0% | 1..17,948 | 11 ms | 35 ms | ✅ |
| `077` | 19,281 | 433.6 KB | 19,281 | 100.0% | 1..19,281 | 12 ms | 35 ms | ✅ |
| `078` | 16,938 | 380.4 KB | 16,937 | 100.0% | 1..19,281 | 11 ms | 34 ms | ✅ |
| `079` | 18,783 | 422.3 KB | 18,783 | 100.0% | 1..19,281 | 12 ms | 35 ms | ✅ |
| `080` | 17,725 | 398.3 KB | 17,725 | 100.0% | 1..19,281 | 11 ms | 34 ms | ✅ |
| `081` | 15,916 | 593.5 KB | 15,916 | 100.0% | ±1e9 | 12 ms | 40 ms | ✅ |
| `082` | 16,912 | 629.5 KB | 16,912 | 100.0% | ±1e9 | 12 ms | 38 ms | ✅ |
| `083` | 16,971 | 631.6 KB | 16,971 | 100.0% | ±1e9 | 12 ms | 38 ms | ✅ |
| `084` | 17,481 | 650.0 KB | 17,481 | 100.0% | ±1e9 | 13 ms | 40 ms | ✅ |
| `085` | 15,345 | 570.0 KB | 15,345 | 100.0% | ±1e9 | 11 ms | 34 ms | ✅ |
| `086` | 18,148 | 406.0 KB | 1 | 0.0% | 1..18,148 | 8 ms | 28 ms | ✅ |
| `087` | 16,232 | 359.2 KB | 2,591 | 16.0% | 1..16,232 | 7 ms | 27 ms | ✅ |
| `088` | 15,290 | 336.2 KB | 2,921 | 19.1% | 1..15,290 | 8 ms | 25 ms | ✅ |
| `089` | 17,967 | 401.6 KB | 1,432 | 8.0% | 1..17,967 | 8 ms | 27 ms | ✅ |
| `090` | 15,281 | 336.0 KB | 1,571 | 10.3% | 1..15,281 | 7 ms | 25 ms | ✅ |
| `091` | 99,707 | 2.4 MB | 99,707 | 100.0% | 1..99,707 | 66 ms | 174 ms | ✅ |
| `092` | 99,933 | 2.4 MB | 99,933 | 100.0% | 1..99,933 | 65 ms | 170 ms | ✅ |
| `093` | 93,610 | 2.3 MB | 93,610 | 100.0% | 1..99,933 | 61 ms | 165 ms | ✅ |
| `094` | 99,137 | 2.4 MB | 99,137 | 100.0% | 1..99,933 | 64 ms | 178 ms | ✅ |
| `095` | 99,985 | 2.4 MB | 99,985 | 100.0% | 1..99,985 | 64 ms | 181 ms | ✅ |
| `096` | 97,948 | 3.6 MB | 97,948 | 100.0% | ±1e9 | 69 ms | 195 ms | ✅ |
| `097` | 96,971 | 3.6 MB | 96,971 | 100.0% | ±1e9 | 68 ms | 201 ms | ✅ |
| `098` | 96,761 | 3.6 MB | 96,761 | 100.0% | ±1e9 | 68 ms | 192 ms | ✅ |
| `099` | 99,351 | 3.7 MB | 99,351 | 100.0% | ±1e9 | 71 ms | 191 ms | ✅ |
| `100` | 94,081 | 3.5 MB | 94,081 | 100.0% | ±1e9 | 66 ms | 184 ms | ✅ |
| `101` | 90,805 | 2.2 MB | 704 | 0.8% | 1..90,805 | 36 ms | 120 ms | ✅ |
| `102` | 92,481 | 2.2 MB | 557 | 0.6% | 1..92,481 | 36 ms | 121 ms | ✅ |
| `103` | 90,336 | 2.2 MB | 224 | 0.2% | 1..90,336 | 34 ms | 119 ms | ✅ |
| `104` | 99,986 | 2.4 MB | 194 | 0.2% | 1..99,986 | 38 ms | 165 ms | ✅ |
| `105` | 94,072 | 2.3 MB | 169 | 0.2% | 1..94,072 | 34 ms | 122 ms | ✅ |
| `106` | 100,000 | 2.4 MB | 100,000 | 100.0% | 1..100,000 | 65 ms | 170 ms | ✅ |
| `107` | 100,000 | 2.4 MB | 100,000 | 100.0% | 1..100,000 | 65 ms | 173 ms | ✅ |
| `108` | 100,000 | 2.4 MB | 100,000 | 100.0% | 1..100,000 | 65 ms | 170 ms | ✅ |
| `109` | 100,000 | 2.4 MB | 100,000 | 100.0% | 1..100,000 | 65 ms | 172 ms | ✅ |
| `110` | 100,000 | 2.4 MB | 100,000 | 100.0% | 1..100,000 | 64 ms | 172 ms | ✅ |
| `111` | 100,000 | 3.7 MB | 100,000 | 100.0% | ±1e9 | 70 ms | 197 ms | ✅ |
| `112` | 100,000 | 3.7 MB | 100,000 | 100.0% | ±1e9 | 71 ms | 201 ms | ✅ |
| `113` | 100,000 | 3.7 MB | 100,000 | 100.0% | ±1e9 | 70 ms | 197 ms | ✅ |
| `114` | 100,000 | 3.7 MB | 100,000 | 100.0% | ±1e9 | 71 ms | 200 ms | ✅ |
| `115` | 100,000 | 3.7 MB | 100,000 | 100.0% | ±1e9 | 70 ms | 199 ms | ✅ |
| `116` | 100,000 | 2.4 MB | 465 | 0.5% | 1..100,000 | 37 ms | 131 ms | ✅ |
| `117` | 100,000 | 2.4 MB | 1,270 | 1.3% | 1..100,000 | 39 ms | 131 ms | ✅ |
| `118` | 100,000 | 2.4 MB | 1 | 0.0% | 1..100,000 | 39 ms | 130 ms | ✅ |
| `119` | 100,000 | 2.4 MB | 119 | 0.1% | 1..100,000 | 38 ms | 129 ms | ✅ |
| `120` | 100,000 | 2.4 MB | 280 | 0.3% | 1..100,000 | 38 ms | 130 ms | ✅ |
| `121` | 3 | 74 B | 1 | 33.3% | ±1e9 | 2 ms | 5 ms | ✅ |
| `122` | 92,292 | 2.1 MB | 92,292 | 100.0% | 0..92,291 | 46 ms | 148 ms | ✅ |
| `123` | 97,071 | 2.2 MB | 32,357 | 33.3% | 0..97,070 | 37 ms | 133 ms | ✅ |
| `124` | 90,436 | 2.0 MB | 30,144 | 33.3% | 0..90,435 | 34 ms | 121 ms | ✅ |
| `125` | 91,328 | 2.0 MB | 30,442 | 33.3% | 0..91,327 | 35 ms | 124 ms | ✅ |
| `126` | 95,362 | 2.1 MB | 23,839 | 25.0% | 0..95,361 | 26 ms | 117 ms | ✅ |
| `127` | 90,235 | 2.0 MB | 45,117 | 50.0% | 0..90,234 | 29 ms | 117 ms | ✅ |
| `128` | 96,339 | 2.2 MB | 48,169 | 50.0% | 0..96,338 | 31 ms | 124 ms | ✅ |
| `129` | 96,497 | 2.2 MB | 32,165 | 33.3% | 0..96,496 | 28 ms | 128 ms | ✅ |
| `130` | 92,190 | 2.1 MB | 46,095 | 50.0% | 0..92,189 | 30 ms | 119 ms | ✅ |
| `131` | 100,000 | 2.2 MB | 50,000 | 50.0% | 0..99,999 | 41 ms | 141 ms | ✅ |
| `132` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 52 ms | 158 ms | ✅ |
| `133` | 100,000 | 2.2 MB | 50,000 | 50.0% | 0..99,999 | 41 ms | 143 ms | ✅ |
| `134` | 100,000 | 2.2 MB | 50,000 | 50.0% | 0..99,999 | 41 ms | 143 ms | ✅ |
| `135` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 52 ms | 165 ms | ✅ |
| `136` | 100,000 | 2.2 MB | 50,000 | 50.0% | 0..99,999 | 33 ms | 136 ms | ✅ |
| `137` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 42 ms | 153 ms | ✅ |
| `138` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 42 ms | 149 ms | ✅ |
| `139` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 42 ms | 154 ms | ✅ |
| `140` | 100,000 | 2.2 MB | 100,000 | 100.0% | 0..99,999 | 42 ms | 149 ms | ✅ |

</details>

---

## <a id="triopt"></a>`triopt` — 65/65 ✅

Optimize `F1(x) + F2(y) + F3(z)` with `x + y + z = S`, each `Fi` being sin or cos.

**Algorithm:** Sum-to-product identity collapses the two repeated functions to a cached extremum.

```bash
python3 check.py ./triopt --tol=1e-12
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 301 ms | 48 ms | `034` | 17 ms | 30 ms |
| `002` | 10 ms | 19 ms | `035` | 15 ms | 29 ms |
| `003` | 9 ms | 17 ms | `036` | 16 ms | 30 ms |
| `004` | 8 ms | 18 ms | `037` | 6 ms | 17 ms |
| `005` | 8 ms | 18 ms | `038` | 16 ms | 29 ms |
| `006` | 8 ms | 17 ms | `039` | 16 ms | 31 ms |
| `007` | 8 ms | 17 ms | `040` | 17 ms | 33 ms |
| `008` | 7 ms | 17 ms | `041` | 16 ms | 29 ms |
| `009` | 7 ms | 16 ms | `042` | 8 ms | 20 ms |
| `010` | 7 ms | 16 ms | `043` | 16 ms | 30 ms |
| `011` | 6 ms | 17 ms | `044` | 17 ms | 29 ms |
| `012` | 7 ms | 16 ms | `045` | 17 ms | 30 ms |
| `013` | 7 ms | 17 ms | `046` | 16 ms | 31 ms |
| `014` | 6 ms | 17 ms | `047` | 12 ms | 27 ms |
| `015` | 6 ms | 17 ms | `048` | 7 ms | 18 ms |
| `016` | 6 ms | 17 ms | `049` | 16 ms | 30 ms |
| `017` | 6 ms | 17 ms | `050` | 16 ms | 30 ms |
| `018` | 6 ms | 17 ms | `051` | 16 ms | 32 ms |
| `019` | 6 ms | 18 ms | `052` | 16 ms | 29 ms |
| `020` | 6 ms | 18 ms | `053` | 8 ms | 20 ms |
| `021` | 6 ms | 17 ms | `054` | 7 ms | 17 ms |
| `022` | 6 ms | 19 ms | `055` | 16 ms | 29 ms |
| `023` | 6 ms | 17 ms | `056` | 17 ms | 30 ms |
| `024` | 6 ms | 18 ms | `057` | 16 ms | 29 ms |
| `025` | 6 ms | 18 ms | `058` | 16 ms | 31 ms |
| `026` | 6 ms | 17 ms | `059` | 11 ms | 24 ms |
| `027` | 16 ms | 29 ms | `060` | 6 ms | 18 ms |
| `028` | 16 ms | 31 ms | `061` | 16 ms | 30 ms |
| `029` | 16 ms | 31 ms | `062` | 16 ms | 29 ms |
| `030` | 15 ms | 29 ms | `063` | 16 ms | 29 ms |
| `031` | 9 ms | 21 ms | `064` | 15 ms | 30 ms |
| `032` | 17 ms | 29 ms | `065` | 8 ms | 21 ms |
| `033` | 17 ms | 32 ms |  |  |  |


---

## <a id="water"></a>`water` — 14/14 ✅

Water levels in `N` spherical tanks after pouring in volume `V`.

**Algorithm:** Binary search on the level; spherical-cap volume `pi*h²*(3r-h)/3`.

```bash
python3 check.py ./water --tol=1e-3
```

| Test | macOS | Windows | Test | macOS | Windows |
|---|---:|---:|---|---:|---:|
| `001` | 288 ms | 59 ms | `008` | 2 ms | 7 ms |
| `002` | 6 ms | 8 ms | `009` | 2 ms | 7 ms |
| `003` | 6 ms | 6 ms | `010` | 2 ms | 6 ms |
| `004` | 4 ms | 7 ms | `011` | 2 ms | 6 ms |
| `005` | 3 ms | 6 ms | `012` | 2 ms | 6 ms |
| `006` | 2 ms | 6 ms | `013` | 4 ms | 7 ms |
| `007` | 2 ms | 6 ms | `014` | 4 ms | 8 ms |


---

## <a id="why---str"></a>Why `--str`

`check.py` compares token-by-token. Its numeric comparator tries `float(token)` first
and only falls back to string equality when that fails — which is right for the six
geometry problems, and wrong for the two whose answers are text.

`tournament` is the sharp case. Sith names are arbitrary letter words, and 9 tests
contain a sith named `nan` or `inf` — both of which `float()` happily accepts:

| Test | Name | Under `--tol` | Under `--str` |
|---|---|---|:---:|
| `122` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `127` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `131` | `inf` | passes by luck | ✅ |
| `132` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `135` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `137` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `138` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `139` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |
| `140` | `inf`, `nan` | **fails** — `nan != nan` | ✅ |

`math.isclose(nan, nan)` is `False`, so eight correct outputs were reported as
`Numeric mismatch: expected nan, got nan` and the suite scored 132/140. Test `131`
holds `inf` but no `nan`, and passed only because `isclose(inf, inf)` is `True`.

`--str` routes to a separate `compare_strings()` that never converts anything to a
float, so no name can be reinterpreted as a number. `quad` uses it for the same
reason: its output is LaTeX such as `x \in \mathbb{R}`.

---

## <a id="reproduce"></a>Reproduce

```bash
set PYTHONIOENCODING=utf-8        # Windows only, for the emoji summary line
python3 check.py ./gsm         --tol=1e-5
python3 check.py ./parabola    --tol=1e-6
python3 check.py ./pool        --tol=1e-3
python3 check.py ./quad        --str
python3 check.py ./slalom      --tol=1e-3
python3 check.py ./tournament  --str
python3 check.py ./triopt      --tol=1e-12
python3 check.py ./water       --tol=1e-3
```

`check.py` compiles `<folder>/main.cpp`, feeds each `tests/*.in` on stdin, and diffs
stdout against the matching `.out`. `--src` compiles a different file from the problem
folder instead of `main.cpp`, which is how a local variant is checked against the same
tests without committing it — only `main.cpp` is committed. A problem with no `tests/`
directory but a `tests.zip`, currently only `tournament`, is unpacked into a temporary
directory automatically and cleaned up afterwards. On Windows use `python`, not
`python3`.
