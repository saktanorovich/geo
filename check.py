import argparse
import subprocess
import pathlib
import sys
import os
import math
import time
import platform
import contextlib
import shutil
import tempfile
import zipfile

if os.name == "nt":
    import ctypes
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

# Ensure emoji/ANSI output works regardless of the console's default encoding
# (Windows defaults to cp1252, which can't encode ✅/❌).
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):
        pass

FAILED_COLOR = "\033[91m"
PASSED_COLOR = "\033[92m"
SYSTEM_COLOR = "\033[0m"

def compile(src_file, out_file):
    try:
        print(f"Compiling {src_file} -> {out_file} ..")
        subprocess.run(
            ["g++", "-std=c++17", "-O2", "-o", out_file, src_file],
            check=True
        )
        print("Compilation successful ✅")
        return out_file
    except subprocess.CalledProcessError as e:
        print(f"Compilation failed ❌: {e}")
        return None

def to_float(s: str):
    try:
        return True, float(s)
    except ValueError:
        return False, None

def tokenize(text: str):
    lines = text.splitlines()
    while lines and lines[-1].strip() == "":
        lines.pop()
    return [line.split() for line in lines]

def compare_shape(exp_lines, act_lines):
    if len(exp_lines) != len(act_lines):
        return False, f"Line count differs: expected {len(exp_lines)}, got {len(act_lines)}"
    for i, (el, al) in enumerate(zip(exp_lines, act_lines), start=1):
        if len(el) != len(al):
            return False, f"Token count differs on line {i}: expected {len(el)}, got {len(al)}"
    return True, "OK"

def compare_strings(exp: str, act: str):
    """Every token is a plain string: compare them verbatim, never as numbers."""
    exp_lines, act_lines = tokenize(exp), tokenize(act)
    ok, reason = compare_shape(exp_lines, act_lines)
    if not ok:
        return False, reason
    for i, (el, al) in enumerate(zip(exp_lines, act_lines), start=1):
        for j, (et, at) in enumerate(zip(el, al), start=1):
            if et != at:
                return False, f"String mismatch at line {i}, token {j}: expected '{et}', got '{at}'"
    return True, "OK"

def compare_numbers(exp: str, act: str, tol: float):
    """Compare numeric tokens within tol, falling back to string equality."""
    exp_lines, act_lines = tokenize(exp), tokenize(act)
    ok, reason = compare_shape(exp_lines, act_lines)
    if not ok:
        return False, reason
    for i, (el, al) in enumerate(zip(exp_lines, act_lines), start=1):
        for j, (et, at) in enumerate(zip(el, al), start=1):
            e_is_num, e_val = to_float(et)
            a_is_num, a_val = to_float(at)
            if e_is_num and a_is_num:
                if not math.isclose(e_val, a_val, rel_tol=tol, abs_tol=tol):
                    return False, (
                        f"Numeric mismatch at line {i}, token {j}: "
                        f"expected {e_val}, got {a_val} (tol={tol})"
                    )
            else:
                if et != at:
                    return False, f"String mismatch at line {i}, token {j}: expected '{et}', got '{at}'"
    return True, "OK"

def format_time(ms: int, width: int = 8) -> str:
    formatted = f"{ms:,}".replace(",", "'")
    return f"{formatted:>{width}}ms"

def run_test(executable, infile, outfile, tol, str_mode=False):
    try:
        with open(infile, "r") as fin:
            start = time.perf_counter()
            result = subprocess.run(
                [executable],
                stdin=fin,
                capture_output=True,
                text=True,
                check=False
            )
            end = time.perf_counter()
            elapsed_ms = int((end - start) * 1000)
            time_str = format_time(elapsed_ms, width=6)
    except Exception as e:
        print(f"Error running {executable} with {infile}: {e}")
        return False
    try:
        expected = pathlib.Path(outfile).read_text()
    except FileNotFoundError:
        print(f"Expected output file not found: {outfile}")
        return False
    actual = result.stdout
    if str_mode:
        passed, reason = compare_strings(expected, actual)
    else:
        passed, reason = compare_numbers(expected, actual, tol)
    verdict = "[PASS]" if passed else "[FAIL]"
    log_str = f"{verdict:<8}{infile.name:>8}{time_str:>12}"
    if not passed:
        print(f"{FAILED_COLOR}{log_str}{SYSTEM_COLOR}")
        print(reason)
    else:
        print(f"{PASSED_COLOR}{log_str}{SYSTEM_COLOR}")
    return passed

