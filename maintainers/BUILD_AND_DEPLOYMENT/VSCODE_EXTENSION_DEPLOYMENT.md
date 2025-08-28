---
tags:
  - maintainer
  - deployment
  - vscode
  - extension
---

# VSCode Extension Deployment

This document covers the development, building, testing, and deployment of the Grapa VSCode extension.

## 🎯 **Overview**

The Grapa VSCode extension provides language support for Grapa programming language, including:
- Syntax highlighting for `.grc` files
- IntelliSense and autocomplete
- Code snippets (40+ templates)
- Interactive terminal and execution
- Error detection and validation

## 📁 **Extension Structure**

```
extensions/vscode-grapa/
├── src/
│   ├── extension.ts          # Main extension entry point
│   ├── executor.ts           # Grapa execution logic
│   └── languageProvider.ts   # Language features (completion, hover)
├── syntaxes/
│   └── grapa.tmLanguage.json # Syntax highlighting grammar
├── snippets/
│   └── grapa.json           # Code snippets
├── .github/workflows/
│   ├── build-and-test-extension.yml # CI/CD build workflow
│   └── publish.yml          # Publishing workflow
├── package.json             # Extension manifest
└── README.md               # User documentation
```

## 🛠️ **Development**

### **Prerequisites**
- Node.js 18+ and npm
- VSCode for testing
- Grapa executable installed

### **Local Development**
```bash
cd extensions/vscode-grapa
npm install
npm run compile
```

### **Testing**
1. **Open extension in VSCode**: `code .`
2. **Press F5** to launch extension development host
3. **Test features** in the new VSCode window
4. **Check output** in the Debug Console

## 🏗️ **Building**

### **Manual Build**
```bash
cd extensions/vscode-grapa
npm run compile
vsce package
```

### **Using Deployment Script**
```bash
# Build only
./scripts/packaging/deploy_vscode_extension.sh --build-only

# Build and publish
./scripts/packaging/deploy_vscode_extension.sh --publish
```

## 🚀 **Deployment**

### **Publishing Process**
1. **Build extension** using deployment script
2. **Manual upload** to Visual Studio Marketplace
3. **Verification** by Microsoft (1-3 business days)
4. **Publication** and availability to users

### **Publisher Information**
- **Publisher ID**: `chris-matichuk`
- **Extension Name**: `Grapa Programming Language`
- **Marketplace URL**: https://marketplace.visualstudio.com/

### **Deployment Script**
The deployment script (`scripts/packaging/deploy_vscode_extension.sh`) handles:
- Prerequisites validation
- Extension building and packaging
- GitHub Actions workflow triggering
- Error handling and reporting

## 🔄 **CI/CD Workflows**

### **Build and Test Workflow** (`build-and-test-extension.yml`)
- **Triggers**: Push to main/develop branches, pull requests
- **Actions**: Build, compile, package extension
- **Output**: Creates downloadable .vsix artifact

### **Publish Workflow** (`publish.yml`)
- **Triggers**: Manual trigger only (via deployment script)
- **Actions**: Build, package, publish to marketplace
- **Requirements**: `VSCE_PAT` GitHub secret

## 📋 **Configuration**

### **Extension Settings**
```json
{
  "grapa.executablePath": "./grapa",
  "grapa.debugMode": false,
  "grapa.autoRunOnSave": false,
  "grapa.preferInteractiveMode": true,
  "grapa.sessionTimeout": 30000,
  "grapa.autoEscapeLineFeeds": true,
  "grapa.showInteractiveOutput": true
}
```

### **GitHub Secrets**
- **`VSCE_PAT`**: Personal Access Token for Visual Studio Marketplace publishing

## 🐛 **Troubleshooting**

### **Common Issues**

#### **Extension Not Loading**
- Check VSCode developer console for errors
- Verify TypeScript compilation succeeded
- Check extension activation events

#### **Grapa Executable Not Found**
- Verify Grapa is installed and in PATH
- Check `grapa.executablePath` setting
- Test `grapa --version` in terminal

#### **Snippet Warnings**
- Check for unescaped `$` characters in snippets
- Verify snippet JSON syntax
- Remove problematic snippets if needed

#### **Interactive Mode Issues**
- Terminal read-only: Use system shell approach
- Session not starting: Check executable path
- Output not visible: Check output channel

### **Debug Mode**
Enable debug mode in extension settings to get detailed logging:
```json
{
  "grapa.debugMode": true
}
```

## 📚 **Documentation**

### **User Documentation**
- **README.md**: User-facing documentation with installation and usage
- **Marketplace listing**: Extension description and features
- **In-extension help**: Command descriptions and examples

### **Maintainer Documentation**
- **This file**: Deployment and development guide
- **Code comments**: Inline documentation in TypeScript files
- **GitHub workflows**: CI/CD configuration documentation

## 🔄 **Maintenance**

### **Version Updates**
1. **Update version** in `package.json`
2. **Update changelog** in README.md
3. **Test thoroughly** before publishing
4. **Deploy** using deployment script

### **Feature Additions**
1. **Add functionality** in TypeScript files
2. **Update snippets** if needed
3. **Add tests** for new features
4. **Update documentation**
5. **Test in development host**

### **Bug Fixes**
1. **Reproduce issue** in development host
2. **Fix in TypeScript** source
3. **Test fix** thoroughly
4. **Update version** and deploy

## 📞 **Support**

### **For Users**
- **GitHub Issues**: Report bugs and request features
- **Documentation**: Complete Grapa documentation
- **Community**: Grapa discussions

### **For Maintainers**
- **Development**: Use VSCode extension development host
- **Testing**: Test all features before publishing
- **Deployment**: Use deployment script for consistency

---

**Last Updated**: August 28, 2024  
**Status**: Extension published to Visual Studio Marketplace
