#include "s21_string.h"

char *s21_strrchr(const char *str, int c) {
  char *result = s21_NULL;
  if (str != s21_NULL) {
    int i = (int)s21_strlen(str);
    while (i >= 0) {
      if (str[i] == (char)c) {
        result = ((char *)(str + i));
        i = 0;
      }
      --i;
    }
  }
  return result;
}
