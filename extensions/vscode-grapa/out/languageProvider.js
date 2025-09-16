"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.GrapaLanguageProvider = void 0;
const vscode = __importStar(require("vscode"));
class GrapaLanguageProvider {
    constructor() {
        // Grapa built-in functions and methods
        this.completions = [
            // Universal Object methods (available on all types)
            { label: 'echo()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Print the object to console' },
            { label: 'len()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get the length (works on strings, arrays, etc.)' },
            { label: 'str()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to string representation' },
            { label: 'int()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to integer' },
            { label: 'uint()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to unsigned integer' },
            { label: 'float()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to float' },
            { label: 'bool()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to boolean' },
            { label: 'raw()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get raw bytes' },
            { label: 'uraw()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get unsigned raw bytes' },
            { label: 'hex()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to hexadecimal string' },
            { label: 'uhex()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to unsigned hexadecimal string' },
            { label: 'bin()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to binary string' },
            { label: 'ubin()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to unsigned binary string' },
            { label: 'list()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to list' },
            { label: 'array()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to array' },
            { label: 'vector()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Convert to vector' },
            { label: 'bits()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get bit count' },
            { label: 'bytes()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get byte count' },
            { label: 'type()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get object type' },
            { label: 'describe()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Describe the object' },
            { label: 'identity()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get identity matrix' },
            { label: 'reverse()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Reverse the object' },
            { label: 'range()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Create range from number' },
            { label: 'keys()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Get object keys' },
            { label: 'iferr()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Handle errors with fallback' },
            { label: 'ifnull()', kind: vscode.CompletionItemKind.Method, detail: 'Universal method', documentation: 'Handle null values with default' },
            // String/Text methods
            { label: 'upper()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Convert to uppercase' },
            { label: 'lower()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Convert to lowercase' },
            { label: 'casefold()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Case-insensitive comparison' },
            { label: 'trim()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Remove whitespace from both ends' },
            { label: 'ltrim()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Remove whitespace from left' },
            { label: 'rtrim()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Remove whitespace from right' },
            { label: 'left()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Get left portion of string' },
            { label: 'right()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Get right portion of string' },
            { label: 'mid()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Get middle portion of string' },
            { label: 'split()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Split string into array' },
            { label: 'join()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Join array into string' },
            { label: 'replace()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Replace substring in string' },
            { label: 'grep()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Search for pattern in string' },
            { label: 'match()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Check if string matches pattern' },
            { label: 'findall()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Find all pattern matches' },
            { label: 'interpolate()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Interpolate variables in string' },
            { label: 'levenshtein()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Calculate Levenshtein distance' },
            { label: 'jarowinkler()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Calculate Jaro-Winkler similarity' },
            { label: 'cosinesimilarity()', kind: vscode.CompletionItemKind.Method, detail: 'Text method', documentation: 'Calculate cosine similarity' },
            // Array/List methods
            { label: 'get()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Get element at index' },
            { label: 'set()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Set element at index' },
            { label: 'add()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Add element to array' },
            { label: 'remove()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Remove element from array' },
            { label: 'clear()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Clear all elements' },
            { label: 'sort()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Sort array elements' },
            { label: 'argsort()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Get sorted indices' },
            { label: 'unique()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Get unique elements' },
            { label: 'group()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Group elements by function' },
            { label: 'map()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Apply function to each element' },
            { label: 'filter()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Filter elements by function' },
            { label: 'reduce()', kind: vscode.CompletionItemKind.Method, detail: 'Array method', documentation: 'Reduce array with function' },
            // Math methods (for numeric types)
            { label: 'abs()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Absolute value' },
            { label: 'log()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Natural logarithm' },
            { label: 'ln()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Natural logarithm' },
            { label: 'sin()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Sine function' },
            { label: 'cos()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Cosine function' },
            { label: 'tan()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Tangent function' },
            { label: 'asin()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Arcsine function' },
            { label: 'acos()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Arccosine function' },
            { label: 'atan()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Arctangent function' },
            { label: 'random()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Generate random number' },
            { label: 'modpow()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Modular exponentiation' },
            { label: 'modinv()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Modular multiplicative inverse' },
            { label: 'genprime()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Generate prime number' },
            { label: 'isprime()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Check if number is prime' },
            { label: 'gcd()', kind: vscode.CompletionItemKind.Method, detail: 'Math method', documentation: 'Greatest common divisor' },
            // File methods
            { label: 'read()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Read file contents' },
            { label: 'write()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Write content to file' },
            { label: 'append()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Append content to file' },
            { label: 'exists()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Check if file exists' },
            { label: 'delete()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Delete file' },
            { label: 'copy()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Copy file to destination' },
            { label: 'move()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Move file to destination' },
            { label: 'cd()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Change directory' },
            { label: 'ls()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'List directory contents' },
            { label: 'mkdir()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Create directory' },
            { label: 'rmdir()', kind: vscode.CompletionItemKind.Method, detail: 'File method', documentation: 'Remove directory' },
            // Database methods
            { label: 'table()', kind: vscode.CompletionItemKind.Method, detail: 'Database method', documentation: 'Access database table' },
            { label: 'query()', kind: vscode.CompletionItemKind.Method, detail: 'Database method', documentation: 'Execute database query' },
            { label: 'insert()', kind: vscode.CompletionItemKind.Method, detail: 'Database method', documentation: 'Insert record into table' },
            { label: 'update()', kind: vscode.CompletionItemKind.Method, detail: 'Database method', documentation: 'Update records in table' },
            { label: 'delete()', kind: vscode.CompletionItemKind.Method, detail: 'Database method', documentation: 'Delete records from table' },
            // Cryptographic methods
            { label: 'genkeys()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Generate cryptographic keys' },
            { label: 'sign()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Sign data with private key' },
            { label: 'verify()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Verify signature with public key' },
            { label: 'verifyrecover()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Verify and recover original data' },
            { label: 'encode()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Encode data (hash, encrypt, etc.)' },
            { label: 'decode()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Decode data' },
            { label: 'secret()', kind: vscode.CompletionItemKind.Method, detail: 'Crypto method', documentation: 'Generate shared secret for key exchange' },
            // Control flow
            { label: 'if', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'Conditional statement' },
            { label: 'else', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'Else clause' },
            { label: 'while', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'While loop' },
            { label: 'for', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'For loop' },
            { label: 'break', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'Break from loop' },
            { label: 'continue', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'Continue to next iteration' },
            { label: 'return', kind: vscode.CompletionItemKind.Keyword, detail: 'Control flow', documentation: 'Return from function' },
            // Error handling
            { label: 'iferr()', kind: vscode.CompletionItemKind.Method, detail: 'Error handling', documentation: 'Handle errors with fallback' },
            { label: 'ifnull()', kind: vscode.CompletionItemKind.Method, detail: 'Error handling', documentation: 'Handle null values with fallback' },
            // Variables and types
            { label: '$STR', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'String type' },
            { label: '$INT', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Integer type' },
            { label: '$FLOAT', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Float type' },
            { label: '$GOBJ', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'List/Array type' },
            { label: '$JSON', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'JSON object type' },
            { label: '$file', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'File type' },
            { label: '$sys', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'System type' },
            { label: '$thread', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Thread type' },
            { label: '$net', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Network type' },
            { label: '$math', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Math type' },
            { label: '$editor', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Editor type' },
            { label: '$TABLE', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Database table type' },
            { label: '$RSA', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'RSA cryptographic type' },
            { label: '$EC', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Elliptic Curve cryptographic type' },
            { label: '$DH', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Diffie-Hellman cryptographic type' },
            { label: '$RPK', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'Raw Public Key cryptographic type' },
            { label: '$BLS', kind: vscode.CompletionItemKind.Class, detail: 'Data type', documentation: 'BLS12-381 cryptographic type' },
            // System variables
            { label: '$ARGV', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Command line arguments array' },
            { label: '$CLIARGV', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'CLI arguments array' },
            { label: '$BIN', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Binary directory path' },
            { label: '$NAME', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Executable name' },
            { label: '$WORK', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Working directory path' },
            { label: '$HOME', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Home directory path' },
            { label: '$TEMP', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Temporary directory path' },
            { label: '$LIB', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Library directory path' },
            { label: '$VERSION', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Grapa version' },
            { label: '$PLATFORM', kind: vscode.CompletionItemKind.Variable, detail: 'System variable', documentation: 'Platform identifier' }
        ];
    }
    provideCompletionItems(document, position, token, context) {
        const items = this.completions.map(completion => {
            const item = new vscode.CompletionItem(completion.label, completion.kind);
            item.detail = completion.detail;
            item.documentation = new vscode.MarkdownString(completion.documentation);
            return item;
        });
        return items;
    }
    provideHover(document, position, token) {
        const range = document.getWordRangeAtPosition(position);
        if (!range) {
            return null;
        }
        const word = document.getText(range);
        const completion = this.completions.find(c => c.label.startsWith(word));
        if (completion) {
            const hover = new vscode.Hover([
                `**${completion.label}**`,
                `*${completion.detail}*`,
                completion.documentation
            ]);
            return hover;
        }
        return null;
    }
    provideSignatureHelp(document, position, token, context) {
        // Basic signature help for common methods
        const signatures = [
            new vscode.SignatureInformation('echo()', new vscode.MarkdownString('Print the string to console')),
            new vscode.SignatureInformation('len()', new vscode.MarkdownString('Get the length of the string')),
            new vscode.SignatureInformation('get(index)', new vscode.MarkdownString('Get element at specified index')),
            new vscode.SignatureInformation('set(index, value)', new vscode.MarkdownString('Set element at specified index')),
            new vscode.SignatureInformation('sign(data, digest?, padding?)', new vscode.MarkdownString('Sign data with private key. Optional digest and padding parameters.')),
            new vscode.SignatureInformation('verify(signature, data, digest?, padding?)', new vscode.MarkdownString('Verify signature with public key. Optional digest and padding parameters.'))
        ];
        const signatureHelp = new vscode.SignatureHelp();
        signatureHelp.signatures = signatures;
        signatureHelp.activeSignature = 0;
        signatureHelp.activeParameter = 0;
        return signatureHelp;
    }
}
exports.GrapaLanguageProvider = GrapaLanguageProvider;
//# sourceMappingURL=languageProvider.js.map