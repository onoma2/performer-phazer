#!/usr/bin/env python3
"""
Script to remove <p> tags (but keep their content) when they contain only a single character.
"""

import re
import sys

def remove_single_char_p_tags_keep_content(input_file, output_file):
    """
    Remove <p> tags (but keep content) when they contain only a single character.
    
    Args:
        input_file: Path to input HTML file
        output_file: Path to output HTML file
    """
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Find all matches of single character <p> tags to report
    # This pattern matches <p> tags that contain only one non-whitespace character
    one_char_pattern = r'<p>\s*(\S)\s*</p>'
    
    single_char_matches = re.findall(one_char_pattern, content)
    print(f"Found {len(single_char_matches)} single character <p> tags to process")
    
    # Replace the single character <p> tags with just the content
    # This keeps the character/content but removes the <p> tags
    cleaned_content = re.sub(one_char_pattern, r'\1', content)
    
    # Clean up any extra whitespace that might result
    cleaned_content = re.sub(r'\n\s*\n', '\n\n', cleaned_content)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(cleaned_content)
    
    print(f"Cleaned HTML saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python remove_single_char_p_tags_keep_content.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    remove_single_char_p_tags_keep_content(input_file, output_file)