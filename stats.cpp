#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

std::vector<float> quantiles(const std::vector<float>& data,
                             const std::vector<float>& probabilities) {
  if (data.empty()) return std::vector<float>();

  if (data.size() == 1) return std::vector<float>(1, data[0]);

  std::vector<float> quantiles;
  auto N = data.size();

  for (float proba : probabilities) {
    size_t index = std::ceil(proba * N);
    quantiles.push_back(data.at(index));
  }

  return quantiles;
}

struct statRecord {
  float mean, variance, stdev, min, max;
  float total, total_squared;
  int count;
  float median, q1, q3;
  bool print_quartiles;
  std::vector<float> elements;

  statRecord()
      : mean(0),
        variance(0),
        stdev(0),
        min(-1),
        max(0),
        total(0),
        total_squared(0),
        count(0),
        median(0),
        q1(0),
        q3(0),
        print_quartiles(false) {}

  statRecord(bool qs)
      : mean(0),
        variance(0),
        stdev(0),
        min(-1),
        max(0),
        total(0),
        total_squared(0),
        count(0),
        median(0),
        q1(0),
        q3(0),
        print_quartiles(qs) {}

  void print_obj() {
    std::cerr << "mean:          " << mean << "\n"
              << "variance:      " << variance << "\n"
              << "stdev:         " << stdev << "\n"
              << "min:           " << min << "\n"
              << "max:           " << max << "\n"
              << "total:         " << total << "\n"
              << "total_squared: " << total_squared << "\n"
              << "count:         " << count << "\n"
              << "median:        " << median << "\n"
              << "q1:            " << q1 << "\n"
              << "q3:            " << q3 << "\n"
              << "print_qs:      " << print_quartiles << "\n"
              << std::endl;
  }

  void process_val(float val) {
    if (val > max) {
      max = val;
    }
    if (val < min || min < 0) {
      min = val;
    }
    if (print_quartiles) elements.push_back(val);
    total += val;
    total_squared += std::pow(val, 2.0);
    ++count;
  }

  void compute_mean() { mean = total / count; }

  void compute_variance() {
    variance = (total_squared / count) - std::pow(mean, 2);
    stdev = std::sqrt(variance);
  }

  void compute_quartiles() {
    std::sort(elements.begin(), elements.end());
    std::vector<float> vals = std::vector<float>{0.25, 0.5, 0.75};
    auto quartiles = quantiles(elements, vals);
    q1 = quartiles[0];
    median = quartiles[1];
    q3 = quartiles[2];
  }

  void print_human() {
    std::cout << "Mean:     " << mean << "\n"
              << "Variance: " << variance << " (" << stdev << " SD)\n"
              << "Min:      " << min << "\n"
              << "Max:      " << max << "\n"
              << "Count:    " << count;

    if (print_quartiles) {
      std::cout << "\n"
                << "Q1:       " << q1 << "\n"
                << "Median:   " << median << "\n"
                << "Q3:       " << q3;
    }

    std::cout << std::endl;
  }

  void print_json() {
    std::cout << "{\n"
              << "\t\"mean\": " << mean << ",\n"
              << "\t\"variance\": " << variance << ",\n"
              << "\t\"stdev\": " << stdev << ",\n"
              << "\t\"min\": " << min << ",\n"
              << "\t\"max\": " << max << ",\n"
              << "\t\"count\": " << count;
    if (print_quartiles) {
      std::cout << ",\n"
                << "\t\"q1\": " << q1 << ",\n"
                << "\t\"median\": " << median << ",\n"
                << "\t\"q3\": " << q3;
    }
    std::cout << "\n}" << std::endl;
  }
};

void usage(std::string name) {
  std::cerr
      << "Stats is meant to compute basic statistics on an input stream of "
         "numbers.\n"
      << "It will output the mean, variance, min, max, count and optionally "
         "the median and quartiles.\n "
      << "\n"
      << "Usage:\t stream | " << name << " [options]\n"
      << "\n"
      << "Options:\n"
      << "\t-q,--quartiles     compute median and quartiles (requires keeping\n"
      << "\t                   all numbers in memory so it might not work on\n"
      << "\t                   large streams.)\n"
      << "\t-j,--json          output statistics in JSON format.\n"
      << "\t-s,--skip-header   do not consider first line of input.\n"
      << "\t-h,--help          show this message\n"
      << "\n"
      << "\n"
      << "This program only works on a single column so if you are working "
         "with a csv file use 'cut'\n"
      << "To select the appropriate column and pipe the output to 'stats'.\n\n"
      << "Example: \n"
      << "\tComputing basic statistics on the second column of a csv:\n"
      << "\t cut -d ',' -f 2 'somefile.csv' | stats" << std::endl;
}

int main(int argc, char* argv[]) {
  std::string name = argv[0];
  bool compute_quartiles = false;
  bool print_json = false;
  bool skip_header = false;

  // Parsing command line arguments
  if (argc > 3) {
    usage(name);
    return 1;
  }
  for (int i = 1; i < argc; ++i) {
    std::string opt = argv[i];
    if ((opt == "-h") || (opt == "--help")) {
      usage(name);
      return 0;
    } else if ((opt == "-q") || (opt == "--quartiles")) {
      compute_quartiles = true;
    } else if ((opt == "-j") || (opt == "--json")) {
      print_json = true;
    } else if ((opt == "-s") || (opt == "--skip-header")) {
      skip_header = true;
    } else {
      std::cerr << "Unkown option: " << opt << "\n" << std::endl;
      usage(name);
      return 1;
    }
  }

  statRecord record = statRecord(compute_quartiles);

  std::string line;
  if (skip_header) std::getline(std::cin, line);

  while (std::getline(std::cin, line)) {
    float val = std::stof(line);
    record.process_val(val);
  }

  record.compute_mean();
  record.compute_variance();

  if (compute_quartiles) record.compute_quartiles();

  print_json ? record.print_json() : record.print_human();

  return 0;
}
