# IEEE754 Number Converter

This C program is developed for educational purposes as part of the Introduction to Computer Science module. It provides a basic understanding of how floating-point numbers are represented in IEEE754 format.

## Usage

To use the program, follow these instructions:

1. Compile the program:

    ```
    gcc main.c -o ieee754_converter -lm
    ```

2. Run the program:

    ```
    ./ieee754_converter <decimal_number> <ieee_format>
    ```

    - `<decimal_number>`: The decimal number you want to convert.
    - `<ieee_format>`: The IEEE754 format you want to use (`binary16`, `binary32`, `binary64`, or `binary128`).


## Supported IEEE754 Formats

The program supports the following IEEE754 formats:

- Binary16 (binary16): 5 exponent bits, 10 mantissa bits, bias of 15.
- Binary32 (binary32): 8 exponent bits, 23 mantissa bits, bias of 127.
- Binary64 (binary64): 11 exponent bits, 52 mantissa bits, bias of 1023.
- Binary128 (binary128): 15 exponent bits, 112 mantissa bits, bias of 16383.

## Example

```
./ieee754_converter 42.625 binary64
```

## Important Note
This program is designed for educational purposes and may not cover all edge cases or error handling. 