#!/usr/bin/python3
import numpy as np
import sys
import os
import matplotlib.pyplot as plt

test_types = ["native", "nativesimd", "wasm", "wasmsimd"]
MIN = 0.0
MAX = 20.0

def get_improvement(hi, lo, performance):
  """
  Get improvement from two execution speeds.
  Arguments:
    hi, lo: (double) execution speeds
    performance: (bool) True if calculating performance increase, False if calculating time reduction.
  Returns: (double) Calculated improvement
  """

  return 100 * (hi - lo) / (lo if performance else hi)

def generate_data():
  data = {}
  input_filenames = os.listdir("inputs")
  for f in input_filenames:
    data[f] = {}
    for t in test_types:
      results_file = "results/" + f + "_" + t + ".csv"
      raw_data = np.genfromtxt(results_file,delimiter='\n')
      data[f][t] = (np.mean(raw_data), raw_data)

  return data


def generate_txt(data):
  """
  Generate output .txt file
  Arguments:
    data = {filename: (average execution time, [raw data])}
  """
  for f in data.keys():
    native_comp_p = get_improvement(data[f]["native"][0], data[f]["nativesimd"][0], True)
    wasm2c_comp_p = get_improvement(data[f]["wasm"][0], data[f]["wasmsimd"][0], True)

    simd2c_comp_p = get_improvement(data[f]["wasmsimd"][0], data[f]["nativesimd"][0], True)
    no_simd2c_comp_p = get_improvement(data[f]["wasm"][0], data[f]["native"][0], True)

    native_comp_t = get_improvement(data[f]["native"][0], data[f]["nativesimd"][0], False)
    wasm2c_comp_t = get_improvement(data[f]["wasm"][0], data[f]["wasmsimd"][0], False)

    simd2c_comp_t = get_improvement(data[f]["wasmsimd"][0], data[f]["nativesimd"][0], False)
    no_simd2c_comp_t = get_improvement(data[f]["wasm"][0], data[f]["native"][0], False)

    results_for_f = "results/" + f + "_analsysis_comparitive_results.txt"
    with open(results_for_f, 'w') as compout:
      compout.write("averages: \n")
      compout.write("  native without sse2  : " + str(data[f]["native"][0]) + "\n")
      compout.write("  native with sse2     : " + str(data[f]["nativesimd"][0]) + "\n")
      compout.write("  wasm2c without simd128    : " + str(data[f]["wasm"][0]) + "\n")
      compout.write("  wasm2c with simd128    : " + str(data[f]["wasmsimd"][0]) + "\n")
      compout.write("\n")
      compout.write("performance increase percentage [100 * (hi - lo) / lo]: \n")
      compout.write("  native without sse2 -> native with sse2     : " + str(native_comp_p) + "\n")
      compout.write("  wasm2c without simd128 -> wasm2c with simd128   : " + str(wasm2c_comp_p) + "\n")
      compout.write("  wasm2c with simd128 -> native with sse2       : " + str(simd2c_comp_p) + "\n")
      compout.write("  wasm2c without simd128 -> native without sse2 : " + str(no_simd2c_comp_p) + "\n")
      compout.write("\n")
      compout.write("time reduction percentage [100 * (hi - lo) / hi]: \n")
      compout.write("  native without sse2 -> native with sse2     : " + str(native_comp_t) + "\n")
      compout.write("  wasm2c without simd128 -> wasm with simd128   : " + str(wasm2c_comp_t) + "\n")
      compout.write("  wasm2c with simd128 -> native with sse2       : " + str(simd2c_comp_t) + "\n")
      compout.write("  wasm2c without simd128 -> native without sse2 : " + str(no_simd2c_comp_t) + "\n")


def generate_plt(data):
  """
  Generate output histogram .png file
  Arguments:
    data = {filename: (average execution time, [raw data])}
  """
  for f in data.keys():
    bins = np.arange(MIN, MAX, 0.01)
    plt.hist(data[f]["native"][1], bins, alpha=0.5, label="Native", edgecolor="black")
    plt.hist(data[f]["nativesimd"][1], bins, alpha=0.5, label="Native SIMD", edgecolor="black")
    plt.hist(data[f]["wasm"][1], bins, alpha=0.5, label="WASM", edgecolor="black")
    plt.hist(data[f]["wasmsimd"][1], bins, alpha=0.5, label="WASMSIMD", edgecolor="black")
    
    plt.xlabel("Time [s]")
    plt.ylabel("Frequency")
    plt.legend(loc='upper left', fontsize='small')
    plt.title("Comparison of Decoding Speeds")
    plt_file_name = "results/" + f + "_analysis_comparison.png"
    plt.savefig(plt_file_name)
    plt.close()


def generate_bar(data):
  """
  Generate output bar chart .png file
  Arguments:
    data = {filename: (average execution time, [raw data])}
  """
  for f in data.keys():
    x_axis = ["Native", "Native SIMD", "WASM", "WASMSIMD"]
    y_axis = np.zeros(len(data[f]))
    for i in range(len(data[f])):
      y_axis[i + 1 if i % 2 == 0 else i - 1] = list(data[f].values())[i][0]
    print(x_axis)
    print(y_axis)
    plt.figure(figsize=(10,6))
    plt.bar(x_axis, y_axis)
    for i in range(len(x_axis)):
      plt.text(i, y_axis[i], y_axis[i])
    plt.xlabel('Implementation')
    plt.ylabel('Time [s]')
    plt.title("Comparison of Decoding Speeds")
    plt_file_name = "results/" + f + "_analysis_bar_chart.png"
    plt.savefig(plt_file_name)
    plt.close()
    

def main():
  data = generate_data()
  generate_txt(data)
  generate_plt(data)
  #plt.close()
  #generate_bar(data)


if __name__ == "__main__":
  main()