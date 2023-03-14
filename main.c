#include "s21_string.h"

int main() {
  char *format = "|%ld|";
  int val = 611516;
  char str1[100];
  char str2[100];

  s21_sprintf(str1, format, val);
  sprintf(str2, format, val);
  printf("%s\n", str1);
  printf("%s\n", str2);
}