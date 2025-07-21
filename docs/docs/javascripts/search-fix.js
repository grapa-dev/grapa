// Fix for Material theme search highlighting bug
// Prevents "float" from being incorrectly replaced with "undefined" in search results

document.addEventListener('DOMContentLoaded', function() {
    // Function to fix undefined -> float replacements
    function fixUndefinedToFloat(element) {
        const text = element.innerHTML;
        let fixed = text;
        
        // Fix function signatures
        if (text.includes('undefined([bits [,extra]])')) {
            fixed = fixed.replace(/undefined\(\[bits \[,extra\]\]\)/g, 'float([bits [,extra]])');
        }
        
        // Fix method calls like "4.21".undefined()
        if (text.includes('".undefined()')) {
            fixed = fixed.replace(/"\.undefined\(\)/g, '".float()');
        }
        
        // Fix function names
        if (text.includes('setundefined(')) {
            fixed = fixed.replace(/setundefined\(/g, 'setfloat(');
        }
        
        // Fix type references
        if (text.includes('Converts to $undefined')) {
            fixed = fixed.replace(/Converts to \$undefined/g, 'Converts to $FLOAT');
        }
        
        if (text.includes('Converts to fixed undefined')) {
            fixed = fixed.replace(/Converts to fixed undefined/g, 'Converts to fixed float');
        }
        
        if (text.includes('Sets the default undefined type')) {
            fixed = fixed.replace(/Sets the default undefined type/g, 'Sets the default float type');
        }
        
        // Fix to undefined type references
        if (text.includes('to undefined, and the default')) {
            fixed = fixed.replace(/to undefined, and the default/g, 'to float, and the default');
        }
        
        if (text.includes('to fix, and the default')) {
            fixed = fixed.replace(/to fix, and the default/g, 'to fix, and the default');
        }
        
        // Only update if changes were made
        if (fixed !== text) {
            element.innerHTML = fixed;
        }
    }

    // Fix search highlighting after search results are loaded
    const observer = new MutationObserver(function(mutations) {
        mutations.forEach(function(mutation) {
            if (mutation.type === 'childList') {
                // Look for search result elements and fix any "undefined" that should be "float"
                const searchResults = document.querySelectorAll('.md-search-result__item');
                searchResults.forEach(function(result) {
                    fixUndefinedToFloat(result);
                });
            }
        });
    });

    // Start observing the search results container
    const searchContainer = document.querySelector('.md-search-result');
    if (searchContainer) {
        observer.observe(searchContainer, { childList: true, subtree: true });
    }

    // Also fix any existing search results on page load
    setTimeout(function() {
        // Fix search results
        const searchResults = document.querySelectorAll('.md-search-result__item');
        searchResults.forEach(function(result) {
            fixUndefinedToFloat(result);
        });

        // Fix main content area
        const mainContent = document.querySelector('.md-content__inner');
        if (mainContent) {
            fixUndefinedToFloat(mainContent);
        }
        
        // Fix any code blocks that might contain the issue
        const codeBlocks = document.querySelectorAll('code');
        codeBlocks.forEach(function(codeBlock) {
            fixUndefinedToFloat(codeBlock);
        });
        
        // Fix any pre blocks that might contain the issue
        const preBlocks = document.querySelectorAll('pre');
        preBlocks.forEach(function(preBlock) {
            fixUndefinedToFloat(preBlock);
        });
    }, 100);
    
    // Additional fix with longer delay to catch any late-loading content
    setTimeout(function() {
        // Fix search results again
        const searchResults = document.querySelectorAll('.md-search-result__item');
        searchResults.forEach(function(result) {
            fixUndefinedToFloat(result);
        });

        // Fix main content area again
        const mainContent = document.querySelector('.md-content__inner');
        if (mainContent) {
            fixUndefinedToFloat(mainContent);
        }
        
        // Fix any code blocks again
        const codeBlocks = document.querySelectorAll('code');
        codeBlocks.forEach(function(codeBlock) {
            fixUndefinedToFloat(codeBlock);
        });
        
        // Fix any pre blocks again
        const preBlocks = document.querySelectorAll('pre');
        preBlocks.forEach(function(preBlock) {
            fixUndefinedToFloat(preBlock);
        });
    }, 500);
}); 