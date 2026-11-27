#include <kstdio.h>
#include <uart.h>

void kstdio_init(void) { uart_init(); }

void kputs(const char *str) { uart_puts(str); }

void kputc(char c) { uart_putc(c); }

int kgetc(void) { return (int)uart_getc(); }

void kgets(char *buf, int max_len) {
  int index = 0;
  while (1) {
    char c = kgetc();

    if (c == '\r' || c == '\n') {
      kprintf("\n");
      buf[index] = '\0';
      break;
    } else if (c == '\b' || c == 0x7F) {
      if (index > 0) {
        index--;
        kputc('\b');
        kputc(' ');
        kputc('\b');
      }
    } else if (c >= 32 && c <= 126) {
      if (index < max_len - 1) {
        buf[index++] = c;
        kputc(c);
      }
    }
  }
}

int kstdio_has_data(void) { return uart_has_data(); }

// Printa um inteiro sem sinal
// Usa um buffer para inverter os digitos no final
static void print_uint_base(unsigned long long num, int base) {
  (void)num; (void)base;
//  char buffer[65];
//  int i = 0;
//
//  if (num == 0) {
//    uart_putc('0');
//    return;
//  }
//
//  while (num > 0) {
//    unsigned long long remainder = num % (unsigned long long)base;
//    if (remainder < 10) {
//      buffer[i++] = (char)('0' + remainder);
//    } else {
//      buffer[i++] = (char)('a' + (remainder - 10));
//    }
//    num /= (unsigned long long)base;
//  }
//
//  while (i > 0) {
//    uart_putc(buffer[--i]);
//  }
}

// Printa inteiro com sinal em base qualquer
static void print_int_base(long long num, int base) {
  if (num < 0) {
    uart_putc('-');
    num = -num;
  }

  print_uint_base((unsigned long long)num, base);
}

// Calcula 10^precision de forma simples (usado para parte fracionaria)
//static unsigned long long pow10_u64(int precision) {
//  unsigned long long result = 1;
//
//  while (precision-- > 0) {
//    result *= 10ull;
//  }
//
//  return result;
//}

// Printa double no formato decimal simples
// Tem precisao fixa (%.Nf). Nao usa notacao cientifica
static void print_float(double value, int precision) {
  (void)value; (void)precision;
//  // Precisao padrao do printf de 6 casas
//  if (precision < 0) {
//    precision = 6;
//  }
//
//  // Evita estourar o buffer e simplifica o arredondamento
//  // Acho que da pra ser 18 mas deu certo com 17
//  if (precision > 17) {
//    precision = 17;
//  }
//
//  // NaN eh o unico numero que nao eh igual a ele mesmo
//  if (value != value) {
//    uart_puts("nan");
//    return;
//  }
//
//  // Sinal
//  if (value < 0.0) {
//    uart_putc('-');
//    value = -value;
//  }
//
//  // Parte inteira e fracionaria
//  unsigned long long int_part = (unsigned long long)value;
//  double frac = value - (double)int_part;
//
//  // Converte parte fracionaria para inteiro arredondando
//  unsigned long long scale = pow10_u64(precision);
//  unsigned long long frac_part =
//      (unsigned long long)(frac * (double)scale + 0.5);
//
//  // Se arredondamento estourar, ajusta a parte inteira
//  if (frac_part >= scale) {
//    int_part += 1;
//    frac_part = 0;
//  }
//
//  print_uint_base(int_part, 10);
//
//  if (precision > 0) {
//    char buffer[20];
//
//    uart_putc('.');
//
//    // Preenche do fim para o comeco com zeros a esquerda
//    for (int i = 0; i < precision; i++) {
//      buffer[precision - 1 - i] = (char)('0' + (frac_part % 10));
//      frac_part /= 10;
//    }
//
//    for (int i = 0; i < precision; i++) {
//      uart_putc(buffer[i]);
//    }
//  }
}

// printf simplificado
// Suporta %c %s %d %u %x %f %% e precisao em %f
void kprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);

  while (*format != '\0') {
    if (*format == '%') {
      format++;

      // Suporte simples a precisao (%.Nf) igual o printf nativo
      int precision = -1;

      if (*format == '.') {
        format++;
        precision = 0;

        while (*format >= '0' && *format <= '9') {
          precision = (precision * 10) + (*format - '0');
          format++;
        }
      }

      int length_mod = 0;
      if (*format == 'l') {
        length_mod++;
        format++;
      }

      switch (*format) {
      case 'c': {
        char c = (char)va_arg(args, int);
        uart_putc(c);
        break;
      }
      case 's': {
        char *s = va_arg(args, char *);
        if (s) {
          uart_puts(s);
        } else {
          uart_puts("(null)");
        }
        break;
      }
      case 'd': {
        long long n;
        if (length_mod >= 2) {
          n = va_arg(args, long long);
        } else if (length_mod == 1) {
          n = va_arg(args, long);
        } else {
          n = va_arg(args, int); // Default 32-bit
        }
        print_int_base(n, 10);
        break;
      }
      case 'u': {
        unsigned long long u;
        if (length_mod >= 2) {
          u = va_arg(args, unsigned long long);
        } else if (length_mod == 1) {
          u = va_arg(args, unsigned long);
        } else {
          u = va_arg(args, unsigned int);
        }
        print_uint_base(u, 10);
        break;
      }
      case 'x': {
        unsigned int x = va_arg(args, unsigned int);
        uart_puts("0x");
        print_uint_base(x, 16);
        break;
      }
      case 'f': {
        double f = va_arg(args, double);
        print_float(f, precision);
        break;
      }
      case '%': {
        uart_putc('%');
        break;
      }
      default: {
        uart_putc('%');
        uart_putc(*format);
        break;
      }
      }
    } else {
      uart_putc(*format);
    }
    format++;
  }

  va_end(args);
}

int sys_write(char* buffer, unsigned max_len) {
    if (!buffer || !max_len) return 0;

    unsigned chars_written = 0;
    while (chars_written < max_len && buffer[chars_written] != '\0') {
        kputc(buffer[chars_written]);
        chars_written++;
    }

    return chars_written;
}

int sys_read(char* buffer, unsigned max_len) {
    if (!buffer || !max_len) return 0;

    unsigned length = 0;
    while (length < max_len) {
        buffer[length] = uart_getc();
        length++;
    }
    return length;
}

