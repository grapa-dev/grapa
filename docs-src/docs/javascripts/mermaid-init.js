// Auto-initialize Mermaid diagrams on page load with larger font and loose security for links
function initMermaid() {
  if (window.mermaid) {
    mermaid.initialize({
      startOnLoad: true,
      themeVariables: {
        fontSize: '22px',
        fontFamily: 'inherit',
      },
      securityLevel: 'loose',
    });
    // Add fallback click handlers for main section nodes
    setTimeout(() => {
      const clickableIds = [
        'B1', 'B2', 'B3', 'B4', 'B5', 'B6', 'B7'
      ];
      const links = [
        'cli_quickstart/',
        'api_reference/',
        'examples/',
        'use_cases/',
        'migrations/',
        'run/',
        'troubleshooting/'
      ];
      clickableIds.forEach((id, idx) => {
        const node = document.querySelector('.mermaid [id^="' + id + '-"]');
        if (node) {
          node.style.cursor = 'pointer';
          node.onclick = () => {
            window.location.href = links[idx];
          };
        }
      });
    }, 1000);
  }
}

if (window.mermaid) {
  initMermaid();
} else {
  document.addEventListener('DOMContentLoaded', initMermaid);
} 