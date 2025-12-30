import argparse
import subprocess
import pathlib
import sys
import os
import difflib
import math
import time
import platform

if os.name == "nt":
    import ctypes
    kernel32 = ctypes.windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

FAILED_COLOR = "\033[91m"
PASSED_COLOR = "\033[92m"
SYSTEM_COLOR = "\033[0m"

def compile(src_file, out_file):
    try:
        print(f"Compiling {src_file} -> {out_file} ...")
        subprocess.run(
            ["g++", "-std=c++17", "-O2", "-o", out_file, src_file],
            check=True
        )
        print("Compilation successful ✅")
        return out_file
    except subprocess.CalledProcessError as e:
        print(f"Compilation failed ❌: {e}")
        return None

def compare(expected: str, actual: str, tol: float) -> bool:
    try:
        exp_val = float(expected)
        act_val = float(actual)
    except ValueError:
        return expected.strip() == actual.strip()
    return math.isclose(exp_val, act_val, rel_tol=tol, abs_tol=tol)

def format_time(ms: int, width: int = 8) -> str:
    formatted = f"{ms:,}".replace(",", "'")
    return f"{formatted:>{width}}ms"

def run_test(executable, infile, outfile, tol):
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
    passed = compare(expected, actual, tol)

    verdict = "[PASS]" if passed else "[FAIL]"
    log_str = f"{verdict:<8}{infile.name:>8}{time_str:>12}"

    if not passed:
        print(f"{FAILED_COLOR}{log_str}{SYSTEM_COLOR}")
    else:
        print(f"{PASSED_COLOR}{log_str}{SYSTEM_COLOR}")

    if not passed:
        print("Expected:")
        print(expected)
        print("Actual:")
        print(actual)
        diff = difflib.unified_diff(
            expected.splitlines(),
            actual.splitlines(),
            fromfile="expected",
            tofile="actual",
            lineterm=""
        )
        print("\n".join(diff))

    return passed

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("folder")
    parser.add_argument("--tol", type=float, default=1e-4)
    parser.add_argument("--out", type=str, default="program")
    args = parser.parse_args()

    folder_path = pathlib.Path(args.folder)
    if not folder_path.is_dir():
        print(f"Error: {folder_path} is not a directory")
        sys.exit(1)

    source_file = folder_path / "main.cpp"
    tests_dir = folder_path / "tests"

    if not source_file.exists():
        print(f"Error: {source_file} not found")
        sys.exit(1)
    if not tests_dir.is_dir():
        print(f"Error: {tests_dir} not found")
        sys.exit(1)

    exe_name = args.out
    if platform.system() == "Windows" and not exe_name.endswith(".exe"):
        exe_name += ".exe"

    executable = compile(str(source_file), exe_name)
    if not executable:
        sys.exit(1)

    in_files = sorted(tests_dir.glob("*.in"))
    if not in_files:
        print("No .in files found in tests/")
        sys.exit(1)

    test_count = len(in_files)
    pass_count = 0

    print(f"{'Verdict':<8}{'Test':>8}{'Time':>12}")
    for infile in in_files:
        outfile = infile.with_suffix(".out")
        exe_path = str(pathlib.Path(executable).resolve())
        if run_test(exe_path, infile, outfile, args.tol):
            pass_count += 1

    status = "✅" if pass_count == test_count else "❌"
    summary_line = f"\nSummary {status}: {pass_count}/{test_count} tests passed"
    if pass_count == test_count:
        print(f"{PASSED_COLOR}{summary_line}{SYSTEM_COLOR}")
    else:
        print(f"{FAILED_COLOR}{summary_line}{SYSTEM_COLOR}")

    os.remove(executable)

if __name__ == "__main__":
    main()
