#!/usr/bin/env python3
"""
Download CA Bundle Script for Grapa

This script downloads the latest CA bundle from trusted sources and places it
in the keys/ directory for use with SSL/TLS connections.

Usage:
    python3 scripts/download_ca_bundle.py
    python3 scripts/download_ca_bundle.py --verify
    python3 scripts/download_ca_bundle.py --source mozilla
"""

import os
import sys
import urllib.request
import urllib.error
import ssl
import argparse
from pathlib import Path

# CA Bundle sources
CA_SOURCES = {
    'mozilla': {
        'url': 'https://curl.se/ca/cacert.pem',
        'description': 'Mozilla CA Bundle (via curl.se)',
        'filename': 'ca-bundle.crt'
    },
    'curl': {
        'url': 'https://curl.se/ca/cacert.pem',
        'description': 'cURL CA Bundle',
        'filename': 'ca-bundle.crt'
    },
    'debian': {
        'url': 'https://packages.debian.org/sid/all/ca-certificates/download',
        'description': 'Debian CA Bundle',
        'filename': 'ca-bundle-debian.crt'
    }
}

def download_file(url, filepath, description, verify_ssl=True):
    """Download a file from URL to filepath."""
    print(f"Downloading {description}...")
    print(f"  URL: {url}")
    print(f"  Destination: {filepath}")
    
    try:
        # Create directory if it doesn't exist
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        # Create SSL context
        if verify_ssl:
            ssl_context = ssl.create_default_context()
        else:
            ssl_context = ssl.create_default_context()
            ssl_context.check_hostname = False
            ssl_context.verify_mode = ssl.CERT_NONE
            print("  ⚠️  SSL verification disabled for download")
        
        # Create request with SSL context
        request = urllib.request.Request(url)
        
        # Download with progress
        def progress_hook(block_num, block_size, total_size):
            if total_size > 0:
                percent = min(100, (block_num * block_size * 100) // total_size)
                print(f"\r  Progress: {percent}%", end='', flush=True)
        
        with urllib.request.urlopen(request, context=ssl_context) as response:
            with open(filepath, 'wb') as f:
                while True:
                    chunk = response.read(8192)
                    if not chunk:
                        break
                    f.write(chunk)
        
        print()  # New line after progress
        
        # Get file size
        file_size = os.path.getsize(filepath)
        print(f"✅ Downloaded successfully ({file_size:,} bytes)")
        return True
        
    except urllib.error.URLError as e:
        print(f"❌ Network error: {e}")
        return False
    except Exception as e:
        print(f"❌ Download failed: {e}")
        return False

def verify_ca_bundle(filepath):
    """Verify that the CA bundle file is valid."""
    print(f"Verifying CA bundle: {filepath}")
    
    if not os.path.exists(filepath):
        print(f"❌ File does not exist: {filepath}")
        return False
    
    try:
        # Try to create SSL context with the CA bundle
        context = ssl.create_default_context(cafile=filepath)
        
        # Count certificates in the bundle
        cert_count = 0
        with open(filepath, 'r') as f:
            content = f.read()
            cert_count = content.count('-----BEGIN CERTIFICATE-----')
        
        print(f"✅ CA bundle is valid")
        print(f"  Contains {cert_count} certificates")
        print(f"  File size: {os.path.getsize(filepath):,} bytes")
        return True
        
    except ssl.SSLError as e:
        print(f"❌ SSL verification failed: {e}")
        return False
    except Exception as e:
        print(f"❌ Verification failed: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description='Download CA bundle for Grapa SSL/TLS connections',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python3 scripts/download_ca_bundle.py
  python3 scripts/download_ca_bundle.py --verify
  python3 scripts/download_ca_bundle.py --source mozilla
  python3 scripts/download_ca_bundle.py --output keys/custom-ca.crt
        """
    )
    
    parser.add_argument(
        '--source', 
        choices=list(CA_SOURCES.keys()),
        default='mozilla',
        help='CA bundle source (default: mozilla)'
    )
    
    parser.add_argument(
        '--output',
        default='keys/ca-bundle.crt',
        help='Output file path (default: keys/ca-bundle.crt)'
    )
    
    parser.add_argument(
        '--verify',
        action='store_true',
        help='Verify the downloaded CA bundle'
    )
    
    parser.add_argument(
        '--force',
        action='store_true',
        help='Overwrite existing file'
    )
    
    parser.add_argument(
        '--no-ssl-verify',
        action='store_true',
        help='Disable SSL verification for download (use when system CA bundle is missing)'
    )
    
    args = parser.parse_args()
    
    # Get source info
    source_info = CA_SOURCES[args.source]
    output_path = args.output
    
    print("=" * 60)
    print("Grapa CA Bundle Downloader")
    print("=" * 60)
    print(f"Source: {source_info['description']}")
    print(f"Output: {output_path}")
    print()
    
    # Check if file exists
    if os.path.exists(output_path) and not args.force:
        print(f"⚠️  File already exists: {output_path}")
        print("Use --force to overwrite, or --verify to check existing file")
        if args.verify:
            return 0 if verify_ca_bundle(output_path) else 1
        return 0
    
    # Download the CA bundle
    success = download_file(
        source_info['url'], 
        output_path, 
        source_info['description'],
        verify_ssl=not args.no_ssl_verify
    )
    
    if not success:
        print("❌ Download failed")
        return 1
    
    # Verify if requested
    if args.verify:
        print()
        if not verify_ca_bundle(output_path):
            print("❌ Verification failed")
            return 1
    
    print()
    print("🎉 CA bundle download completed successfully!")
    print(f"   File: {output_path}")
    print(f"   Source: {source_info['description']}")
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
