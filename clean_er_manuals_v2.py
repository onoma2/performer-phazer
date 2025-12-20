#!/usr/bin/env python3
"""
Script to clean ER HTML files, keeping only headers, paragraphs, and images.
Improved version that properly reassembles text content.
"""

import re
from bs4 import BeautifulSoup


def clean_pdf_focus_html(input_path, output_path):
    """Clean PDF Focus generated HTML files, extracting and reassembling text content."""
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
        # Look for text content in ssddiv elements
        text_elements = []
        
        for div in page_div.find_all('div', class_='ssddiv'):
            # Extract text content from ssdspan elements
            span_elements = div.find_all('span', class_='ssdspan')
            if span_elements:
                # Combine text from all spans in this div
                text_content = ''
                for span in span_elements:
                    if span.string:
                        text_content += span.string
                    elif span.get_text():
                        text_content += span.get_text()
                
                if text_content.strip():
                    # Check if this text content looks like a heading
                    text_content = text_content.strip()
                    
                    # Check if this is a page number or header-like content
                    if text_content.isdigit() or re.match(r'^[IVX]+$', text_content):  # Roman numerals
                        # Create a heading for page numbers
                        h_tag = soup.new_tag('h2')
                        h_tag.string = text_content
                        body_tag.append(h_tag)
                    elif any(keyword in text_content.lower() for keyword in 
                             ['introduction', 'contents', 'what', 'how', 'section', 'chapter', 'part', 'index']):
                        # Create a heading for likely section titles
                        h_tag = soup.new_tag('h2')
                        h_tag.string = text_content
                        body_tag.append(h_tag)
                    else:
                        # Add as paragraph
                        p_tag = soup.new_tag('p')
                        p_tag.string = text_content
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