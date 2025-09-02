module.exports = {
  root: true,
  parser: '@typescript-eslint/parser',
  parserOptions: {
    ecmaVersion: 2020,
    sourceType: 'module',
    project: './tsconfig.json',
  },
  plugins: ['@typescript-eslint'],
  extends: [
    'eslint:recommended',
  ],
  rules: {
    // Grapa-specific relaxed rules based on basic_syntax.md and operator.md
    'no-unused-vars': 'warn', // Relaxed for Grapa patterns
    'no-explicit-any': 'off', // Grapa uses dynamic typing extensively
    
    // VSCode extension specific
    'no-console': 'off', // Allow console.log for debugging
    'prefer-const': 'warn', // Relaxed for Grapa patterns
    'no-var': 'warn',
    
    // Grapa-specific allowances based on documented syntax
    'camelcase': 'off', // Grapa uses different naming conventions
    'prefer-template': 'off', // Grapa uses string interpolation and concatenation
    'no-plusplus': 'off', // Grapa uses ++= operator for array/list extension
    'no-bitwise': 'off', // Grapa supports bitwise operators: &, |, ^, ~, <<, >>
    
    // Grapa operator support (from operator.md)
    'no-multi-assign': 'off', // Grapa uses compound assignment like ++=, **=, etc.
    'operator-assignment': 'off', // Grapa has custom operators like .* (dot product)
    'no-sequences': 'off', // Grapa allows multiple operations in expressions
    
    // Grapa syntax patterns (from basic_syntax.md)
    'no-constant-condition': 'warn', // Grapa uses while(true) patterns
    'no-empty': 'warn', // Grapa allows empty blocks in some contexts
    
    // Grapa-specific naming and patterns
    'id-length': 'off', // Grapa uses short names like $sys, $thread, etc.
    'no-underscore-dangle': 'off', // Grapa uses underscores in numbers (3_14)
    'no-magic-numbers': 'off', // Grapa uses magic numbers extensively
  },
  env: {
    node: true,
    es6: true,
  },
  ignorePatterns: ['out/', 'node_modules/', '*.js'],
  // Grapa-specific globals and patterns
  globals: {
    // Grapa system globals
    'GrapaRuntime': 'readonly',
    'GrapaContext': 'readonly',
    '$sys': 'readonly',
    '$thread': 'readonly',
    '$global': 'readonly',
    '$local': 'readonly',
    '$this': 'readonly',
    
    // Grapa type globals
    '$INT': 'readonly',
    '$FLOAT': 'readonly',
    '$STR': 'readonly',
    '$BOOL': 'readonly',
    '$ARRAY': 'readonly',
    '$LIST': 'readonly',
    '$OBJ': 'readonly',
    '$VECTOR': 'readonly',
    '$TUPLE': 'readonly',
    '$RULE': 'readonly',
    '$ERR': 'readonly',
    '$TIME': 'readonly',
    '$RAW': 'readonly',
    '$XML': 'readonly',
    '$WIDGET': 'readonly',
  },
};
