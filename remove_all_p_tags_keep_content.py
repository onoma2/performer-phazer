#!/usr/bin/env python3
"""
Script to remove all <p> and </p> tags while keeping their content.
"""

import re
import sys

def remove_all_p_tags_keep_content(input_file, output_file):
    """
    Remove all <p> and </p> tags while keeping their content.
    
    Args:
        input_file: Path to input HTML file
        output_file: Path to output HTML file
    """
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Count the number of <p> tags before processing
    original_count = len(re.findall(r'<p[^>]*>', content))
    
    # Remove all opening <p> tags (with any attributes)
    content = re.sub(r'<p[^>]*>', '', content)
    
    # Remove all closing </p> tags
    content = re.sub(r'</p>', '', content)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print(f"Removed {original_count} <p> tags.")
    print(f"Cleaned HTML saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python remove_all_p_tags_keep_content.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    remove_all_p_tags_keep_content(input_file, output_file)