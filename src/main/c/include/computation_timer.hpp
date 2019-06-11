//
// Created by baprof on 4/25/19.
//

#pragma once

#include <iostream>
#include <chrono>

#include "utils.h"

using namespace std::chrono;

class ComputationTimer {
  const std::string name;
  const high_resolution_clock::time_point start_time;

 public:
  explicit ComputationTimer(const std::string& name) :
      name(name),
      start_time(high_resolution_clock::now()) {
      std::cout << this->name << " starts" << std::endl;
  };

  ~ComputationTimer() {
      high_resolution_clock::time_point end_time = high_resolution_clock::now();
      time_t duration = duration_cast<milliseconds>(end_time - start_time).count();

      std::cout << this->name << " duration: " << (duration / 1000.0) << "s" << std::endl;
      std::cout << std::endl;
  }

};