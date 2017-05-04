#include "stdafx.h"

#include <vector>

#include "VectorMath.h"
#include "WriteToCSV.h"

static const std::string separator(", ");

void WriteToCSV(std::string const& filename, Array2D<dataType>& data)
{
    FILE* file;
    fopen_s(&file, filename.c_str(), "wt");
    if (!file)
    {
        throw "File not opened";
    }

    for (int row = 0; row < data.rows(); ++row)
    {
        std::string line;

        for (auto const& value : data[row])
        {
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "%f%s", value, separator.c_str());
            buffer[_countof(buffer) - 1] = 0;
            line += buffer;
        }
        line = std::string(line.begin(), line.end() - separator.size()) + "\n";

        fprintf(file, line.c_str());
    }

    fclose(file);
}

void WriteToCSV(std::string const& filename, std::vector<Point3<dataType>> const& data)
{
    FILE* file;
    fopen_s(&file, filename.c_str(), "wt");
    if (!file)
    {
        throw "File not opened";
    }

    for (auto const& value : data)
    {
        fprintf(file, "%f%s%f%s%f\n", value.X(), separator.c_str(), value.Y(), separator.c_str(), value.Z());
    }

    fclose(file);
}

void WriteToCSV(std::string const& filename, std::vector<dataType> const& data)
{
    FILE* file;
    fopen_s(&file, filename.c_str(), "wt");
    if (!file)
    {
        throw "File not opened";
    }

    for (auto const& value : data)
    {
        fprintf(file, "%f\n", value);
    }

    fclose(file);
}

