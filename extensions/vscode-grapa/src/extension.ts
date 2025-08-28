import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { GrapaExecutor } from './executor';
import { GrapaLanguageProvider } from './languageProvider';

export function activate(context: vscode.ExtensionContext) {
    console.log('🎉 Grapa extension is now active!');
    console.log('Extension context:', context.extensionPath);
    console.log('Workspace folders:', vscode.workspace.workspaceFolders);

    const executor = new GrapaExecutor(context);
    const languageProvider = new GrapaLanguageProvider();

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

    const debugPathDetection = vscode.commands.registerCommand('grapa.debugPathDetection', () => {
        executor.debugPathDetection();
    });

    // Register language features
    const completionProvider = vscode.languages.registerCompletionItemProvider(
        { language: 'grapa' },
        languageProvider,
        '.', '(', ' '
    );

    const hoverProvider = vscode.languages.registerHoverProvider(
        { language: 'grapa' },
        languageProvider
    );

    const signatureProvider = vscode.languages.registerSignatureHelpProvider(
        { language: 'grapa' },
        languageProvider,
        '(', ','
    );

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
    context.subscriptions.push(
        runFile,
        runSelection,
        runCommand,
        debugFile,
        startInteractive,
        restartSession,
        interruptSession,
        startInteractiveTerminal,
        debugPathDetection,
        completionProvider,
        hoverProvider,
        signatureProvider,
        autoRunOnSave,
        // Cleanup executor on deactivation
        { dispose: () => executor.dispose() }
    );
}

export function deactivate() {}
