/*
 * "Struktur CFG"
 * 
 * E -> E + T | E - T | T
 * T -> T * P | T / P | P
 * P -> F ^ P | F
 * F -> U | - U
 * U -> ( E ) | N
 * N -> D . D | D | . D
 * D -> 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | D D
 * 
 * Mengembalikan nilai dengan formatter %g.
 */

/* 
 * File:   main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// slot token.
char slot[2048];
int n = 0;

// current char.
int CC;

// syntax token.

enum {
    NUMBER,
    L_PARENTHESES,
    R_PARENTHESES,
    MUL_DIV,
    ADD_SUB,
    POWD,
    END
} syntax;

// deklarasi fungsi.
void init(void);
void adv(void);
double expression(void);
void error(char *msg);
double power(void);

int main(int argc, char** argv) {
    printf("hello\n");
    printf("\n"
            " ,-.     .         .     .           \n"
            "/        |         |     |           \n"
            "|    ,-: | ,-. . . | ,-: |-  ,-. ;-. \n"
            "\\    | | | |   | | | | | |   | | |   \n"
            " `-' `-` ' `-' `-` ' `-` `-' `-' '   \n"
            "\n"
            "Operator:\n"
            " ()\t-- pengelompokan\n"
            "  ^\t-- perpangkatan\n"
            "  *\t-- perkalian\n"
            "  /\t-- pembagian\n"
            "  +\t-- penjumlahan/positif\n"
            "  -\t-- pengurangan/negatif\n"
            "  !\t-- keluar program\n"
            "> ");
    init();
    while (CC != '!') {
        double val = expression();
        if (isfinite(val)) {
            printf("result: %g\n", val);
        } else {
            printf("MATH ERROR\n");
        }
        if (syntax != END) error("junk after expression");
        printf("> ");
        adv();
    }
    return (EXIT_SUCCESS);
}

// Keluar jika ada kesalahan syntax.

void error(char *msg) {
    printf("SYNTAX ERROR\n");
    fprintf(stderr, "Error: %s. quit.\n", msg);
    exit(1);
}

// Menyimpan CC, lalu membaca char baru.

void read() {
    slot[n++] = CC;
    slot[n] = '\0'; // Terminate the string.
    CC = getchar();
}

// Mengabaikan char.

void ignore() {
    CC = getchar();
}

// Me-reset slot token.

void reset() {
    n = 0;
    slot[0] = '\0';
}

// membaca dan memeriksa syntax.
// juga sebagai CFG NUMBER dan DIGIT.

int scan() {
    reset();
START:
    switch (CC) {
        case ' ': case '\t': case '\r':
            ignore();
            goto START;

        case '^':
            read();
            return POWD;

        case '-': case '+':
            read();
            return ADD_SUB;

        case '*': case '/':
            read();
            return MUL_DIV;

        case '(':
            read();
            return L_PARENTHESES;

        case ')':
            read();
            return R_PARENTHESES;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            read();
            goto IN_LEADING_DIGITS;

        case '\n':
            CC = ' '; // delayed ignore()
            return END;

        case '.':
            read();
            goto IN_TRAILING_DIGITS;

        case '!':
            return END;

        default:
            error("bad character");
    }

IN_LEADING_DIGITS:
    switch (CC) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            read();
            goto IN_LEADING_DIGITS;

        case '.':
            read();
            goto IN_TRAILING_DIGITS;

        default:
            return NUMBER;
    }

IN_TRAILING_DIGITS:
    switch (CC) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            read();
            goto IN_TRAILING_DIGITS;

        default:
            return NUMBER;
    }
}

// membaca syntax masukan selanjutnya.

void adv() {
    syntax = scan();
}

// Me-reset slot token lalu membaca syntax selanjutnya.

void init() {
    reset();
    ignore();
    adv();
}

// CFG NOTSIGNED.

double notsigned() {
    double rtn = 0;
    switch (syntax) {
        case NUMBER:
            sscanf(slot, "%lf", &rtn);
            adv();
            break;

        case L_PARENTHESES:
            adv();
            rtn = expression();
            if (syntax != R_PARENTHESES) error("missing ')'");
            adv();
            break;

        default:
            error("unexpected token");
    }
    return rtn;
}

// CFG FACTOR.

double factor() {
    double rtn = 0;
    // If there is a leading minus...
    if (syntax == ADD_SUB && slot[0] == '-') {
        adv();
        rtn = -notsigned();
    } else
        rtn = notsigned();
    return rtn;
}

// CFG TERM.

double term() {
    double rtn = power();
    while (syntax == MUL_DIV) {
        switch (slot[0]) {
            case '*':
                adv();
                rtn *= power();
                break;

            case '/':
                adv();
                rtn /= power();
                break;
        }
    }
    return rtn;
}

// CFG EXPRESSION.

double expression() {
    double rtn = term();
    while (syntax == ADD_SUB) {
        switch (slot[0]) {
            case '+':
                adv();
                rtn += term();
                break;

            case '-':
                adv();
                rtn -= term();
                break;
        }
    }
    return rtn;
}

// CFG Power.

double power() {
    double rtn = factor();
    double fs[2048];
    fs[0] = rtn;
    int i = 1;
    while (syntax == POWD) {
        switch (slot[0]) {
            case '^':
                adv();
                fs[i] = factor();
                break;
        }
        i++;
    }
    while (i--) {
        fs[i - 1] = pow(fs[i - 1], fs[i]);
    }
    rtn = fs[0];
    return rtn;
}
