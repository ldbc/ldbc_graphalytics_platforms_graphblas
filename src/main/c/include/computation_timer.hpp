#include <utility>

//
// Created by baprof on 4/25/19.
//

#pragma once

#include <iostream>
#include <chrono>
#include <ratio>

#include "utils.h"

using namespace std::chrono;

class ComputationTimer {
  const int level = 0;
  const std::string name;
  const high_resolution_clock::time_point start_time;

 public:
  explicit ComputationTimer(std::string name, int level = 0) :
      level(level),
      name(std::move(name)),
      start_time(high_resolution_clock::now()) {

      for (int i = 0; i < this->level; i++) {
          std::cout << '\t';
      }
      std::cout << this->name << " starts" << std::endl;
  };

  ComputationTimer(std::string name, const ComputationTimer& parent) :
    ComputationTimer(std::move(name), parent.level + 1) { }

  ~ComputationTimer() {
      high_resolution_clock::time_point end_time = high_resolution_clock::now();
      duration<double> duration = round<milliseconds>(end_time - start_time);

      for (int i = 0; i < this->level; i++) {
          std::cout << '\t';
      }

      std::cout
        << this->name
        << " duration: "
        << duration.count() << "s"
        << std::endl;
  }

};