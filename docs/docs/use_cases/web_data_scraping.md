# Web/Data Scraping & Automation

> **Best Practices:**
> - Use `$net().get(url)` for HTTP requests; always check `.get("status")` for HTTP status.
> - Parse HTML/XML with `$XML().parse(html)` or `.html()`.
> - Use `.select()` or `.findall()` to extract elements or attributes.
> - Rate limit requests with `$sys().sleep(ms)` to avoid overloading servers.
> - Use public, stable endpoints (e.g., `https://httpbin.org/html`, `https://example.com`) in examples.
> - Always check for `$ERR` when accessing attributes or elements that may not exist.
> - See [Advanced Extraction Patterns](../obj/document.md) for more on `.findall()` and complex queries.

## Example: Web Scraper with Rate Limiting
```grapa
/* Define a function to scrape multiple pages with rate limiting */
scrape_pages = op(urls, delay_ms) {
    /* Map over each URL, with index for delay control */
    results = urls.map(op(url, index) {
        /* Add delay between requests except for the first */
        if (index > 0) {
            $sys().sleep(delay_ms);
        };
        
        try {
            /* Perform HTTP GET request */
            response = $net().get(url);
            /* Check for successful response */
            if (response.get("status") == 200) {
                {
                    "url": url,
                    "success": true,
                    "content": response.get("body"),
                    "size": response.get("body").len()
                };
            } else {
                {"url": url, "success": false, "error": "HTTP " + response.get("status").str()};
            };
        } catch (error) {
            /* Handle network or HTTP errors */
            {"url": url, "success": false, "error": error.get("message")};
        };
    });
    results;
};

/* Define a function to extract data from HTML content */
extract_data = op(html_content) {
    /* Parse HTML and extract specific elements */
    doc = $XML().parse(html_content);
    titles = doc.select("h1, h2, h3");
    links = doc.select("a[href]");
    
    {
        "titles": titles.map(op(t) { t.get("text"); }),
        "links": links.map(op(l) { l.get("href"); })
    };
};

/* Example usage: list of target URLs to scrape */
target_urls = [
    "https://example.com/page1",
    "https://example.com/page2",
    "https://example.com/page3"
];

/* Scrape the pages with a 1 second delay between requests */
scraped_data = scrape_pages(target_urls, 1000);

/* Extract data from each successfully scraped page */
extracted_data = scraped_data.map(op(page) { 
    if (page.get("success")) {
        extract_data(page.get("content"));
    } else {
        {"error": page.get("error")};
    };
});
```

## Example: API Data Collection
```grapa
/* Collect data from REST API endpoints */
collect_api_data = op(base_url, endpoints) {
    results = endpoints.map(op(endpoint) {
        try {
            url = base_url + endpoint;
            response = $net().get(url);
            
            if (response.get("status") == 200) {
                /* Parse JSON response */
                data = $JSON().parse(response.get("body"));
                {"endpoint": endpoint, "success": true, "data": data};
            } else {
                {"endpoint": endpoint, "success": false, "error": "HTTP " + response.get("status").str()};
            };
        } catch (error) {
            {"endpoint": endpoint, "success": false, "error": error.get("message")};
        };
    });
    results;
};

/* Example API endpoints */
api_endpoints = ["/users", "/products", "/orders"];
api_data = collect_api_data("https://api.example.com", api_endpoints);

/* Process collected data */
successful_data = api_data.filter(op(result) { result.get("success"); });
failed_requests = api_data.filter(op(result) { !result.get("success"); });

("Successfully collected data from " + successful_data.len().str() + " endpoints").echo();
("Failed requests: " + failed_requests.len().str()).echo();
```

## Example: Automated Form Submission
```grapa
/* Automated form submission with validation */
submit_form = op(form_data) {
    try {
        /* Prepare form data */
        post_data = $JSON().stringify(form_data);
        
        /* Submit form via POST request */
        response = $net().post("https://example.com/submit", post_data, {
            "Content-Type": "application/json"
        });
        
        if (response.get("status") == 200) {
            result = $JSON().parse(response.get("body"));
            {"success": true, "result": result};
        } else {
            {"success": false, "error": "HTTP " + response.get("status").str()};
        };
    } catch (error) {
        {"success": false, "error": error.get("message")};
    };
};

/* Example form data */
form_data = {
    "name": "John Doe",
    "email": "john@example.com",
    "message": "Hello from Grapa!"
};

/* Submit the form */
result = submit_form(form_data);
if (result.get("success")) {
    ("Form submitted successfully: " + result.get("result").get("message")).echo();
} else {
    ("Form submission failed: " + result.get("error")).echo();
}
```

- **See also:** [Python Web Scraping Examples](../PYTHON_USE_CASES.md#5-webdata-scraping-automation) 