@contextlib.contextmanager
def tests_directory(folder):
    """Locate the tests for a problem, yielding None when there are none.

    A problem normally keeps its tests in <folder>/tests. When that is absent but
    <folder>/tests.zip is present, the archive is unpacked into a temporary
    directory and removed again on the way out, so a large test set can live in
    the repo packed without ever leaving unpacked files behind.
    """
    plain = folder / "tests"
    if plain.is_dir():
        yield plain
        return
    archive = folder / "tests.zip"
    if not archive.is_file():
        yield None
        return
    temp_dir = pathlib.Path(tempfile.mkdtemp(prefix="check_tests_"))
    try:
        size_mb = archive.stat().st_size / (1024 * 1024)
        print(f"Unpacking {archive} ({size_mb:,.1f} MB) ..")
        with zipfile.ZipFile(archive) as bundle:
            bundle.extractall(temp_dir)
        # Tolerate archives that wrap their tests in a top-level tests/ folder
        # as well as those that hold the .in/.out files directly.
        inner = temp_dir / "tests"
        yield inner if inner.is_dir() else temp_dir
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)
        print("Unpacked tests removed ✅")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("folder")
    parser.add_argument("--tol", type=float, default=1e-4)
    parser.add_argument("--out", type=str, default="program")
    parser.add_argument("--src", type=str, default="main.cpp",
                        help="source file (relative to folder) to compile")
    parser.add_argument("--str", dest="str_mode", action="store_true",
                        help="compare output as a list of strings (exact token "
                             "match, no numeric tolerance)")
    args = parser.parse_args()
    folder_path = pathlib.Path(args.folder)
    if not folder_path.is_dir():
        print(f"Error: {folder_path} is not a directory")
        sys.exit(1)
    source_file = folder_path / args.src
    if not source_file.exists():
        print(f"Error: {source_file} not found")
        sys.exit(1)
    exe_name = args.out
    if platform.system() == "Windows" and not exe_name.endswith(".exe"):
        exe_name += ".exe"
    executable = compile(str(source_file), exe_name)
    if not executable:
        sys.exit(1)
    try:
        with tests_directory(folder_path) as tests_dir:
            if tests_dir is None:
                print(f"Error: neither {folder_path / 'tests'} nor "
                      f"{folder_path / 'tests.zip'} found")
                sys.exit(1)
            in_files = sorted(tests_dir.glob("*.in"))
            if not in_files:
                print(f"No .in files found in {tests_dir}")
                sys.exit(1)
            test_count = len(in_files)
            pass_count = 0
            print(f"{'Verdict':<8}{'Test':>8}{'Time':>12}")
            time.perf_counter()  # warm up timer
            exe_path = str(pathlib.Path(executable).resolve())
            for infile in in_files:
                outfile = infile.with_suffix(".out")
                if run_test(exe_path, infile, outfile, args.tol, args.str_mode):
                    pass_count += 1
            status = "✅" if pass_count == test_count else "❌"
            summary_line = f"\nSummary {status}: {pass_count}/{test_count} tests passed"
            if pass_count == test_count:
                print(f"{PASSED_COLOR}{summary_line}{SYSTEM_COLOR}")
            else:
                print(f"{FAILED_COLOR}{summary_line}{SYSTEM_COLOR}")
    finally:
        if os.path.exists(executable):
            os.remove(executable)

if __name__ == "__main__":
    main()