import subprocess
import os
import difflib

# Test matrix: (input_file, pattern, options, description)
test_cases = [
    ("basic.txt", "foo", "", "Simple literal match"),
    ("basic.txt", "bar", "-i", "Case-insensitive match"),
    ("unicode.txt", "café", "", "Unicode literal"),
    ("unicode.txt", "\\X", "-o", "Grapheme cluster extraction"),
    ("multiline.txt", "^foo.*bar$", "-m", "Multiline pattern"),
    ("lookaround.txt", "foo(?=bar)", "", "Lookahead assertion"),
    ("basic.txt", "foo", "-v", "Invert match"),
    ("basic.txt", "foo", "-n", "Line numbers"),
    ("basic.txt", "foo", "-c", "Count only"),
    ("context.txt", "foo", "-A1", "After context"),
    ("context.txt", "foo", "-B1", "Before context"),
    ("context.txt", "foo", "-C1", "Context (before & after)"),
    ("delimiter.txt", "foo", "-o --delim=|||", "Custom delimiter (if supported)"),
    ("error.txt", "(unclosed", "", "Invalid pattern"),
    ("unicode.txt", "\\p{Han}+", "", "Unicode script property"),
]

GRAPA_CMD = "..\\..\\grapa.exe -q -ccmd"
RIPGREP_CMD = "rg"
TEST_DIR = os.path.dirname(os.path.abspath(__file__))
INPUT_DIR = os.path.join(TEST_DIR, "inputs")

def run_grapa(input_file, pattern, options):
    with open(os.path.join(INPUT_DIR, input_file), "r", encoding="utf-8") as f:
        data = f.read().replace("\"", "\\\"")
    # Compose Grapa command
    grc = f'result = ("{data}").grep("{pattern}", "{options}"); result.str().echo();'
    cmd = ["..\\..\\grapa.exe", "-q", "-ccmd", grc]
    try:
        out = subprocess.check_output(cmd, encoding="utf-8", stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        out = e.output
    return out.strip()

def run_ripgrep(input_file, pattern, options):
    input_path = os.path.join(INPUT_DIR, input_file)
    opts = options.split()
    cmd = [RIPGREP_CMD] + opts + [pattern, input_path]
    try:
        out = subprocess.check_output(cmd, encoding="utf-8", stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
        out = e.output
    return out.strip()

def normalize_output(s):
    # Remove extra whitespace, normalize line endings
    return "\n".join(line.strip() for line in s.strip().splitlines() if line.strip())

def main():
    print("Grapa vs. ripgrep Parity Test Results:\n")
    for idx, (input_file, pattern, options, desc) in enumerate(test_cases, 1):
        print(f"Test {idx}: {desc}")
        grapa_out = normalize_output(run_grapa(input_file, pattern, options))
        rg_out = normalize_output(run_ripgrep(input_file, pattern, options))
        if grapa_out == rg_out:
            print("  PASS")
        else:
            print("  FAIL")
            print("  --- Grapa Output ---")
            print(grapa_out)
            print("  --- ripgrep Output ---")
            print(rg_out)
            print("  --- Diff ---")
            for line in difflib.unified_diff(grapa_out.splitlines(), rg_out.splitlines(), fromfile="grapa", tofile="ripgrep", lineterm=""):
                print(line)
        print()

if __name__ == "__main__":
    main() 