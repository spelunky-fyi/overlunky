import sys


def setup_stdout(filename):
    sys.stdout = sys.__stdout__
    print(f"Generating file '{filename}'...")
    sys.stdout = open(f"{filename}", "w")


def cleanup_stdout():
    sys.stdout.close()
    sys.stdout = sys.__stdout__


def print_collecting_info(data):
    print(f"Collecting {data} data...")


def print_console(*args, **kwargs):
    sys.stdout, stdout = sys.__stdout__, sys.stdout
    print(*args, **kwargs)
    sys.stdout = stdout


def breakpoint():
    sys.stdout, stdout = sys.__stdout__, sys.stdout
    import pdb

    pdb.set_trace()
    sys.stdout = stdout
