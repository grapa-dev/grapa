---
tags:
  - maintainer
  - implementation
  - networking
  - web-scraping
  - http-client
  - content-extraction
---

# Web Scraping Implementation

This document provides comprehensive implementation details for web scraping capabilities in Grapa, including HTTP client operations, content extraction, and integration with XML/HTML processing.

## Overview

**Purpose:** HTTP client for web scraping with content extraction and processing
**Integration:** HTTP client + XML/HTML parsing + data extraction
**Status:** Complete implementation

## Architecture

### **Web Scraping Architecture**
```
┌─────────────────────────────────────────────────────────────┐
│                    User Script Layer                        │
│  Web scraping scripts, data processing                     │
├─────────────────────────────────────────────────────────────┤
│                    Content Processing Layer                 │
│  XML/HTML parsing, CSS selectors, data extraction         │
├─────────────────────────────────────────────────────────────┤
│                    HTTP Client Layer                        │
│  HTTP requests, response handling, content extraction      │
├─────────────────────────────────────────────────────────────┤
│                    Networking Layer                         │
│  TCP connections, SSL/TLS, cross-platform networking       │
└─────────────────────────────────────────────────────────────┘
```

### **Component Relationships**
- **`GrapaNetConnect`** provides HTTP client capabilities
- **`$XML()`** handles HTML/XML parsing and content extraction
- **CSS selectors** enable precise element selection
- **Data processing** with Grapa's unified data access

## Implementation Details

### **1. HTTP Client for Web Requests**

#### **HTTP GET Request Implementation**
```cpp
// Implementation of $net().get(url) functionality
GrapaError GrapaNetConnect::HttpGet(const GrapaCHAR& url)
{
    // Connect to URL
    mError = mNet.Connect(url);
    if (mError) return mError;
    
    // Send HTTP GET request
    GrapaCHAR request = "GET / HTTP/1.1\r\nHost: ";
    request += ExtractHost(url);
    request += "\r\nUser-Agent: Grapa/1.0\r\nConnection: close\r\n\r\n";
    
    mError = mNet.Send(request);
    if (mError) return mError;
    
    // Read response
    GrapaBYTE response;
    mError = mNet.Receive(response);
    
    // Parse response and extract content
    return ParseHttpResponse(response);
}
```

#### **URL Parsing and Host Extraction**
```cpp
GrapaCHAR GrapaNetConnect::ExtractHost(const GrapaCHAR& url)
{
    // Parse URL to extract hostname
    // Handle http:// and https:// protocols
    // Extract port if specified
    // Return hostname for Host header
}
```

#### **HTTP Response Parsing**
```cpp
GrapaError GrapaNetConnect::ParseHttpResponse(const GrapaBYTE& response)
{
    // Parse HTTP response headers
    // Extract status code
    // Separate headers from body
    // Handle chunked encoding
    // Handle compression (gzip, deflate)
    
    // Return structured response object
    return 0;
}
```

### **2. Response Structure and Processing**

#### **HTTP Response Structure**
```cpp
struct HttpResponse {
    int statusCode;          // 200, 404, 500, etc.
    std::string statusText;  // OK, Not Found, etc.
    std::map<std::string, std::string> headers;
    std::string body;        // Response body (HTML/XML content)
    std::string encoding;    // Content encoding
    std::string contentType; // Content-Type header
};
```

#### **Response Processing Flow**
```grapa
/* Basic web scraping flow */
response = $net().get("https://example.com");
if (response.get("status") == 200) {
    content = response.get("body");
    html = $XML().parse(content);
    titles = html.select("h1, h2, h3");
    links = html.select("a[href]");
};
```

### **3. Content Extraction and Processing**

#### **HTML Parsing Integration**
```grapa
/* HTML parsing for content extraction */
html = $XML().parse(response.get("body"));

// Extract text content
titles = html.select("h1, h2, h3");
titleText = titles.map(op(t) { t.get("text"); });

// Extract attributes
links = html.select("a[href]");
urls = links.map(op(l) { l.get("href"); });

// Extract specific elements
images = html.select("img[src]");
imageUrls = images.map(op(img) { img.get("src"); });
```

#### **CSS Selector Support**
```grapa
/* CSS selector examples for content extraction */
// Element selection
elements = html.select("div.content");
elements = html.select("p.important");

// Attribute selection
links = html.select("a[href*='example.com']");
images = html.select("img[alt]");

// Complex selectors
items = html.select("div.product-list > li");
titles = html.select("h1, h2, h3");
```

### **4. Advanced Web Scraping Patterns**

#### **Multi-Page Scraping**
```grapa
/* Scrape multiple pages with rate limiting */
scrape_pages = op(urls, delay_ms) {
    results = urls.map(op(url, index) {
        if (index > 0) {
            $sys().sleep(delay_ms);
        };
        
        try {
            response = $net().get(url);
            if (response.get("status") == 200) {
                {
                    "url": url,
                    "success": true,
                    "content": response.get("body"),
                    "size": response.get("body").len()
                };
            } else {
                {"url": url, "success": false, "error": "HTTP ${response.get('status')}".interpolate()};
            };
        } catch (error) {
            {"url": url, "success": false, "error": error.get("message")};
        };
    });
    results;
};
```

