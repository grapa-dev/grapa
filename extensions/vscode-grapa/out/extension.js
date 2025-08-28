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
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = __importStar(require("vscode"));
const executor_1 = require("./executor");
const languageProvider_1 = require("./languageProvider");
function activate(context) {
    console.log('🎉 Grapa extension is now active!');
    console.log('Extension context:', context.extensionPath);
    console.log('Workspace folders:', vscode.workspace.workspaceFolders);
    const executor = new executor_1.GrapaExecutor(context);
    const languageProvider = new languageProvider_1.GrapaLanguageProvider();
    // Register basic commands
    const runFile = vscode.commands.registerCommand('grapa.runFile', () => {
        executor.runCurrentFile();
    });
    const runSelection = vscode.commands.registerCommand('grapa.runSelection', () => {
        executor.runSelection();
    });
    const runCommand = vscode.commands.registerCommand('grapa.runCommand', () => {
        executor.runCommand();
    });
    const debugFile = vscode.commands.registerCommand('grapa.debugFile', () => {
        executor.debugFile();
    });
    // Register interactive session management commands (Python-style)
    const startInteractive = vscode.commands.registerCommand('grapa.startInteractive', () => {
        executor.startInteractive();
    });
    const restartSession = vscode.commands.registerCommand('grapa.restartSession', () => {
        executor.restartSession();
    });
    const interruptSession = vscode.commands.registerCommand('grapa.interruptSession', () => {
        executor.interruptSession();
    });
    const startInteractiveTerminal = vscode.commands.registerCommand('grapa.startInteractiveTerminal', () => {
        executor.startInteractiveTerminal();
    });
    // Register language features
    const completionProvider = vscode.languages.registerCompletionItemProvider({ language: 'grapa' }, languageProvider, '.', '(', ' ');
    const hoverProvider = vscode.languages.registerHoverProvider({ language: 'grapa' }, languageProvider);
    const signatureProvider = vscode.languages.registerSignatureHelpProvider({ language: 'grapa' }, languageProvider, '(', ',');
    // Register auto-run on save if enabled
    const autoRunOnSave = vscode.workspace.onDidSaveTextDocument((document) => {
        if (document.languageId === 'grapa') {
            const config = vscode.workspace.getConfiguration('grapa');
            if (config.get('autoRunOnSave')) {
                executor.runFile(document.fileName);
            }
        }
    });
    // Add subscriptions to context
    context.subscriptions.push(runFile, runSelection, runCommand, debugFile, startInteractive, restartSession, interruptSession, startInteractiveTerminal, completionProvider, hoverProvider, signatureProvider, autoRunOnSave, 
    // Cleanup executor on deactivation
    { dispose: () => executor.dispose() });
}
function deactivate() { }
//# sourceMappingURL=extension.js.map