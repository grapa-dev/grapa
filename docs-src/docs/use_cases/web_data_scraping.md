# Web/Data Scraping & Automation

> **Best Practices:**
> - Use `$net().get(url)` for HTTP requests; always check `.getfield("status")` for HTTP status.
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
        
        try response = $net().get(url);
        catch (error) {"url": url, "success": false, "error": error.getfield("message")};
        
        /* Check for successful response */
        if (response.getfield("status") == 200) {
            {
                "url": url,
                "success": true,
                "content": response.getfield("body"),
                "size": response.getfield("body").len()
            };
        } else {
            {"url": url, "success": false, "error": "HTTP " + response.getfield("status").str()};
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
        "titles": titles.map(op(t) { t.getfield("text"); }),
        "links": links.map(op(l) { l.getfield("href"); })
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
    if (page.getfield("success")) {
        extract_data(page.getfield("content"));
    } else {
        {"error": page.getfield("error")};
    };
});
```

## Example: API Data Collection
```grapa
/* Collect data from REST API endpoints */
collect_api_data = op(base_url, endpoints) {
    results = endpoints.map(op(endpoint) {
        try url = base_url + endpoint;
        catch (error) {"endpoint": endpoint, "success": false, "error": error.getfield("message")};
        
        response = $net().get(url);
        
        if (response.getfield("status") == 200) {
            /* Parse JSON response */
            data = $JSON().parse(response.getfield("body"));
            {"endpoint": endpoint, "success": true, "data": data};
        } else {
            {"endpoint": endpoint, "success": false, "error": "HTTP " + response.getfield("status").str()};
        };
    });
    results;
};

/* Example API endpoints */
api_endpoints = ["/users", "/products", "/orders"];
api_data = collect_api_data("https://api.example.com", api_endpoints);

/* Process collected data */
successful_data = api_data.filter(op(result) { result.getfield("success"); });
failed_requests = api_data.filter(op(result) { !result.getfield("success"); });

("Successfully collected data from " + successful_data.len().str() + " endpoints").echo();
("Failed requests: " + failed_requests.len().str()).echo();
```

## Example: Automated Form Submission
```grapa
/* Automated form submission with validation */
submit_form = op(form_data) {
    try post_data = $JSON().stringify(form_data);
    catch (error) {"success": false, "error": error.getfield("message")};
    
    /* Submit form via POST request */
    response = $net().post("https://example.com/submit", post_data, {
        "Content-Type": "application/json"
    });
    
    if (response.getfield("status") == 200) {
        result = $JSON().parse(response.getfield("body"));
        {"success": true, "result": result};
    } else {
        {"success": false, "error": "HTTP " + response.getfield("status").str()};
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
if (result.getfield("success")) {
    ("Form submitted successfully: " + result.getfield("result").getfield("message")).echo();
} else {
    ("Form submission failed: " + result.getfield("error")).echo();
}
```

- **See also:** [Python Web Scraping Examples](../python_use_cases.md#5-webdata-scraping-automation-verified) 

---

## Related topics
- [API Reference](../api_reference.md)
- [Examples](../examples/README.md)
- [Language Reference](../syntax/basic_syntax.md) 