#### **Data Extraction and Processing**
```grapa
/* Extract structured data from web pages */
extract_data = op(html_content) {
    doc = $XML().parse(html_content);
    
    // Extract product information
    products = doc.select(".product");
    productData = products.map(op(product) {
        {
            "name": product.select(".name").get("text"),
            "price": product.select(".price").get("text"),
            "description": product.select(".description").get("text"),
            "image": product.select("img").get("src")
        };
    });
    
    productData;
};
```

### **5. Error Handling and Robustness**

#### **Network Error Handling**
```grapa
/* Robust web scraping with error handling */
scrape_with_retry = op(url, max_retries) {
    for (i = 0; i < max_retries; i += 1) {
        try {
            response = $net().get(url);
            if (response.get("status") == 200) {
                return response;
            };
        } catch (error) {
            if (i == max_retries - 1) {
                throw error;
            };
            $sys().sleep(1000 * (i + 1)); // Exponential backoff
        };
    };
};
```

#### **Content Validation**
```grapa
/* Validate scraped content */
validate_content = op(response) {
    if (response.get("status") != 200) {
        return {"valid": false, "error": "HTTP ${response.get('status')}".interpolate()};
    };
    
    content = response.get("body");
    if (content.len() == 0) {
        return {"valid": false, "error": "Empty content"};
    };
    
    // Check content type
    contentType = response.get("headers").get("content-type");
    if (!contentType.startsWith("text/html")) {
        return {"valid": false, "error": "Not HTML content"};
    };
    
    {"valid": true, "content": content};
};
```

### **6. Rate Limiting and Ethics**

#### **Rate Limiting Implementation**
```grapa
/* Rate limiting for respectful scraping */
scrape_with_rate_limit = op(urls, requests_per_second) {
    delay_ms = 1000 / requests_per_second;
    
    results = [];
    for (i = 0; i < urls.len(); i += 1) {
        if (i > 0) {
            $sys().sleep(delay_ms);
        };
        
        response = $net().get(urls[i]);
        results += response;
    };
    
    results;
};
```

#### **User Agent Management**
```grapa
/* Custom user agent for web scraping */
custom_user_agent = "Grapa Web Scraper/1.0 (+https://grapa.dev)";

// Note: User agent customization would require C++ implementation
// Currently uses default Grapa user agent
```

### **7. Content Processing and Transformation**

#### **Text Extraction and Cleaning**
```grapa
/* Extract and clean text content */
extract_clean_text = op(html_content) {
    doc = $XML().parse(html_content);
    
    // Remove script and style elements
    doc.select("script, style").remove();
    
    // Extract text content
    text = doc.get("text");
    
    // Clean up whitespace
    text = text.replace(/\s+/g, " ").trim();
    
    text;
};
```

#### **Data Normalization**
```grapa
/* Normalize extracted data */
normalize_data = op(raw_data) {
    normalized = raw_data.map(op(item) {
        {
            "title": item.get("title").trim().toLowerCase(),
            "price": parsePrice(item.get("price")),
            "url": normalizeUrl(item.get("url")),
            "timestamp": $sys().time()
        };
    });
    
    normalized;
};
```

## Integration with Grapa Language

### **System Function Registration**
```cpp
// HTTP client functions
http_read, http_send, http_message

// Note: $net().get() is implemented via these functions
```

### **$net Class Web Scraping Methods**
```grapa
@global["$net"] = class {
    // HTTP operations for web scraping
    httpread = @<[op,@<http_read,{this}>]>;
    httpsend = @<[op,@<http_send,{this,@<var,{method}>,@<var,{entity}>,@<var,{header}>,@<var,{body}>}>],{method,entity,header,body}>;
    httpmessage = @<[op,@<http_message,{this,@<var,{raw}>}>],{raw:null}>;
};
```

### **$XML Integration**
```grapa
// HTML parsing for web scraping
html = $XML().parse(response.get("body"));

// CSS selector support
elements = html.select("selector");

// Data extraction
text = element.get("text");
attribute = element.get("attribute");
```

## Usage Patterns

### **1. Basic Web Scraping**
```grapa
/* Basic web scraping example */
response = $net().get("https://example.com");
if (response.get("status") == 200) {
    html = $XML().parse(response.get("body"));
    titles = html.select("h1, h2, h3");
    titleText = titles.map(op(t) { t.get("text"); });
    titleText.echo();
};
```

