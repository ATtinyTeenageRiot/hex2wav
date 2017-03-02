#include <iostream>
#include <stdio.h>
#include <string.h>
#include "v7.h"


class Test {
public:
    Test()
    {
        std::cout << "hello\n";
    }

    void ok()
    {
        std::cout << "hello\n";

    }
};

static double sum(double a, double b) {
  return a + b;
}

static enum v7_err js_sum(struct v7 *v7, v7_val_t *res) {
  double arg0 = v7_get_double(v7, v7_arg(v7, 0));
  double arg1 = v7_get_double(v7, v7_arg(v7, 1));
  double result = sum(arg0, arg1);

  *res = v7_mk_number(v7, result);
  return V7_OK;
}

int v7_example(void) {
  enum v7_err rcode = V7_OK;
  struct v7 *v7 = v7_create();
  v7_val_t result;
  v7_set_method(v7, v7_get_global(v7), "sum", &js_sum);

  rcode = v7_exec_file(v7, "test.js", &result);

  if (rcode != V7_OK) {
    v7_print_error(stderr, v7, "Evaluation error", result);
  }

  v7_destroy(v7);
  return (int) rcode;
}

int main(void) {
  Test test;
  test.ok();
  return v7_example();
}
