#include <iostream>

long long compos(long long compos_result, size_t layer, bool isBanned[], int** arrays, size_t cnt_of_arrs, size_t* sizes_of_arrs) {
  long long sum_result = 0;
  for (size_t i = 0; i < sizes_of_arrs[layer]; ++i) {
    if (isBanned[i]) {
      continue;
    }
    if (layer + 1 != cnt_of_arrs) {
      isBanned[i] = true;
      sum_result += compos(compos_result * arrays[layer][i], layer + 1,
                           isBanned, arrays, cnt_of_arrs, sizes_of_arrs);
      isBanned[i] = false;
    } else {
      sum_result += compos_result * arrays[layer][i];
    }
  }
  return sum_result;
}

int main(int argc, char* argv[]) {
  size_t cnt_of_arrs = argc - 1;
  size_t* sizes_of_arrs = new size_t[cnt_of_arrs];
  size_t max_size = 0;
  for (size_t i = 0; i < cnt_of_arrs; ++i) {
    sizes_of_arrs[i] = atoi(argv[i + 1]);
    if (sizes_of_arrs[i] > max_size) {
      max_size = sizes_of_arrs[i];
    }
  }

  int **arrays = new int* [cnt_of_arrs];
  for (size_t i = 0; i < cnt_of_arrs; ++i) {
    arrays[i] = new int[sizes_of_arrs[i]];
    for (size_t j = 0; j < sizes_of_arrs[i]; ++j) {
      std::cin >> arrays[i][j];
    }
  }

  bool* isBanned = new bool[max_size];
  std::fill(isBanned, isBanned + max_size, false);

  std::cout << compos(1, 0, isBanned, arrays, cnt_of_arrs, sizes_of_arrs);

  for (size_t i = 0; i < cnt_of_arrs; ++i) {
    delete[] arrays[i];
  }
  delete[] arrays;
  delete[] sizes_of_arrs;
  delete[] isBanned;
}
