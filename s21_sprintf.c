// #include "s21_sprintf.h"

#include "s21_string.h"

int s21_sprintf(char *str, char *format, ...) {
  char *str_beg = str;
  arguments argument = {0, 0, 0, 0, 0, 0, 0, 0};
  va_list value;
  va_start(value, format);
  while (*format != '\0') {
    if (*format != '%') {
      *str = *format;
      ++str;
      ++format;
    } else {
      ++format;
      s21_memset(&argument, 0, sizeof(arguments));

      format = flags(format, &argument);
      format = width(format, &argument);
      format = precision(format, &argument);
      format = length(format, &argument);
      format = specifier(format, &argument);

      char *buffer = calloc(BUFFER_SIZE, sizeof(char));

      fill_buffer(buffer, &value, &argument);
      if (((s21_size_t)argument.wid > s21_strlen(buffer)) &&
          !(argument.minus)) {
        for (s21_size_t i = 0; i < argument.wid - s21_strlen(buffer); i++) {
          *str = ' ';
          ++str;
        }
      }
      for (int i = 0; buffer[i]; i++) {
        *str = buffer[i];
        ++str;
      }
      if ((argument.wid) && (argument.minus)) {
        for (s21_size_t i = 0; i < argument.wid - s21_strlen(buffer); i++) {
          *str = ' ';
          ++str;
        }
      }
      free(buffer);
    }
  }
  *str = '\0';
  va_end(value);

  return str - str_beg;
}

char *flags(char *format, arguments *argument) {
  while (s21_strchr(" -+", *format) != s21_NULL) {
    if (*format == '+') {
      argument->plus = 1;
    } else if (*format == '-') {
      argument->minus = 1;
    } else if (*format == ' ') {
      argument->space = 1;
    }

    ++format;
  }
  return format;
}

char *width(char *format, arguments *argument) {
  int num_width = 0;
  format = read_int_from_str(format, &num_width);
  argument->wid = num_width;
  return format;
}

char *precision(char *format, arguments *argument) {
  if (*format == '.') {
    argument->if_prec = 1;
    ++format;
    int num_prec = 0;
    format = read_int_from_str(format, &num_prec);
    argument->prec = num_prec;
  }
  return format;
}

char *length(char *format, arguments *argument) {
  if (s21_strchr("hl", *format) != s21_NULL) {
    argument->leng = *format;
    ++format;
  }
  return format;
}

char *specifier(char *format, arguments *argument) {
  argument->spec = *format;
  ++format;
  return format;
}

char *read_int_from_str(char *format, int *value) {
  while (('0' <= *format) && (*format <= '9')) {
    *value = *value * 10 + (int)*format - (int)'0';
    format++;
  }
  return format;
}

void fill_buffer(char *buffer, va_list *value, arguments *argument) {
  switch (argument->spec) {
  case 'c':
    fill_symb(buffer, value, argument);
    break;
  case 'd':
    fill_int(buffer, value, argument);
    break;
  case 'i':
    fill_int(buffer, value, argument);
    break;
  case 'f':
    fill_float(buffer, value, argument);
    break;
  case 's':
    fill_string(buffer, value, argument);
    break;
  case 'u':
    fill_unsigned(buffer, value, argument);
    break;
  case '%':
    buffer[0] = '%';
    break;
  }
}

void fill_symb(char *buffer, va_list *value, arguments *argument) {
  if (argument->leng != 'l') {
    char n = (char)va_arg(*value, int);
    buffer[0] = n;
  } else {
    wchar_t n = va_arg(*value, wchar_t);
    wcstombs(buffer, &n, BUFFER_SIZE);
  }
}

void fill_int(char *buffer, va_list *value, arguments *argument) {
  int n = va_arg(*value, int);
  switch (argument->leng) {
  case 0:
    n = (int32_t)n;
    break;
  case 'h':
    n = (int16_t)n;
    break;
  case 'l':
    n = (int64_t)n;
    break;
  }
  num_to_string(n, buffer);

  if (argument->if_prec) {
    handle_precision(argument, buffer);
  }
  handle_flags(argument, buffer);
}

void fill_string(char *buffer, va_list *value, arguments *argument) {
  if (argument->leng != 'l') {
    char *n = va_arg(*value, char *);
    s21_strcpy(buffer, n);
  } else {
    wchar_t *n = va_arg(*value, wchar_t *);
    wcstombs(buffer, n, BUFFER_SIZE);
  }

  if (argument->prec != 0) {
    buffer[argument->prec] = '\0';
  }
}