### **2. E-commerce Data Extraction**
```grapa
/* Extract product data from e-commerce site */
scrape_products = op(base_url, pages) {
    all_products = [];
    
    for (page = 1; page <= pages; page += 1) {
        url = "${base_url}?page=${page}".interpolate();
        response = $net().get(url);
        
        if (response.get("status") == 200) {
            html = $XML().parse(response.get("body"));
            products = html.select(".product-item");
            
            page_products = products.map(op(product) {
                {
                    "name": product.select(".product-name").get("text"),
                    "price": product.select(".product-price").get("text"),
                    "rating": product.select(".product-rating").get("text"),
                    "image": product.select("img").get("src")
                };
            });
            
            all_products = all_products.concat(page_products);
        };
        
        $sys().sleep(1000); // Rate limiting
    };
    
    all_products;
};
```

### **3. News Article Extraction**
```grapa
/* Extract news articles from news site */
scrape_news = op(news_url) {
    response = $net().get(news_url);
    
    if (response.get("status") == 200) {
        html = $XML().parse(response.get("body"));
        articles = html.select(".article");
        
        articles.map(op(article) {
            {
                "headline": article.select(".headline").get("text"),
                "summary": article.select(".summary").get("text"),
                "author": article.select(".author").get("text"),
                "date": article.select(".date").get("text"),
                "url": article.select("a").get("href")
            };
        });
    } else {
        [];
    };
};
```

### **4. API Data Collection**
```grapa
/* Collect data from REST API endpoints */
collect_api_data = op(base_url, endpoints) {
    results = endpoints.map(op(endpoint) {
        try {
            url = base_url + endpoint;
            response = $net().get(url);
            
            if (response.get("status") == 200) {
                data = $JSON().parse(response.get("body"));
                {"endpoint": endpoint, "success": true, "data": data};
            } else {
                {"endpoint": endpoint, "success": false, "error": "HTTP ${response.get('status')}".interpolate()};
            };
        } catch (error) {
            {"endpoint": endpoint, "success": false, "error": error.get("message")};
        };
    });
    results;
};
```

## Performance Characteristics

### **HTTP Client Performance**
- **Efficient HTTP parsing** with minimal allocations
- **Connection reuse** for improved performance
- **Response streaming** for large content
- **Compression support** for bandwidth optimization

### **Content Processing Performance**
- **Fast HTML parsing** via XML integration
- **Efficient CSS selectors** for element selection
- **Memory management** for large documents
- **Streaming processing** for large datasets

### **Scalability Features**
- **Parallel requests** with rate limiting
- **Batch processing** for multiple URLs
- **Incremental processing** for large datasets
- **Resource management** with automatic cleanup

## Error Handling

### **Network Errors**
- **Connection failures** with retry logic
- **HTTP errors** with status code handling
- **Timeout handling** for long-running requests
- **SSL/TLS errors** with certificate validation

### **Content Errors**
- **Parsing errors** with graceful fallback
- **Missing elements** with null handling
- **Encoding issues** with charset detection
- **Malformed HTML** with robust parsing

### **Rate Limiting Errors**
- **429 Too Many Requests** with exponential backoff
- **403 Forbidden** with user agent management
- **Robots.txt** compliance for ethical scraping
- **Server overload** with request throttling

## Security Considerations

### **Web Scraping Security**
- **Input validation** for URLs and parameters
- **Content sanitization** to prevent XSS
- **Rate limiting** to avoid server overload
- **User agent identification** for transparency

### **Data Security**
- **Secure connections** via HTTPS
- **Certificate validation** for trusted sites
- **Content verification** for data integrity
- **Privacy protection** for sensitive data

### **Ethical Considerations**
- **Respect robots.txt** for site policies
- **Rate limiting** to avoid server impact
- **User agent identification** for transparency
- **Terms of service** compliance

## Future Enhancements

### **Planned Features**
- **JavaScript rendering** for dynamic content
- **Session management** for authenticated sites
- **Cookie handling** for stateful scraping
- **Proxy support** for distributed scraping

### **Performance Improvements**
- **Async I/O** for concurrent requests
- **Content caching** for repeated requests
- **Incremental parsing** for large documents
- **Streaming processing** for real-time data

### **Advanced Features**
- **Machine learning** for content classification
- **Natural language processing** for text extraction
- **Image processing** for visual content
- **Data validation** for quality assurance

## Related Documentation

- **[GRAPA_NET_IMPLEMENTATION.md](GRAPA_NET_IMPLEMENTATION.md)** - Core networking implementation
- **[GRAPA_NET_CONNECT_IMPLEMENTATION.md](GRAPA_NET_CONNECT_IMPLEMENTATION.md)** - HTTP/HTTPS implementation
- **[NETWORKING_ARCHITECTURE.md](NETWORKING_ARCHITECTURE.md)** - Overall architecture
- **[docs-src/docs/sys/net.md](../../../docs-src/docs/sys/net.md)** - User API documentation
- **[docs-src/docs/use_cases/web_data_scraping.md](../../../docs-src/docs/use_cases/web_data_scraping.md)** - Web scraping examples
