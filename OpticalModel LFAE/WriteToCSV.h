#pragma once

#include <vector>

#include "Array2D.h"
#include "DataType.h"
#include "VectorMath.h"

void WriteToCSV(std::string const& filename, Array2D<floatType>& data);
void WriteToCSV(std::string const& filename, std::vector<Point3<floatType>> const& data);
void WriteToCSV(std::string const& filename, std::vector<floatType> const& data);