import pytest
from hypothesis import given, strategies as st
import grapapy
import re

xy = grapapy.grapa()

def safe_eval(expr, vars=None):
    try:
        return xy.eval(expr, vars or {})
    except Exception as e:
        return str(e)

def is_valid_output(result):
    # Should be a list, or "$ERR" for error
    return isinstance(result, list) or result == "$ERR"

def no_null_for_empty(result):
    # No null for empty string in output
    if isinstance(result, list):
        return all(x != None for x in result)
    return True

@given(
    st.text(min_size=0, max_size=100),
    st.text(min_size=1, max_size=10),
    st.sampled_from(["", "o", "i", "v", "a", "N", "d", "oj", "b"])
)
def test_property_based_grep(input_text, pattern, options):
    # Property: No crash, valid output, no null for empty string
    try:
        result = safe_eval(f'"""{input_text}""".grep(r"{pattern}", "{options}")')
        assert is_valid_output(result)
        assert no_null_for_empty(result)
    except Exception as e:
        # Should never crash
        assert False, f"Crash: {e}"

def test_round_trip_idempotence():
    # Output of grep fed back into grep should be unchanged
    input_text = "a\nb\nc\n"
    result1 = xy.eval('input.grep(".", "o")', {"input": input_text})
    result2 = xy.eval('input.grep(".", "o")', {"input": "\n".join(result1)})
    assert result1 == result2

def test_large_scale_stress():
    # Very large input, long lines, many threads
    big_input = ("abc123\n" * 10000) + ("x" * 10000) + "\n"
    result = xy.eval('input.grep("abc", "o", "", "", "", 8)', {"input": big_input})
    assert isinstance(result, list)
    assert all("abc" in x for x in result)

def test_unicode_locale_edge_cases():
    # Rare scripts, mixed normalization, RTL, Turkish I, emoji ZWJ
    cases = [
        "",  # Thaana
        "",  # Cherokee
        "İstanbul",  # Turkish dotted I
        "Σίσυφος",    # Greek sigma
        "مرحبا بالعالم",  # Arabic (RTL)
        "👨‍👩‍👧‍👦",  # Emoji ZWJ
        "e01",  # e + combining acute
        "e",    # e + modifier letter apostrophe
    ]
    for text in cases:
        result = xy.eval('input.grep(".", "o")', {"input": text})
        assert isinstance(result, list)
        assert all(isinstance(x, str) for x in result)

def test_malformed_regex_error():
    # All invalid patterns should return "$ERR"
    bad_patterns = ["[", "(*)", "(?P<bad>", "\\K", "(?R)", "[a-z"]
    for pat in bad_patterns:
        result = safe_eval(f'"abc".grep(r"{pat}")')
        assert result == "$ERR"

def test_output_format_robustness():
    # Deeply nested, special chars, null/empty combos
    input_text = "\0\n\n\n\0"
    result = xy.eval('input.grep("^", "o")', {"input": input_text})
    assert isinstance(result, list)
    assert all(x == "" or isinstance(x, str) for x in result)
    # Array with empty string and null
    result2 = xy.eval('[1, "", 2]')
    assert result2 == [1, "", 2]

def test_doc_examples():
    # Run all code examples from documentation (sampled)
    assert xy.eval('"Hello world".grep("world")') == ["Hello world"]
    assert xy.eval('"Hello world".grep("world", "i")') == ["Hello world"]
    assert xy.eval('"Hello world".grep(r"\\w+", "o")') == ["Hello", "world"]
    assert xy.eval('"a\nb\n".grep("^", "o")') == ["", "a", "", "b", ""]
    assert xy.eval('[1, "", 2]') == [1, "", 2] 