#!/usr/bin/env python3
"""
 @file   convert_ppm_to_png.py
 @authors Team 4 - Li
 @date   18.07.2025

 Utility script that batch-converts all PPM images in the current directory
 to PNG format using the Pillow (PIL) library.

 Usage:
   cd examples/output
   python3 convert_ppm_to_png.py
 """

import os
from PIL import Image
import glob

def convert_ppm_to_png():
    """Convert all PPM files in current directory to PNG format"""
    # Get all PPM files in current directory
    ppm_files = glob.glob("*.ppm")
    
    if not ppm_files:
        print("No PPM files found in current directory")
        return
    
    print(f"Found {len(ppm_files)} PPM files to convert:")
    
    for ppm_file in ppm_files:
        try:
            # Open PPM file
            with Image.open(ppm_file) as img:
                # Create PNG filename
                png_file = ppm_file.replace('.ppm', '.png')
                
                # Save as PNG
                img.save(png_file, 'PNG')
                print(f"✓ Converted {ppm_file} -> {png_file}")
                
        except Exception as e:
            print(f"✗ Error converting {ppm_file}: {e}")

if __name__ == "__main__":
    convert_ppm_to_png() 