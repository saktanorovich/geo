---
name: problem-solver
description: Solve, verify or optimise a problem in this repo end-to-end - extract the constraints from its statement, derive the algorithm against the bounds, run check.py, profile before optimising, and add tests whose expected values come from an analytic construction or an independent oracle rather than from the solution itself.
---

# Solving a problem in this repo

One directory per problem, each holding `main.cpp`, the statement as `task-ru.docx`
(sometimes also `.pdf`), and `tests/` — or `tests.zip` when the data is large.

Harness, from the repo root:

```bash
python check.py ./<problem> --tol=1e-6      # numeric comparison
python check.py ./<problem> --str           # exact token comparison
python check.py ./<problem> --src other.cpp # compile something else, not main.cpp
```

- Use `python`, not `python3`; set `PYTHONIOENCODING=utf-8` so the summary prints.
- The right `--tol` is per-problem and comes from the statement. `REPORT.md` records
  the exact command for each — read it there rather than guessing, so the two cannot
  drift apart.
- `check.py` compiles with `g++ -std=c++17 -O2`, feeds each `tests/NNN.in` on stdin and
  diffs stdout against `tests/NNN.out`. Tests use a 3-digit schema.
- It is import-safe, so verification scripts can `import check` and reuse
  `check.compare_numbers(expected, actual, tol)` / `check.compare_strings(...)`.
- Only `main.cpp` is committed per problem. Oracles, benchmark variants and scratch
  implementations stay outside the repo — build them in the scratch directory and point
  `--src` at one when it needs to run against the suite.

## 1. Read the statement before anything else

`task-ru.docx` is a zip. Extract it — never work from the tests alone:

```python
import re, zipfile
with zipfile.ZipFile(r"<problem>/task-ru.docx") as z:
    xml = z.read("word/document.xml").decode("utf-8")
text = re.sub(r"<[^>]+>", "", xml.replace("</w:p>", "\n"))
print(text.replace("&amp;", "&").replace("&lt;", "<").replace("&gt;", ">"))
```

Write down the I/O format, the bounds, the required precision, the tie and degeneracy
guarantees, and every "assume X holds" clause. These decide both the design and which
tests are even in spec, and they are easy to miss:

- A guarantee that no two inputs share a value can make a relation total, which may
  turn a messy case analysis into a clean structure.
- A guarantee that a solution always exists means a test expecting "impossible" is out
  of spec and must not be written.
- A required output *order* is part of the answer. Sorted or discovery order is wrong if
  the statement asks for input order.
- Watch for exclusions such as a coefficient that cannot be zero: they remove
  degenerate branches you would otherwise have to handle, and relying on one is only
  safe if the statement really says so.

Note the numeric ranges separately from the counts. A wide value range with a small
element count is often the regime where a naive method loses accuracy.

## 2. Derive the algorithm, then write the reduction down

Put the argument in a comment at the top of the solution — why it is correct, not just
what it does. Check it against the bounds before coding: a pairwise relation over `1e5`
elements implies on the order of `5e9` pairs, which can never be materialised, so each
step has to be answered implicitly by a structure rather than by enumeration.

## 3. Implement

- Read stdin, write stdout. Never `freopen` — it silently produces no output under the
  harness.
- Choose `--str` whenever an output token is text rather than a number. This is not
  cosmetic: `float()` accepts `nan` and `inf`, so a text token spelled either way is
  silently reinterpreted as a number, and `math.isclose(nan, nan)` is `False`, which
  fails a correct answer with a nonsensical message. Symbolic output such as LaTeX needs
  it for the opposite reason — a tolerance is meaningless there.
- Size structures from `n`, not from a `max_n` bound.

## 4. Profile before optimising

Measure; never guess where the time goes. Build a probe that only parses the input and
does nothing else, and time it against the full solution. In one case that showed
`scanf` alone accounting for 116 ms of a 194 ms run while the entire algorithm was
~78 ms — so the target was I/O, not the algorithm. Replacing it with a hand-rolled 64 KB
`fread` scanner and a matching write buffer took a suite from 12.2 s to 3.1 s with the
algorithm untouched.

