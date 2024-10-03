# myls
Simple version of an ls-style command that lists the contents of a given directory.
# Input
    ./myls [dir]

# Output
  File name | # hard links | File type | File size | Content preview (16 bytes)

# Example
    ./myls myls_example/contents/

    make:           gcc -o myls myls.c; ./myls    (ls for active directory)
    make clean:     rm myls