void fill_float(char *buffer, va_list *value, arguments *argument) {
  int check_neg = 0;
  double n = va_arg(*value, double);

  if (n < 0) {
    buffer[0] = '-';
    check_neg = 1;
    n = -n;
  }

  double_to_string(n, buffer, argument, check_neg);
  handle_flags(argument, buffer);
}

void double_to_string(double val, char *buffer, arguments *argument,
                      int check_neg) {
  char *tmp_frac_val = calloc(BUFFER_SIZE, sizeof(char));
  int prec = 6;
  long int int_val_2 = 0;
  long double frac_val = 0, int_val = 0, value = val;
  frac_val = modfl(value, &int_val);

  if (argument->if_prec) {
    prec = argument->prec;
  }

  long double mem = (frac_val * (long int)pow((long int)10, prec));
  mem = roundl(mem);

  frac_val = (long int)mem;
  int_val_2 = (long int)int_val;
  reading_num_and_reversing(int_val_2, buffer, check_neg);

  if (s21_strlen(buffer) == 0) {
    buffer[0] = '0';
  }

  s21_strcpy(buffer + s21_strlen(buffer), ".");

  reading_num_and_reversing(frac_val, tmp_frac_val, 0);
  int lol = s21_strlen(tmp_frac_val);

  for (int i = 0; i < prec - lol; i++) {
    tmp_frac_val[lol + i] = '0';
  }

  s21_strcpy(buffer + s21_strlen(buffer), tmp_frac_val);
  free(tmp_frac_val);
}

void fill_unsigned(char *buffer, va_list *value, arguments *argument) {
  unsigned int n = va_arg(*value, unsigned int);

  switch (argument->leng) {
  case 0:
    n = (int32_t)n;
    break;
  case 'h':
    n = (int16_t)n;
    break;
  case 'l':
    n = (int64_t)n;
    break;
  }
  reading_num_and_reversing(n, buffer, 0);

  if (argument->prec) {
    handle_precision(argument, buffer);
  }
  handle_flags(argument, buffer);
}

void num_to_string(int val, char *buffer) {
  int check_neg = 0;
  int n = val;

  if (n < 0) {
    buffer[0] = '-';
    check_neg = 1;
    n = -n;
  }
  reading_num_and_reversing(n, buffer, check_neg);
}

void reading_num_and_reversing(int val, char *buffer, int check_neg) {
  char *tmp = calloc(BUFFER_SIZE, sizeof(char));
  char c = '?';
  int i = 0;

  while (val) {
    tmp[i] = val % 10 + '0';
    val /= 10;
    ++i;
  }

  for (int j = 0; j < i / 2; j++) {
    c = tmp[j];
    tmp[j] = tmp[i - 1 - j];
    tmp[i - 1 - j] = c;
  }

  s21_strcpy(buffer + check_neg, tmp);
  free(tmp);
}

void handle_precision(arguments *argument, char *buffer) {
  int size = s21_strlen(buffer);
  int index = 0, j = 0;
  int precision = argument->prec;
  char *tmp = calloc(size + argument->prec + 2, sizeof(char));

  if (buffer[0] == '-') {
    tmp[0] = '-';
    ++index;
    j = 1;
  }

  if (precision > size) {
    for (int i = 0; i < (precision - size + j); i++) {
      tmp[index] = '0';
      ++index;
    }

    for (int k = j; buffer[k]; k++) {
      tmp[index] = buffer[k];
      ++index;
    }
    s21_strcpy(buffer, tmp);
  }
  free(tmp);
}

void handle_flags(arguments *argument, char *buffer) {
  char *tmp = calloc(BUFFER_SIZE, sizeof(char));
  if (argument->plus && argument->spec != 'u' && argument->spec != 'c' &&
      argument->spec != 's') {
    if (buffer[0] != '-') {
      tmp[0] = '+';
      s21_strcpy(tmp + 1, buffer);
    } else {
      tmp[0] = buffer[0];
      s21_strcpy(tmp + 1, buffer + 1);
    }
    s21_strcpy(buffer, tmp);
  } else if (argument->space && buffer[0] != '-' && argument->spec != 'u') {
    tmp[0] = ' ';
    s21_strcpy(tmp + 1, buffer);
    s21_strcpy(buffer, tmp);
  }
  free(tmp);
}
