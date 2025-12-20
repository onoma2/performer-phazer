#!/usr/bin/env python3
"""
Script to remove <p> tags that contain only a single character from an HTML file.
"""

import re
import sys

def remove_single_char_p_tags(input_file, output_file):
    """
    Remove <p> tags that contain only a single character.
    
    Args:
        input_file: Path to input HTML file
        output_file: Path to output HTML file
    """
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Regular expression to match <p> tags with only a single character inside
    # This pattern looks for <p>, optional whitespace, a single character, optional whitespace, and </p>
    single_char_p_pattern = r'<p>\s*\w\s*</p>|<p>\s*[^\w\s<>&]\s*</p>'
    
    # More comprehensive pattern to catch any <p> tag with just one character (letter, number, or symbol)
    comprehensive_pattern = r'<p>\s*[\w\s]*\w[\w\s]*\s*</p>'
    
    # Actually, let's be more precise: match <p> tags that contain only one non-whitespace character
    # This pattern matches <p> tags with just one meaningful character inside
    one_char_pattern = r'<p>\s*\S\s*</p>'
    
    # Find all matches of single character <p> tags to report
    single_char_matches = re.findall(one_char_pattern, content)
    print(f"Found {len(single_char_matches)} single character <p> tags to remove")
    
    # Remove the single character <p> tags
    cleaned_content = re.sub(one_char_pattern, '', content)
    
    # Also remove empty <p> tags that might result from the removal
    cleaned_content = re.sub(r'<p>\s*</p>', '', cleaned_content)
    
    # Clean up any extra whitespace that might result
    cleaned_content = re.sub(r'\n\s*\n', '\n\n', cleaned_content)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(cleaned_content)
    
    print(f"Cleaned HTML saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python remove_single_char_p_tags.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    remove_single_char_p_tags(input_file, output_file)