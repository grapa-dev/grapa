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
exports.GrapaExecutor = void 0;
const vscode = __importStar(require("vscode"));
const path = __importStar(require("path"));
const fs = __importStar(require("fs"));
const child_process_1 = require("child_process");
class GrapaExecutor {
    constructor(context) {
        this.interactiveSession = null;
        this.sessionStatusBarItem = null;
        this.isInteractiveMode = false;
        this.outputChannel = null;
        this.context = context;
        this.createStatusBarItem();
    }
    createStatusBarItem() {
        this.sessionStatusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
        this.sessionStatusBarItem.text = '$(terminal) Grapa: Ready';
        this.sessionStatusBarItem.tooltip = 'Click to start interactive session';
        this.sessionStatusBarItem.command = 'grapa.startInteractive';
        this.sessionStatusBarItem.show();
    }
    updateStatusBar(status) {
        if (!this.sessionStatusBarItem)
            return;
        switch (status) {
            case 'Ready':
                this.sessionStatusBarItem.text = '$(terminal) Grapa: Ready';
                this.sessionStatusBarItem.tooltip = 'Click to start interactive session';
                this.sessionStatusBarItem.command = 'grapa.startInteractive';
                this.sessionStatusBarItem.backgroundColor = undefined;
                break;
            case 'Active':
                this.sessionStatusBarItem.text = '$(terminal) Grapa: Active';
                this.sessionStatusBarItem.tooltip = 'Click to restart session';
                this.sessionStatusBarItem.command = 'grapa.restartSession';
                this.sessionStatusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.prominentBackground');
                break;
            case 'Error':
                this.sessionStatusBarItem.text = '$(error) Grapa: Error';
                this.sessionStatusBarItem.tooltip = 'Click to restart session';
                this.sessionStatusBarItem.command = 'grapa.restartSession';
                this.sessionStatusBarItem.backgroundColor = new vscode.ThemeColor('statusBarItem.errorBackground');
                break;
        }
    }
    getExecutablePath() {
        const config = vscode.workspace.getConfiguration('grapa');
        let executablePath = config.get('executablePath', './grapa');
        // If relative path, resolve from workspace root
        if (!path.isAbsolute(executablePath)) {
            const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
            if (workspaceRoot) {
                executablePath = path.join(workspaceRoot, executablePath);
            }
        }
        return executablePath;
    }
    async findGrapaExecutable() {
        const config = vscode.workspace.getConfiguration('grapa');
        let executablePath = config.get('executablePath', '');
        console.log('Looking for Grapa executable. Configured path:', executablePath);
        // If user has configured a specific path, use it
        if (executablePath && executablePath.trim() !== '') {
            if (fs.existsSync(executablePath)) {
                console.log('Found executable at configured path:', executablePath);
                return executablePath;
            }
            else {
                console.log('Configured path does not exist:', executablePath);
            }
        }
        // Try to find grapa in system PATH (like other language extensions)
        try {
            const { execSync } = require('child_process');
            const whichResult = execSync('which grapa', { encoding: 'utf8' }).trim();
            if (whichResult && fs.existsSync(whichResult)) {
                console.log('Found Grapa in system PATH:', whichResult);
                return whichResult;
            }
        }
        catch (error) {
            console.log('Grapa not found in system PATH');
        }
        // Try to find grapa in common installation locations
        const commonSystemPaths = [
            '/usr/local/bin/grapa',
            '/usr/bin/grapa',
            '/opt/homebrew/bin/grapa', // Homebrew on Apple Silicon
            '/usr/local/opt/grapa/bin/grapa', // Homebrew on Intel
            'C:\\Program Files\\Grapa\\grapa.exe', // Windows
            'C:\\Program Files (x86)\\Grapa\\grapa.exe' // Windows 32-bit
        ];
        for (const systemPath of commonSystemPaths) {
            if (fs.existsSync(systemPath)) {
                console.log('Found Grapa at system location:', systemPath);
                return systemPath;
            }
        }
        // Fallback: Try to find grapa in workspace (for development)
        const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
        console.log('Workspace root:', workspaceRoot);
        if (workspaceRoot) {
            const workspacePaths = [
                path.join(workspaceRoot, 'grapa'),
                path.join(workspaceRoot, 'bin', 'grapa'),
                path.join(workspaceRoot, 'bin', 'grapa-mac-arm64'),
                path.join(workspaceRoot, 'bin', 'grapa-linux-amd64'),
                path.join(workspaceRoot, 'bin', 'grapa-win-amd64.exe'),
                path.join(workspaceRoot, 'bin', 'linux-amd64', 'grapa'),
                path.join(workspaceRoot, 'bin', 'linux-arm64', 'grapa'),
                path.join(workspaceRoot, 'bin', 'mac-arm64', 'grapa'),
                path.join(workspaceRoot, 'bin', 'win-amd64', 'grapa.exe')
            ];
            console.log('Checking workspace paths:', workspacePaths);
            for (const workspacePath of workspacePaths) {
                if (fs.existsSync(workspacePath)) {
                    console.log('Found executable in workspace:', workspacePath);
                    return workspacePath;
                }
            }
        }
        console.log('No Grapa executable found');
        return null;
    }
    async createTerminal(name) {
        // Close existing terminal with same name
        vscode.window.terminals.forEach(terminal => {
            if (terminal.name === name) {
                terminal.dispose();
            }
        });
        return vscode.window.createTerminal(name);
    }
    async showError(message) {
        vscode.window.showErrorMessage(`Grapa: ${message}`);
    }
    async showInfo(message) {
        vscode.window.showInformationMessage(`Grapa: ${message}`);
    }
    // Start interactive session (Python-style)
    async startInteractive() {
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        // Kill existing session if any
        if (this.interactiveSession) {
            this.interactiveSession.kill('SIGTERM');
        }
        try {
            console.log('Starting interactive session with:', executablePath);
            // Create output channel to show interactive session
            if (!this.outputChannel) {
                this.outputChannel = vscode.window.createOutputChannel('Grapa Interactive');
            }
            this.outputChannel.clear(); // Clear previous output
            this.outputChannel.show(true); // Show and focus the output channel
            this.outputChannel.appendLine('Starting Grapa Interactive Session...');
            this.outputChannel.appendLine('This session is for code selections only.');
            this.outputChannel.appendLine('For manual interaction, use "Grapa: Start Interactive Terminal" command.');
            this.outputChannel.appendLine('');
            // Create interactive session with proper stdio configuration
            this.interactiveSession = (0, child_process_1.spawn)(executablePath, ['-i', '--no-prompt'], {
                stdio: ['pipe', 'pipe', 'pipe'],
                // Ensure we can send commands to the session
                detached: false
            });
            console.log('Interactive session spawned, PID:', this.interactiveSession.pid);
            // Add output handling to show what's happening in the output channel
            if (this.interactiveSession.stdout) {
                this.interactiveSession.stdout.on('data', (data) => {
                    const output = data.toString();
                    console.log('Interactive session output:', output);
                    // Always append output to make sure it's visible
                    this.outputChannel?.append(output);
                });
            }
            if (this.interactiveSession.stderr) {
                this.interactiveSession.stderr.on('data', (data) => {
                    const error = data.toString();
                    console.log('Interactive session error:', error);
                    this.outputChannel?.appendLine(`ERROR: ${error}`);
                });
            }
            this.isInteractiveMode = true;
            this.updateStatusBar('Active');
            // Monitor the session
            this.interactiveSession.on('error', async (error) => {
                await this.showError(`Session error: ${error.message}`);
                this.updateStatusBar('Error');
                this.isInteractiveMode = false;
                this.outputChannel?.appendLine(`Session Error: ${error.message}`);
            });
            this.interactiveSession.on('exit', async (code) => {
                if (code !== 0) {
                    await this.showInfo('Interactive session ended');
                }
                this.updateStatusBar('Ready');
                this.isInteractiveMode = false;
                this.interactiveSession = null;
                this.outputChannel?.appendLine('Interactive session ended.');
            });
            // Wait a moment for the session to start
            setTimeout(async () => {
                await this.showInfo('Interactive session started. Check the "Grapa Interactive" output panel.');
                this.outputChannel?.appendLine('Session ready. You can now run code selections.');
            }, 500);
        }
        catch (error) {
            await this.showError(`Failed to start interactive session: ${error}`);
            this.updateStatusBar('Error');
        }
    }
    // Restart interactive session (Python-style)
    async restartSession() {
        if (this.interactiveSession) {
            this.interactiveSession.kill('SIGTERM');
            await this.showInfo('Session killed');
        }
        setTimeout(async () => {
            await this.startInteractive();
        }, 1000);
    }
    // Interrupt current command (Python-style)
    async interruptSession() {
        if (this.interactiveSession) {
            this.interactiveSession.kill('SIGINT');
            await this.showInfo('Command interrupted');
        }
        else {
            await this.showError('No active session to interrupt');
        }
    }
    // Run selection with proper mode detection
    async runSelection() {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            await this.showError('No active editor');
            return;
        }
        const selection = editor.selection;
        const code = editor.document.getText(selection);
        if (!code.trim()) {
            await this.showError('No text selected');
            return;
        }
        // Check if we should use interactive mode
        const config = vscode.workspace.getConfiguration('grapa');
        const preferInteractive = config.get('preferInteractiveMode', true);
        console.log('runSelection debug:', {
            preferInteractive,
            isInteractiveMode: this.isInteractiveMode,
            hasInteractiveSession: !!this.interactiveSession,
            code: code.substring(0, 50) + '...'
        });
        if (preferInteractive && this.isInteractiveMode && this.interactiveSession) {
            console.log('Using interactive session');
            // Run in interactive session (no escaping needed)
            await this.runInInteractive(code);
        }
        else {
            console.log('Using CLI mode');
            // Run as CLI command (escaping needed)
            await this.runAsCLI(code);
        }
    }
    // Run code in interactive session (no escaping needed)
    async runInInteractive(code) {
        if (!this.interactiveSession || !this.interactiveSession.stdin) {
            await this.showError('No active interactive session');
            return;
        }
        try {
            // Check if auto-escaping is enabled
            const config = vscode.workspace.getConfiguration('grapa');
            const autoEscape = config.get('autoEscapeLineFeeds', true);
            let processedCode = code;
            // Show the command being sent in the output channel
            this.outputChannel?.appendLine(`\n>>> ${code}`);
            // For multi-line code, enclose it in $[ and $] to process as a single block
            if (code.includes('\n')) {
                const wrappedCode = `$[\n${code}\n$]`;
                this.interactiveSession.stdin.write(wrappedCode + '\n');
            }
            else {
                // Single line - send directly
                this.interactiveSession.stdin.write(code + '\n');
            }
            // Make sure output channel is visible
            this.outputChannel?.show(true);
            await this.showInfo('Code sent to interactive session. Check the "Grapa Interactive" output panel.');
        }
        catch (error) {
            await this.showError(`Failed to send code: ${error}`);
        }
    }
    // Run code as CLI command (escaping needed)
    async runAsCLI(code) {
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        const terminal = await this.createTerminal('Grapa');
        // Remove line feeds from multi-line code to prevent shell "dquote>" mode
        let processedCode = code.replace(/\n/g, ' ');
        // For CLI: outer quotes must be double quotes, inner double quotes must be escaped
        // Also escape $ characters for non-Windows platforms
        let escapedCode = processedCode.replace(/"/g, '\\"');
        if (process.platform !== 'win32') {
            escapedCode = escapedCode.replace(/\$/g, '\\$');
        }
        terminal.sendText(`"${executablePath}" -c "${escapedCode}"`);
        terminal.show();
    }
    async runCurrentFile() {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            await this.showError('No active editor');
            return;
        }
        const document = editor.document;
        if (document.languageId !== 'grapa') {
            await this.showError('Current file is not a Grapa file');
            return;
        }
        const filePath = document.fileName;
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        const terminal = await this.createTerminal('Grapa');
        terminal.sendText(`"${executablePath}" "${filePath}"`);
        terminal.show();
    }
    async runCommand() {
        const command = await vscode.window.showInputBox({
            prompt: 'Enter Grapa command',
            placeHolder: 'e.g., "hello".echo()'
        });
        if (!command) {
            return;
        }
        // Check if we should use interactive mode
        const config = vscode.workspace.getConfiguration('grapa');
        const preferInteractive = config.get('preferInteractiveMode', false);
        if (preferInteractive && this.isInteractiveMode && this.interactiveSession) {
            // Run in interactive session (no escaping needed)
            await this.runInInteractive(command);
        }
        else {
            // Run as CLI command (escaping needed)
            await this.runAsCLI(command);
        }
    }
    async startInteractiveTerminal() {
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        console.log('Starting interactive terminal...');
        // Create a terminal using the system shell, then launch Grapa
        const terminal = vscode.window.createTerminal({
            name: 'Grapa Interactive'
        });
        // Wait a moment for the terminal to be ready, then send the command
        setTimeout(() => {
            terminal.sendText(`${executablePath} -i`);
        }, 1000);
        terminal.show();
        await this.showInfo('Interactive terminal opened. Grapa will start in a moment...');
    }
    async startContinuousInputLoop() {
        console.log('Starting continuous input loop...');
        // Create output channel if not exists
        if (!this.outputChannel) {
            this.outputChannel = vscode.window.createOutputChannel('Grapa Interactive');
        }
        this.outputChannel.show();
        this.outputChannel.appendLine('Interactive session ready. Type commands below:');
        this.outputChannel.appendLine('');
        // Create a status bar item to show the interactive mode is active
        const statusItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
        statusItem.text = '$(terminal) Grapa Interactive';
        statusItem.tooltip = 'Click to stop interactive session';
        statusItem.command = 'grapa.interruptSession';
        statusItem.show();
        try {
            while (true) {
                const command = await vscode.window.showInputBox({
                    prompt: 'Grapa Command',
                    placeHolder: 'Enter command (or type "exit" to quit)',
                    value: '',
                    ignoreFocusOut: true // Keep the input box open
                });
                if (!command || command.toLowerCase() === 'exit') {
                    console.log('Exiting continuous input loop');
                    break;
                }
                // Add command to output
                this.outputChannel?.appendLine(`>>> ${command}`);
                // Execute the command in the interactive session
                if (this.isInteractiveMode && this.interactiveSession) {
                    console.log('Executing command in interactive session:', command);
                    await this.runInInteractive(command);
                }
                else {
                    console.log('Falling back to CLI mode');
                    await this.runAsCLI(command);
                }
                this.outputChannel?.appendLine(''); // Add blank line for readability
            }
        }
        finally {
            // Clean up status bar item
            statusItem.dispose();
        }
        await this.showInfo('Interactive session ended.');
    }
    async startCommandInputLoop() {
        console.log('Starting command input loop...');
        while (true) {
            const command = await vscode.window.showInputBox({
                prompt: 'Enter Grapa command (or type "exit" to quit)',
                placeHolder: 'e.g., "hello".echo()',
                value: ''
            });
            if (!command || command.toLowerCase() === 'exit') {
                console.log('Exiting command loop');
                break;
            }
            console.log('Executing command:', command);
            // Execute the command in the interactive session
            if (this.isInteractiveMode && this.interactiveSession) {
                console.log('Using interactive session');
                await this.runInInteractive(command);
            }
            else {
                console.log('Falling back to CLI mode');
                await this.runAsCLI(command);
            }
        }
    }
    async debugFile() {
        const editor = vscode.window.activeTextEditor;
        if (!editor) {
            await this.showError('No active editor');
            return;
        }
        const document = editor.document;
        if (document.languageId !== 'grapa') {
            await this.showError('Current file is not a Grapa file');
            return;
        }
        const filePath = document.fileName;
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        const terminal = await this.createTerminal('Grapa Debug');
        terminal.sendText(`"${executablePath}" -d "${filePath}"`);
        terminal.show();
    }
    async runFile(filePath) {
        const executablePath = await this.findGrapaExecutable();
        if (!executablePath) {
            await this.showError('Grapa executable not found. Please configure the path in settings.');
            return;
        }
        const terminal = await this.createTerminal('Grapa');
        terminal.sendText(`"${executablePath}" "${filePath}"`);
        terminal.show();
    }
    // Cleanup on deactivation
    dispose() {
        if (this.interactiveSession) {
            this.interactiveSession.kill('SIGTERM');
        }
        if (this.sessionStatusBarItem) {
            this.sessionStatusBarItem.dispose();
        }
        if (this.outputChannel) {
            this.outputChannel.dispose();
        }
    }
}
exports.GrapaExecutor = GrapaExecutor;
//# sourceMappingURL=executor.js.map