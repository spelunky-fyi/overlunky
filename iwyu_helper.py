import argparse
import subprocess
import sys
import json
import shutil
import os


def main():
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description="IWYU helper script, calls iwyu_tool.py and passes output to fix_includes.py"
    )
    parser.add_argument(
        "--iwyu_exe",
        "-i",
        default=None,
        help="Path to include-what-you-use executable. Must be passed if --iwyu_tool is passed.",
    )
    parser.add_argument(
        "--iwyu_tool",
        "-t",
        default=None,
        help="Path to iwyu_tool.py to execute.",
    )
    parser.add_argument(
        "--iwyu_tool_output",
        "-o",
        default=None,
        help="Path to iwyu_tool output, pass instead of --iwyu_tool to avoid running the tool.",
    )
    parser.add_argument(
        "--compile_commands",
        "-c",
        default=None,
        help="Path to compile_commands.json file to pass to IWYU.",
    )
    parser.add_argument(
        "--extra_args",
        "-e",
        default=[],
        action="append",
        help="Extra arguments to pass to IWYU.",
    )
    parser.add_argument(
        "--fix_includes",
        "-f",
        default=None,
        required=True,
        help="Path to fix_includes.py to execute.",
    )

    args = parser.parse_args()

    if args.iwyu_tool:
        # Create a temp folder
        tmp_dir: str = "iwyu_tmp"
        if os.path.exists(tmp_dir):
            if os.path.isdir(tmp_dir):
                shutil.rmtree(tmp_dir, ignore_errors=False, onerror=None)
            else:
                os.remove(tmp_dir)
        os.makedirs(tmp_dir)

        if not args.compile_commands:
            parser.error("--iwyu_tool requires --compile_commands")
        if not args.iwyu_exe:
            parser.error("--iwyu_tool requires --iwyu_exe")

        # Fix up compile_commands.json
        with open(args.compile_commands, "r", encoding="utf-8") as cc_file:
            compile_commands = json.load(cc_file)
            # Strip PCH compiles
            compile_commands = [
                compile_command
                for compile_command in compile_commands
                if not compile_command["command"].endswith("cmake_pch.hxx.cxx")
            ]
            # Append extra agrs
            if args.extra_args:
                for compile_command in compile_commands:
                    compile_command["command"] += " " + " ".join(args.extra_args)
            # Dump fixed file
            with open(
                "iwyu_tmp/compile_commands.json", "w", encoding="utf-8"
            ) as cc_file_out:
                json.dump(compile_commands, cc_file_out, indent=2)

        # Call iwyu_tool.py
        iwyu_tool_args = [
            "python",
            args.iwyu_tool,
            "-p",
            tmp_dir,
            "-v",
        ]
        os.environ["IWYU_BINARY"] = args.iwyu_exe
        iwyu_out = os.path.join(tmp_dir, "iwyu_tool_out")
        with open(iwyu_out, "w") as iwyu_out_file:
            subprocess.call(iwyu_tool_args, stdout=iwyu_out_file)
    elif args.iwyu_tool_output:
        iwyu_out = args.iwyu_tool_output
    else:
        parser.error("either --iwyu_tool or --iwyu_tool_output required")

    # Call fix_includes.py with the output from iwyu_tool.py
    fix_includes_args = [
        "python",
        args.fix_includes,
        "--comments",
        "--update_comments",
    ]
    with open(iwyu_out, "r") as fix_includes_in_file:
        subprocess.call(fix_includes_args, stdin=fix_includes_in_file)


if __name__ == "__main__":
    sys.exit(main())
