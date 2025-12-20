#!/usr/bin/env python3
"""
Script to clean ER HTML files, keeping only headers, paragraphs, and images.
Advanced version that properly reassembles text content by position.
"""

import re
from bs4 import BeautifulSoup


def clean_pdf_focus_html(input_path, output_path):
    """Clean PDF Focus generated HTML files, extracting and reassembling text content by position."""
    with open(input_path, 'r', encoding='utf-8') as file:
        content = file.read()
    
    soup = BeautifulSoup(content, 'html.parser')
    
    # Create a new soup object for the cleaned content
    new_soup = BeautifulSoup('', 'html.parser')
    new_soup.append(soup.new_tag('!DOCTYPE html'))
    html_tag = soup.new_tag('html')
    new_soup.append(html_tag)
    
    head_tag = soup.new_tag('head')
    title_tag = soup.new_tag('title')
    title_tag.string = "Cleaned Manual"
    head_tag.append(title_tag)
    html_tag.append(head_tag)
    
    body_tag = soup.new_tag('body')
    html_tag.append(body_tag)
    
    # Find and process the content divs
    for page_div in soup.find_all('div', class_='ssdpage'):
        # Group text elements by position to reconstruct lines
        positioned_text = {}
        
        for div in page_div.find_all('div', class_='ssddiv'):
            # Get the position of this div
            style = div.get('style', '')
            top_match = re.search(r'top:(\d+\.?\d*)pt', style)
            left_match = re.search(r'left:(\d+\.?\d*)pt', style)
            
            if top_match and left_match:
                top = float(top_match.group(1))
                left = float(left_match.group(1))
                
                # Group by top position (to form lines)
                if top not in positioned_text:
                    positioned_text[top] = {}
                
                # Add text content with its left position
                span_elements = div.find_all('span', class_='ssdspan')
                if span_elements:
                    text_content = ''
                    for span in span_elements:
                        if span.string:
                            text_content += span.string
                        elif span.get_text():
                            text_content += span.get_text()
                    
                    positioned_text[top][left] = text_content
        
        # Sort lines by top position and then text by left position
        sorted_lines = sorted(positioned_text.items())
        
        for top, line_content in sorted_lines:
            # Sort text elements in the line by left position
            sorted_text = sorted(line_content.items())
            line_text = ''.join([text for _, text in sorted_text])
            
            # Clean up the text (remove extra spaces, newlines)
            line_text = re.sub(r'\s+', ' ', line_text).strip()
            
            if line_text:
                # Determine if this is a heading based on content
                if any(keyword in line_text.lower() for keyword in 
                       ['introduction', 'contents', 'what', 'how', 'section', 'chapter', 'part', 'index', 'user manual', 'firmware']):
                    h_tag = soup.new_tag('h2')
                    h_tag.string = line_text
                    body_tag.append(h_tag)
                elif line_text.isdigit() or re.match(r'^[IVX]+$', line_text):  # Page numbers or Roman numerals
                    h_tag = soup.new_tag('h3')
                    h_tag.string = line_text
                    body_tag.append(h_tag)
                else:
                    # Add as paragraph
                    p_tag = soup.new_tag('p')
                    p_tag.string = line_text
                    body_tag.append(p_tag)
        
        # Preserve images
        for img in page_div.find_all('img', class_='ssdimg'):
            body_tag.append(img)
    
    # Write the cleaned content to the output file
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(str(new_soup.prettify()))


def main():
    # Clean both HTML files
    files_to_clean = [
        ('er-101-user-manual-f2.09 conv.html', 'er-101-user-manual-cleaned.html'),
        ('er-102-manual-v2.04 conv.html', 'er-102-manual-cleaned.html')
    ]
    
    for input_file, output_file in files_to_clean:
        print(f"Cleaning {input_file}...")
        clean_pdf_focus_html(input_file, output_file)
        print(f"Cleaned version saved as {output_file}")


if __name__ == "__main__":
    main()