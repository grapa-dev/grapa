// Fix for Material theme search highlighting bug
// Prevents "float" from being incorrectly replaced with "undefined" in search results

document.addEventListener('DOMContentLoaded', function() {
    // Fix search highlighting after search results are loaded
    const observer = new MutationObserver(function(mutations) {
        mutations.forEach(function(mutation) {
            if (mutation.type === 'childList') {
                // Look for search result elements and fix any "undefined" that should be "float"
                const searchResults = document.querySelectorAll('.md-search-result__item');
                searchResults.forEach(function(result) {
                    const text = result.innerHTML;
                    if (text.includes('undefined([bits [,extra]])')) {
                        result.innerHTML = text.replace(/undefined\(\[bits \[,extra\]\]\)/g, 'float([bits [,extra]])');
                    }
                    if (text.includes('Converts to $undefined')) {
                        result.innerHTML = text.replace(/Converts to \$undefined/g, 'Converts to $FLOAT');
                    }
                    if (text.includes('Converts to fixed undefined')) {
                        result.innerHTML = text.replace(/Converts to fixed undefined/g, 'Converts to fixed float');
                    }
                    if (text.includes('Sets the default undefined type')) {
                        result.innerHTML = text.replace(/Sets the default undefined type/g, 'Sets the default float type');
                    }
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
        const searchResults = document.querySelectorAll('.md-search-result__item');
        searchResults.forEach(function(result) {
            const text = result.innerHTML;
            if (text.includes('undefined([bits [,extra]])')) {
                result.innerHTML = text.replace(/undefined\(\[bits \[,extra\]\]\)/g, 'float([bits [,extra]])');
            }
            if (text.includes('Converts to $undefined')) {
                result.innerHTML = text.replace(/Converts to \$undefined/g, 'Converts to $FLOAT');
            }
            if (text.includes('Converts to fixed undefined')) {
                result.innerHTML = text.replace(/Converts to fixed undefined/g, 'Converts to fixed float');
            }
            if (text.includes('Sets the default undefined type')) {
                result.innerHTML = text.replace(/Sets the default undefined type/g, 'Sets the default float type');
            }
        });
    }, 100);
}); 