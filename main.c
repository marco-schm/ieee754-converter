#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>

struct IEEE754 {
    int exponentBits;
    int mantissaBits;
    int bias;
};

int calculate_binary_digits(int decimal);

int *convert_integral_to_binary(int size, int integral);

int *convert_fractional_part_to_binary(struct IEEE754 ieee754, double fractional_part);

int *normalize_mantissa(const int *integral_bits, int integral_bits_size, int *fractional_bits, int fractional_bits_size);

int *convert_exponent_to_binary(struct IEEE754 ieee754, int exponent_decimal);

int *create_ieee754(struct IEEE754 ieee754, int sign, const int *exponent_bits, int *normalized_mantissa_bits, int size);

void print_binary_representation(int size, int *binary_digits);


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <decimal_number> <ieee_format>\n", argv[0]);
        return 1;
    }
    const struct IEEE754 ieee754_binary16 = {5, 10, 15};
    const struct IEEE754 ieee754_binary32 = {8, 23, 127};
    const struct IEEE754 ieee754_binary64 = {11, 52, 1023};
    const struct IEEE754 ieee754_binary128 = {15, 112, 16383};

    struct IEEE754 selected_ieee754;

    if (strcmp(argv[2], "binary16") == 0) {
        selected_ieee754 = ieee754_binary16;
    } else if (strcmp(argv[2], "binary32") == 0) {
        selected_ieee754 = ieee754_binary32;
    } else if (strcmp(argv[2], "binary64") == 0) {
        selected_ieee754 = ieee754_binary64;
    } else if (strcmp(argv[2], "binary128") == 0) {
        selected_ieee754 = ieee754_binary128;
    } else {
        printf("Invalid IEEE format. Supported formats: binary16, binary32, binary64, binary128\n");
        return 1;
    }

    printf("Using %s format\n", argv[2]);

    double input_number = strtod(argv[1], NULL);
    double decimal_number;
    int sign;

    if (input_number < 0) {
        sign = 1;
        decimal_number = -1 * input_number;
    } else {
        sign = 0;
        decimal_number = input_number;
    }

    printf("Number: %f\n", decimal_number);

    int integral = (int) decimal_number;
    double fractional_part = decimal_number - integral;
    printf("Integral: %d\nFractional Part: %f\n", integral, fractional_part);

    int integral_binary_digits = calculate_binary_digits(integral);

    int *integral_bits = convert_integral_to_binary(integral_binary_digits, integral);
    printf("Binary Representation of Integral Part: ");
    print_binary_representation(integral_binary_digits, integral_bits);

    int *fractional_bits = convert_fractional_part_to_binary(selected_ieee754, fractional_part);
    printf("Binary Representation of Fractional Part: ");
    print_binary_representation(selected_ieee754.mantissaBits, fractional_bits);

    printf("IEEE754 Binary Representation of Sign: ");
    printf("%d\n", sign);

    int *exponent_bits = convert_exponent_to_binary(selected_ieee754, integral_binary_digits - 1);
    printf("IEEE754 Binary Representation of Exponent: ");
    print_binary_representation(selected_ieee754.exponentBits, exponent_bits);

    int *mantissa_bits = normalize_mantissa(integral_bits, integral_binary_digits, fractional_bits,
                                            selected_ieee754.mantissaBits);
    printf("IEEE754 Binary Representation of Mantissa: ");
    print_binary_representation(selected_ieee754.mantissaBits, mantissa_bits);

    int ieee754_size = 1 + selected_ieee754.mantissaBits + selected_ieee754.exponentBits;
    int *ieee754_bits = create_ieee754(selected_ieee754, sign, exponent_bits, mantissa_bits, ieee754_size);
    printf("IEEE754 Binary Representation: ");
    print_binary_representation(ieee754_size, ieee754_bits);


    free(ieee754_bits),
            free(exponent_bits);
    free(integral_bits);
    free(fractional_bits);
    return 0;
}

int calculate_binary_digits(int decimal) {
    return (decimal == 0) ? 1 : (int) (log2(decimal) + 1);
}

int *convert_integral_to_binary(int size, int integral) {
    int *integral_bits = (int *) malloc(size * sizeof(int));
    if (integral_bits == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    int number = integral;

    for (int i = size - 1; i >= 0; i--) {
        integral_bits[i] = number % 2;
        number = number / 2;
    }

    return integral_bits;
}

int *convert_fractional_part_to_binary(struct IEEE754 ieee754, double fractional_part) {
    int *fractional_part_bits = (int *) malloc(ieee754.mantissaBits * sizeof(int));
    if (fractional_part_bits == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < ieee754.mantissaBits; i++) {
        fractional_part = fractional_part * 2;
        if (fractional_part == 1) {
            break;
        } else if (fractional_part > 1) {
            fractional_part = fractional_part - 1;
            fractional_part_bits[i] = 1;
        } else {
            fractional_part_bits[i] = 0;
        }
    }

    return fractional_part_bits;
}

int *normalize_mantissa(const int *integral_bits, int integral_bits_size, int *fractional_bits, int fractional_bits_size) {
    int shift_size = integral_bits_size - 1;

    for (int i = fractional_bits_size - shift_size - 1; i >= 0; i--) {
        fractional_bits[i + shift_size] = fractional_bits[i];
    }

    for (int i = 0; i < shift_size; i++) {
        fractional_bits[i] = integral_bits[i + 1];
    }

    return fractional_bits;
}

int *convert_exponent_to_binary(struct IEEE754 ieee754, int exponent_decimal) {
    int excess_exponent_decimal = exponent_decimal + ieee754.bias;
    int exponent_binary_digits = ieee754.exponentBits;
    int *converted_exponent = convert_integral_to_binary(exponent_binary_digits, excess_exponent_decimal);

    int *ieee754_exponent_bits = (int *) malloc(ieee754.exponentBits * sizeof(int));
    if (ieee754_exponent_bits == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < ieee754.exponentBits; i++) {
        int copy_index = exponent_binary_digits - ieee754.exponentBits + i;
        if (copy_index >= 0) {
            ieee754_exponent_bits[i] = converted_exponent[copy_index];
        } else {
            ieee754_exponent_bits[i] = 0;
        }
    }

    free(converted_exponent);

    return ieee754_exponent_bits;
}

int *create_ieee754(struct IEEE754 ieee754, int sign, const int *exponent_bits, int *normalized_mantissa_bits, int size) {
    int *ieee754_bits = (int *) malloc(size * sizeof(int));
    if (ieee754_bits == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    ieee754_bits[0] = sign;

    for (int i = 1; i < ieee754.exponentBits + 1; i++) {
        ieee754_bits[i] = exponent_bits[i - 1];
    }
    for (int i = ieee754.exponentBits + 1; i < size; i++) {
        ieee754_bits[i] = normalized_mantissa_bits[i - (ieee754.exponentBits + 1)];
    }

    return ieee754_bits;
}

void print_binary_representation(int size, int *binary_digits) {
    for (int i = 0; i < size; i++) {
        printf("%d ", binary_digits[i]);
    }
    printf("\n");
}

