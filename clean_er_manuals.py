#!/usr/bin/env python3
"""
Script to clean ER HTML files, keeping only headers, paragraphs, and images.
"""

import re
from bs4 import BeautifulSoup


def clean_html_file(input_path, output_path):
    """Clean an HTML file, keeping only headers, paragraphs, and images."""
    with open(input_path, 'r', encoding='utf-8') as file:
        content = file.read()
    
    soup = BeautifulSoup(content, 'html.parser')
    
    # Find all elements that should be preserved
    allowed_elements = []
    
    # Preserve headers (h1, h2, h3, etc.)
    for tag in soup.find_all(['h1', 'h2', 'h3', 'h4', 'h5', 'h6']):
        allowed_elements.append(tag)
    
    # Preserve paragraphs
    for tag in soup.find_all('p'):
        allowed_elements.append(tag)
    
    # Preserve images
    for tag in soup.find_all('img'):
        allowed_elements.append(tag)
    
    # Preserve div elements that contain only images
    for tag in soup.find_all('div'):
        if tag.find_all('img') and not tag.find_all(['p', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6']):
            allowed_elements.append(tag)
    
    # Create a new soup object for the cleaned content
    new_soup = BeautifulSoup('', 'html.parser')
    
    # Create the basic structure
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
    
    # Add only the allowed elements to the new body
    for element in allowed_elements:
        # Clone the element to avoid modifying the original document
        cloned_element = element.__copy__()
        body_tag.append(cloned_element)
    
    # Write the cleaned content to the output file
    with open(output_path, 'w', encoding='utf-8') as file:
        file.write(str(new_soup.prettify()))


def clean_pdf_focus_html(input_path, output_path):
    """Clean PDF Focus generated HTML files, extracting text content."""
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
        for div in page_div.find_all('div', class_='ssddiv'):
            # Extract text content from ssdspan elements
            span_elements = div.find_all('span', class_='ssdspan')
            if span_elements:
                # Create a paragraph for the text content
                p_tag = soup.new_tag('p')
                text_content = ''
                for span in span_elements:
                    if span.string:
                        text_content += span.string
                    elif span.get_text():
                        text_content += span.get_text()
                if text_content.strip():
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