#pragma once

#include <vector>
#include <functional>

#include "Walnut/Image.h"

class Generator {
public:
  void generate_TAM(int level);
private:
  std::vector <std::shared_ptr<Walnut::Image>> tam;
};