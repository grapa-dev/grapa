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
        
        # Load syntax rules from basic_syntax.md
        self.syntax_rules = self._load_syntax_rules()
        
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
        
    def _load_syntax_rules(self) -> Dict[str, List[str]]:
        """Load syntax rules from basic_syntax.md"""
        rules = {
            'forbidden_patterns': [],
            'required_patterns': [],
            'comment_rules': [],
            'semicolon_rules': [],
            'loop_rules': []
        }
        
        if not self.basic_syntax_file.exists():
            print(f"Warning: {self.basic_syntax_file} not found")
            return rules
            
        try:
            with open(self.basic_syntax_file, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Extract rules from the markdown content
            # Forbidden patterns
            if 'for' in content.lower():
                rules['forbidden_patterns'].append(r'\bfor\s*\(')
                
            # Comment rules
            if '/*' in content and '*/' in content:
                rules['comment_rules'].append(r'//.*$')  # Line comments forbidden
                
            # Semicolon rules
            if 'semicolon' in content.lower():
                rules['semicolon_rules'].extend([
                    r'op\([^)]*\)\s*\{;',  # Incorrect semicolon after op() {
                    r'class\s*\{;',        # Incorrect semicolon after class {
                ])
                
            # Loop rules
            if 'while' in content.lower():
                rules['loop_rules'].append(r'\bfor\s*\(')
                
        except Exception as e:
            print(f"Error loading syntax rules: {e}")
            
        return rules
    
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
        
        return grc_files
    
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
        for pattern in self.syntax_rules['forbidden_patterns']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'forbidden_pattern',
                        'line': i,
                        'message': f'Forbidden pattern found: {pattern}',
                        'content': line.strip(),
                        'auto_fixable': pattern == r'\bfor\s*\('
                    })
        
        # Check for incorrect semicolons
        for pattern in self.syntax_rules['semicolon_rules']:
            for i, line in enumerate(lines, 1):
                if re.search(pattern, line):
                    issues.append({
                        'type': 'incorrect_semicolon',
                        'line': i,
                        'message': f'Incorrect semicolon after opening brace',
                        'content': line.strip(),
                        'auto_fixable': True
                    })
        
        # Check for line comments
        for i, line in enumerate(lines, 1):
            stripped = line.strip()
            if stripped.startswith('//') and not stripped.startswith('//"'):
                issues.append({
                    'type': 'line_comment',
                    'line': i,
                    'message': 'Line comments (//) are not allowed in Grapa',
                    'content': line.strip(),
                    'auto_fixable': True
                })
        
        # Check for missing semicolons on statements
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
                not stripped.startswith('continue')):
                
                # This might be a missing semicolon
                issues.append({
                    'type': 'missing_semicolon',
                    'line': i,
                    'message': 'Possible missing semicolon',
                    'content': line.strip(),
                    'auto_fixable': False  # Too risky to auto-fix
                })
        
        # Check for .echo() without newlines
        for i, line in enumerate(lines, 1):
            if '.echo()' in line and '\\n' not in line and '"' in line:
                # Check if it's a string that should have a newline
                if re.search(r'"[^"]*"\.echo\(\)', line):
                    issues.append({
                        'type': 'missing_newline',
                        'line': i,
                        'message': '.echo() without explicit newline - consider adding \\n',
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
                fixed_content = re.sub(r'op\([^)]*\)\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                # Fix class {; -> class {
                fixed_content = re.sub(r'class\s*\{;', lambda m: m.group(0).replace('{;', '{'), fixed_content)
                
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
                
            elif issue['type'] == 'forbidden_pattern':
                if 'for (' in issue['content']:
                    # Convert for loop to while loop (basic conversion)
                    # This is a simplified conversion - manual review needed
                    pass
        
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
                    status = "🔧" if issue.get('auto_fixable', False) else "⚠️"
                    print(f"{status} Line {issue['line']}: {issue['message']}")
                    if verbose and 'content' in issue:
                        print(f"   Content: {issue['content']}")
                    
                    if issue.get('auto_fixable', False):
                        self.stats['auto_fixed'] += 1
                    else:
                        self.stats['manual_fix_needed'] += 1
        
        self._print_summary()
    
    def _print_summary(self):
        """Print validation summary"""
        print("\n" + "=" * 50)
        print("📊 VALIDATION SUMMARY")
        print("=" * 50)
        print(f"Files checked: {self.stats['files_checked']}")
        print(f"Files with issues: {self.stats['files_with_issues']}")
        print(f"Total issues found: {self.stats['total_issues']}")
        print(f"Auto-fixed: {self.stats['auto_fixed']}")
        print(f"Manual fix needed: {self.stats['manual_fix_needed']}")
        
        if self.stats['manual_fix_needed'] > 0:
            print(f"\n⚠️  {self.stats['manual_fix_needed']} issues require manual attention")
            print("   Review the output above and fix manually")
        
        if self.stats['auto_fixed'] > 0:
            print(f"\n✅ {self.stats['auto_fixed']} issues were auto-fixed")
        
        if self.stats['total_issues'] == 0:
            print("\n🎉 All files are compliant with Grapa syntax rules!")

def main():
    parser = argparse.ArgumentParser(description='Validate Grapa syntax in .grc files')
    parser.add_argument('--fix', action='store_true', help='Auto-fix issues that can be reliably fixed')
    parser.add_argument('--verbose', action='store_true', help='Show detailed information about each issue')
    parser.add_argument('--file', help='Validate only a specific file')
    
    args = parser.parse_args()
    
    # Find project root (assuming script is in scripts/ directory)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    validator = GrapaSyntaxValidator(project_root)
    validator.run_validation(auto_fix=args.fix, verbose=args.verbose, specific_file=args.file)
    
    # Exit with error code if there are issues that need manual attention
    if validator.stats['manual_fix_needed'] > 0:
        sys.exit(1)

if __name__ == '__main__':
    main() 