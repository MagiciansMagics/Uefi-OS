#!/usr/bin/env python3
import struct
import sys

PSF1_MAGIC = b'\x36\x04'
PSF2_MAGIC = 0x864ab572

def strip_psf(infile, outfile):
    with open(infile, "rb") as f:
        data = f.read()

    # Detect PSF1
    if data[:2] == PSF1_MAGIC:
        mode = data[2]
        charheight = data[3]
        num_glyphs = 256 if (mode & 1) == 0 else 512

        header_size = 4
        glyph_size = charheight
        bitmap_offset = header_size
        bitmap_end = bitmap_offset + num_glyphs * glyph_size

        print(f"PSF1 detected: {num_glyphs} glyphs, {charheight} bytes each")

        glyphs = data[bitmap_offset:bitmap_end]

    else:
        # Detect PSF2
        (magic,) = struct.unpack_from("<I", data, 0)
        if magic != PSF2_MAGIC:
            print("ERROR: Not a valid PSF file")
            return

        (
            magic, version, header_size, flags, num_glyphs,
            bytes_per_glyph, height, width
        ) = struct.unpack_from("<IIIIIIII", data, 0)

        print(f"PSF2 detected: {num_glyphs} glyphs, {bytes_per_glyph} bytes each")

        bitmap_offset = header_size
        bitmap_end = bitmap_offset + num_glyphs * bytes_per_glyph

        glyphs = data[bitmap_offset:bitmap_end]

    # Write only raw glyph data
    with open(outfile, "wb") as out:
        out.write(glyphs)

    print(f"Wrote raw glyph data to: {outfile}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: strip_psf.py input.psf output.raw")
        sys.exit(1)

    strip_psf(sys.argv[1], sys.argv[2])
