#!/usr/bin/env python3
"""
Grapa Syntax Validator and Auto-Fixer

This script validates all .grc files against Grapa's basic_syntax.md rules
and can automatically fix common issues.

Usage:
    python3 scripts/validate_grapa_syntax.py [--fix] [--verbose] [--file <file>]
    
Options:
    --fix      Auto-fix issues that can be reliably fixed
    --verbose  Show detailed information about each issue
    --file     Validate only a specific file
"""

import os
import sys
import re
import argparse
from pathlib import Path
from typing import List, Dict, Tuple, Set

class GrapaSyntaxValidator:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.test_dir = self.project_root / "test"
        self.docs_dir = self.project_root / "docs-src" / "docs"
        self.basic_syntax_file = self.docs_dir / "syntax" / "basic_syntax.md"
        
        # Statistics
        self.stats = {
            'files_checked': 0,
            'files_with_issues': 0,
            'total_issues': 0,
            'auto_fixed': 0,
            'manual_fix_needed': 0
        }
        
        # Issues found
        self.issues = []
        
        # Grapa syntax patterns based on basic_syntax.md and $grapa.grc
        self.syntax_patterns = {
            # Forbidden patterns
            'forbidden': [
                r'\bfor\s*\(',  # for loops are not supported
                r'\btry\s*\{',  # try/catch is not supported, use .iferr() instead
                r'\bcatch\s*\(',  # try/catch is not supported, use .iferr() instead
            ],
            
            # Incorrect method call syntax (should be object.method(), not .method(object))
            'incorrect_method_calls': [
                r'\.print\s*\([^)]*\)',  # .print("string") -> "string".print()
                r'\.len\s*\([^)]*\)',    # .len(array) -> array.len()
                r'\.size\s*\([^)]*\)',   # .size() is not supported, use .len()
                r'\.keys\s*\([^)]*\)',   # .keys() is not supported on $GOBJ
            ],
            
            # Line comments (not allowed in Grapa)
            'line_comments': [
                r'^\s*//.*$',  # // comment at start of line
                r'[^"]//[^/].*$',  # // comment in middle of line (but not in strings)
            ],
            
            # Incorrect semicolons
            'incorrect_semicolons': [
                r'op\s*\([^)]*\)\s*\{;',  # op() {; -> op() {
                r'class\s*\{;',           # class {; -> class {
                r'if\s*\([^)]*\)\s*\{;',  # if () {; -> if () {
                r'while\s*\([^)]*\)\s*\{;', # while () {; -> while () {
                r'else\s*\{;',            # else {; -> else {
            ],
            
            # Missing quotes around string literals in method calls
            'missing_quotes': [
                r'\.print\s*\([^"]+\)',  # .print(string) -> .print("string")
            ],
            
            # Missing newlines in echo calls (only for simple strings without \n)
            'missing_newlines': [
                r'"[^"]*"\.echo\s*\(\s*\)',  # "string".echo() without \n
            ],
            
            # Incorrect echo syntax (should be "string".echo(), not .echo("string"))
            'incorrect_echo_syntax': [
                r'\.echo\s*\([^)]*\)',  # .echo("string") -> "string".echo()
            ]
        }
        
    def find_grc_files(self, specific_file: str = None) -> List[Path]:
        """Find all .grc files to validate"""
        if specific_file:
            file_path = Path(specific_file)
            if file_path.exists() and file_path.suffix == '.grc':
                return [file_path]
            else:
                print(f"Error: {specific_file} not found or not a .grc file")
                return []
        
        grc_files = []
        for root, dirs, files in os.walk(self.test_dir):
            for file in files:
                if file.endswith('.grc'):
                    grc_files.append(Path(root) / file)
        
        return sorted(grc_files)
    
    def validate_file(self, file_path: Path, auto_fix: bool = False) -> List[Dict]:
        """Validate a single .grc file"""
        issues = []
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                lines = content.split('\n')
        except Exception as e:
            issues.append({
                'type': 'error',
                'line': 0,
                'message': f'Could not read file: {e}',
                'auto_fixable': False
            })
            return issues
        
        # Check for forbidden patterns
        for pattern in self.syntax_patterns['forbidden']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'forbidden_pattern',
                        'line': i,
                        'message': f'Forbidden pattern found: {pattern}',
                        'content': line.strip(),
                        'auto_fixable': False  # Manual fix needed
                    })
        
        # Check for incorrect method call syntax
        for pattern in self.syntax_patterns['incorrect_method_calls']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'incorrect_method_call',
                        'line': i,
                        'message': f'Incorrect method call syntax: {pattern}',
                        'content': line.strip(),
                        'auto_fixable': False  # Manual fix needed
                    })
        
        # Check for incorrect echo syntax (most critical) - .echo("string") is wrong
        for i, line in enumerate(lines, 1):
            # Look for .echo("string") pattern
            if re.search(r'\.echo\s*\([^)]*\)', line):
                # But exclude cases where it's already correct like "string".echo() or (expr).echo()
                if not (re.search(r'"[^"]*"\.echo\s*\(\s*\)', line) or 
                       re.search(r'\([^)]*\)\.echo\s*\(\s*\)', line)):
                    issues.append({
                        'type': 'incorrect_echo_syntax',
                        'line': i,
                        'message': 'Incorrect echo syntax: use "string".echo() not .echo("string")',
                        'content': line.strip(),
                        'auto_fixable': False  # Manual fix needed
                    })
        
        # Check for line comments (improved - excludes // within quoted strings)
        for i, line in enumerate(lines, 1):
            # Skip lines that start with // (these are definitely comments)
            stripped = line.strip()
            if stripped.startswith('//'):
                issues.append({
                    'type': 'line_comment',
                    'line': i,
                    'message': 'Line comments (//) are not allowed in Grapa',
                    'content': line.strip(),
                    'auto_fixable': True
                })
                continue
            
            # Check for // in the middle of lines, but exclude those within quoted strings
            if '//' in line:
                # Simple state machine to track if we're inside a quoted string
                in_string = False
                string_char = None  # Track which quote character we're in
                j = 0
                while j < len(line):
                    char = line[j]
                    
                    # Handle escaped quotes
                    if char == '\\' and j + 1 < len(line):
                        j += 2  # Skip the escape and the next character
                        continue
                    
                    # Handle quote characters
                    if char in ['"', "'"]:
                        if not in_string:
                            in_string = True
                            string_char = char
                        elif string_char == char:
                            in_string = False
                            string_char = None
                    
                    # Check for // outside of strings
                    elif char == '/' and j + 1 < len(line) and line[j + 1] == '/' and not in_string:
                        issues.append({
                            'type': 'line_comment',
                            'line': i,
                            'message': 'Line comments (//) are not allowed in Grapa',
                            'content': line.strip(),
                            'auto_fixable': True
                        })
                        break
                    
                    j += 1
        
        # Check for incorrect semicolons after opening braces
        for pattern in self.syntax_patterns['incorrect_semicolons']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'incorrect_semicolon',
                        'line': i,
                        'message': 'Incorrect semicolon after opening brace',
                        'content': line.strip(),
                        'auto_fixable': True
                    })
        
        # Check for missing quotes around string literals in method calls
        for pattern in self.syntax_patterns['missing_quotes']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'missing_quotes',
                        'line': i,
                        'message': 'Missing quotes around string literal',
                        'content': line.strip(),
                        'auto_fixable': False  # Manual fix needed
                    })
        
        # Check for missing newlines in echo calls (only for simple strings without \n)
        for i, line in enumerate(lines, 1):
            # Look for "string".echo() without \n in the string
            if re.search(r'"[^"]*"\.echo\s*\(\s*\)', line):
                # Extract the string content
                match = re.search(r'"([^"]*)"\.echo\s*\(\s*\)', line)
                if match and '\\n' not in match.group(1):
                    issues.append({
                        'type': 'missing_newline',
                        'line': i,
                        'message': '.echo() without explicit newline - consider adding \\n',
                        'content': line.strip(),
                        'auto_fixable': False  # Manual fix needed
                    })
        
        # Check for missing semicolons on statements (basic check)
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            if (stripped and 
                not stripped.startswith('/*') and 
                not stripped.endswith('*/') and
                not stripped.startswith('//') and
                not stripped.endswith('{') and
                not stripped.endswith('};') and
                not stripped.endswith(';') and
                not stripped.startswith('"') and
                not stripped.startswith('(') and
                not stripped.startswith(')') and
                not stripped.startswith('}') and
                not stripped.startswith('if') and
                not stripped.startswith('while') and
                not stripped.startswith('else') and
                not stripped.startswith('op') and
                not stripped.startswith('class') and
                not stripped.startswith('=') and
                not stripped.startswith('return') and
                not stripped.startswith('break') and
                not stripped.startswith('continue') and
                not stripped.startswith('case') and
                not stripped.startswith('default') and
                not stripped.startswith('switch') and
                not stripped.startswith('exit') and
                not stripped.startswith('include') and
                not stripped.startswith('reset')):
                
                # This might be a missing semicolon
                issues.append({
                    'type': 'missing_semicolon',
                    'line': i,
                    'message': 'Possible missing semicolon',
                    'content': line.strip(),
                    'auto_fixable': False  # Too risky to auto-fix
                })
        
        # Auto-fix if requested
        if auto_fix and issues:
            fixed_content = self._auto_fix_issues(content, issues)
            if fixed_content != content:
                try:
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(fixed_content)
                    print(f"✓ Auto-fixed {file_path}")
                except Exception as e:
                    print(f"✗ Could not write fixed file {file_path}: {e}")
        
        return issues
    
    def _auto_fix_issues(self, content: str, issues: List[Dict]) -> str:
        """Auto-fix issues that can be reliably fixed"""
        fixed_content = content
        
        for issue in issues:
            if not issue.get('auto_fixable', False):
                continue
                
            if issue['type'] == 'incorrect_semicolon':
                # Fix op() {; -> op() {
                fixed_content = re.sub(r'op\s*\([^)]*\)\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                # Fix class {; -> class {
                fixed_content = re.sub(r'class\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                # Fix if () {; -> if () {
                fixed_content = re.sub(r'if\s*\([^)]*\)\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                # Fix while () {; -> while () {
                fixed_content = re.sub(r'while\s*\([^)]*\)\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                # Fix else {; -> else {
                fixed_content = re.sub(r'else\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                
            elif issue['type'] == 'line_comment':
                # Convert // comments to /* */ comments
                lines = fixed_content.split('\n')
                line_num = issue['line'] - 1
                if line_num < len(lines):
                    comment = lines[line_num].strip()
                    if comment.startswith('//'):
                        new_comment = comment.replace('//', '/* ', 1) + ' */'
                        lines[line_num] = lines[line_num].replace(comment, new_comment)
                fixed_content = '\n'.join(lines)
        
        return fixed_content
    
    def run_validation(self, auto_fix: bool = False, verbose: bool = False, specific_file: str = None):
        """Run the complete validation process"""
        print("🔍 Grapa Syntax Validator")
        print("=" * 50)
        
        grc_files = self.find_grc_files(specific_file)
        if not grc_files:
            print("No .grc files found to validate")
            return
        
        print(f"Found {len(grc_files)} .grc files to validate")
        print()
        
        for file_path in grc_files:
            if verbose:
                print(f"Checking {file_path}...")
            
            issues = self.validate_file(file_path, auto_fix)
            self.stats['files_checked'] += 1
            
            if issues:
                self.stats['files_with_issues'] += 1
                self.stats['total_issues'] += len(issues)
                
                print(f"\n📁 {file_path}")
                print("-" * len(str(file_path)))
                
                for issue in issues:
                    if issue.get('auto_fixable', False):
                        self.stats['auto_fixed'] += 1
                        status = "🔧 AUTO-FIXED"
                    else:
                        self.stats['manual_fix_needed'] += 1
                        status = "⚠️  MANUAL FIX NEEDED"
                    
                    print(f"  Line {issue['line']}: {status}")
                    print(f"    {issue['message']}")
                    if 'content' in issue:
                        print(f"    Content: {issue['content']}")
                    print()
        
        self._print_summary()
    
    def _print_summary(self):
        """Print validation summary"""
        print("=" * 50)
        print("📊 VALIDATION SUMMARY")
        print("=" * 50)
        print(f"Files checked: {self.stats['files_checked']}")
        print(f"Files with issues: {self.stats['files_with_issues']}")
        print(f"Total issues found: {self.stats['total_issues']}")
        print(f"Auto-fixed: {self.stats['auto_fixed']}")
        print(f"Manual fix needed: {self.stats['manual_fix_needed']}")
        
        if self.stats['files_with_issues'] == 0:
            print("\n🎉 All files are compliant with Grapa syntax rules!")
        else:
            print(f"\n⚠️  {self.stats['files_with_issues']} files need attention")

def main():
    parser = argparse.ArgumentParser(description='Validate Grapa syntax in .grc files')
    parser.add_argument('--fix', action='store_true', help='Auto-fix issues that can be reliably fixed')
    parser.add_argument('--verbose', action='store_true', help='Show detailed information about each issue')
    parser.add_argument('--file', type=str, help='Validate only a specific file')
    
    args = parser.parse_args()
    
    # Get project root (assuming script is in scripts/ directory)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    validator = GrapaSyntaxValidator(str(project_root))
    validator.run_validation(auto_fix=args.fix, verbose=args.verbose, specific_file=args.file)

if __name__ == '__main__':
    main() 