Then ablate each change separately and report what reverting it costs, so the
attribution is measured rather than assumed. In that same case, summed over the five
heaviest tests against a 260 ms baseline: `scanf` input +471 ms, `printf` output +41 ms,
no early exit +28 ms, insertion-based tree build +11 ms. Only the first mattered much,
and guessing would have picked wrong.

Wins that generalise: build a tree bottom-up in `O(n)` rather than by `n` insertions;
stop a search the moment the answer is settled; prefer an exact closed-form solve to an
iterative search when one exists — it is usually both correct and faster.

## 5. Verify — never trust the solution's own output

An expected value taken from the program under test bakes in that program's bugs. Source
expected values from one of:

- **An analytic construction** provable from the statement alone. Build the input so the
  answer is forced by an argument rather than computed: arrange for a trivial candidate
  to be provably optimal (a lower bound that is also achievable), or force the answer
  through cut points that every solution must pass, so its value is a fixed sum. Neither
  form assumes anything about how the solution searches.
- **An independent oracle built on a different principle.** If the solution decomposes a
  problem one way, make the oracle decompose it another way, then compare. Agreement
  between two structurally different methods is evidence; a program agreeing with itself
  is not. Where a closed form exists, prefer it in the oracle and keep a brute-force
  scan as a third opinion.

Validate the oracle before trusting it: it must reproduce every existing expected output
first. Then cross-check all applicable methods and only emit a test when they agree.

In the generator, assert the statement's constraints for every generated test, so an
out-of-spec case can never reach the suite.

Reject fragile cases: skip configurations within a small epsilon of a tangency or any
other boundary decision, where two correct implementations may legitimately disagree at
the comparison tolerance. Round values to the precision actually written to the file and
compute expected values from the *rounded* numbers, since that is what the solution
reads. Where an exact coincidence is structural, build it from quantities that survive
being written as decimal text — rounding it apart silently changes the problem.

Random sampling is not a substitute for construction. A defect can sit in a corner that
uniform random inputs essentially never hit; if a sweep of thousands finds nothing,
that is evidence about the sampling distribution, not about the code.

## 6. Prove each new test is load-bearing

Revert each fix independently and record which tests fail. The matrix is the deliverable
— it shows the tests catch the defect and that the old code fails them:

| variant | passed | failing |
|---|---|---|
| both fixes | 100/100 | - |
| fix A reverted | 87/100 | 13 tests |
| fix B reverted | 85/100 | 15 tests |
| neither | 76/100 | 24 tests |

Watch the suite's cost. A correct test that takes seconds on its own is worth dropping
in favour of a cheap one that still exercises the same bound.

If a defect is found but deliberately left unfixed, keep its tests committed and
failing, and say so in `REPORT.md` — a red suite that is explained is better than a
green one that hides a known bug.

## 7. Recurring bug classes worth checking

- **A comparator or split valid only in the non-degenerate case.** Ordering by a single
  coordinate ties when that coordinate is constant, and a sort then returns an arbitrary
  permutation. Order by a quantity that is always well defined.
- **A partial intersection dropped.** Accepting a crossing only when *all* roots fall in
  range means a boundary that clips the range once contributes nothing, and a valid
  configuration gets rejected.
- **A search assuming unimodality.** Ternary and golden-section search are only valid on
  a unimodal interval. Prove unimodality or subdivide; a distance function along a curve
  usually has several stationary points, and splitting at one of them does not
  necessarily separate the rest.

## 8. When reporting

State what was measured versus assumed. Give run-to-run spread when claiming a speedup —
repeat the sweep, since a single pair of runs can differ by more than the effect. Say
plainly which tests, if any, fail to distinguish a change, and name what is a mitigation
rather than a proof.
