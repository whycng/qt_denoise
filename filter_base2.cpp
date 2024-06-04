#include "filter_base2.h"

#include "qdebug.h"
#include "qtextcodec.h"
#include "wavelet.h"
using namespace Vector;





//读取las文件
std::vector<std::vector<float>> readLasFile(const std::string& filePath)
{
    std::vector<std::vector<float>> data;
    std::string line;

    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening  file!" << std::endl;
        return data;
    }

    std::getline(file, line);//跳过首行
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<float> row;
        float value;

        // read depth (first column)
        iss >> value;
        //row.push_back(value); //深度值，暂时不需要

        // read data body (remaining columns)
        while (iss >> value) {
            row.push_back(value);
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

//存储文件
void saveFile(const std::string& filePath, const std::vector<std::vector<float>>& data) {
    std::ofstream outputFile(filePath); // replace with your output file name
    if (!outputFile) {
        std::cerr << "Error opening test output file!" << std::endl;
        return ;
    }
    // print the data
    for (const auto& row : data) {
        for (const auto& value : row) {
            //std::cout << value << " ";
            outputFile << value << " ";
        }
        //std::cout << std::endl;
        outputFile << std::endl;
    }
    outputFile.close();

}


/*高斯滤波器是一种线性滤波器，它使用高斯分布对相邻像素进行加权。 该滤波器将输入图像与高斯核进行卷积，
高斯核是遵循高斯分布的二维权重数组。 滤波器的输出是相邻像素的加权平均值。*/
void gaussianFilter(float* input, float* output, int width, int height,
                    const int& kernelSize) {
    int kernelRadius = kernelSize / 2;
    float* kernel = new float[kernelSize * kernelSize];

    // Calculate the Gaussian kernel
    for (int i = -kernelRadius; i <= kernelRadius; i++) {
        for (int j = -kernelRadius; j <= kernelRadius; j++) {
            float distance = sqrt(i * i + j * j);
            kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)] = exp(-distance * distance / (2 * kernelRadius * kernelRadius));
        }
    }

    // Normalize the kernel
    float sum = 0;
    for (int i = 0; i < kernelSize * kernelSize; i++) {
        sum += kernel[i];
    }
    for (int i = 0; i < kernelSize * kernelSize; i++) {
        kernel[i] /= sum;
    }

    // Convolve the input image with the kernel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0;
            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int neighborX = x + i;
                    int neighborY = y + j;
                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
                        sum += input[neighborY * width + neighborX] * kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
                    }
                }
            }
            output[y * width + x] = sum;
        }
    }

    delete[] kernel;
}


//调试1
void gaussianFilter(const std::vector<std::vector<float>>& input,
                    std::vector<std::vector<float>>& output,
                    const int& kernelSize) {
    int width = input[0].size();
    int height = input.size();
    int kernelRadius = kernelSize / 2;
    std::vector<float> kernel(kernelSize * kernelSize);

    // Calculate the Gaussian kernel
    float sigma = kernelRadius / 2.0;  // Standard deviation
    float sum = 0.0;
    for (int i = -kernelRadius; i <= kernelRadius; i++) {
        for (int j = -kernelRadius; j <= kernelRadius; j++) {
            float distance = sqrt(i * i + j * j);
            kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)] = exp(-distance * distance / (2 * sigma * sigma));
            sum += kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
        }
    }

    // Normalize the kernel
    for (float& val : kernel) {
        val /= sum;
    }

    // Print the kernel for debugging
//    std::cout << "Gaussian Kernel:" << std::endl;
//    for (int i = 0; i < kernelSize; ++i) {
//        for (int j = 0; j < kernelSize; ++j) {
//            std::cout << kernel[i * kernelSize + j] << " ";
//        }
//        std::cout << std::endl;
//    }

    // Prepare output with the same size as input
    output.resize(height, std::vector<float>(width, 0.0f));

    // Convolve the input image with the kernel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float sum = 0;
            float weightSum = 0;  // To normalize the sum at the borders
            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int neighborX = x + j;
                    int neighborY = y + i;
                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
                        sum += input[neighborY][neighborX] * kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
                        weightSum += kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
                    }
                }
            }
            output[y][x] = sum / weightSum;  // Normalize to avoid darkening at borders
        }
    }
}

//修正1
//void gaussianFilter(const std::vector<std::vector<float>>& input,
//                    std::vector<std::vector<float>>& output,
//                    const int& kernelSize) {
//    int width = input[0].size();
//    int height = input.size();
//    int kernelRadius = kernelSize / 2;
//    std::vector<float> kernel(kernelSize * kernelSize);

//    // Calculate the Gaussian kernel
//    for (int i = -kernelRadius; i <= kernelRadius; i++) {
//        for (int j = -kernelRadius; j <= kernelRadius; j++) {
//            float distance = sqrt(i * i + j * j);
//            kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)] = exp(-distance * distance / (2 * kernelRadius * kernelRadius));
//        }
//    }

//    // Normalize the kernel
//    float sum = 0;
//    for (float val : kernel) {
//        sum += val;
//    }
//    for (float& val : kernel) {
//        val /= sum;
//    }

//    // Prepare output with the same size as input
//    output.resize(height, std::vector<float>(width, 0.0f));

//    // Convolve the input image with the kernel
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            float sum = 0;
//            float weightSum = 0;  // To normalize the sum at the borders
//            for (int i = -kernelRadius; i <= kernelRadius; i++) {
//                for (int j = -kernelRadius; j <= kernelRadius; j++) {
//                    int neighborX = x + j;
//                    int neighborY = y + i;
//                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
//                        sum += input[neighborY][neighborX] * kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
//                        weightSum += kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
//                    }
//                }
//            }
//            output[y][x] = sum / weightSum;  // Normalize to avoid darkening at borders
//            qDebug() << " output[y][x]:" << output[y][x];
//        }
//    }
//}


//原版高斯滤波
//void gaussianFilter(const std::vector<std::vector<float>>& input,
//                    std::vector<std::vector<float>>& output,
//                    const int& kernelSize) {
//    int width = input[0].size();
//    int height = input.size();
//    int kernelRadius = kernelSize / 2;
//    std::vector<float> kernel(kernelSize * kernelSize);

//    // Calculate the Gaussian kernel
//    for (int i = -kernelRadius; i <= kernelRadius; i++) {
//        for (int j = -kernelRadius; j <= kernelRadius; j++) {
//            float distance = sqrt(i * i + j * j);
//            kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)] = exp(-distance * distance / (2 * kernelRadius * kernelRadius));
//        }
//    }

//    // Normalize the kernel
//    float sum = 0;
//    for (float val : kernel) {
//        sum += val;
//    }
//    for (float& val : kernel) {
//        val /= sum;
//    }

//    // Convolve the input image with the kernel
//    output.resize(height, std::vector<float>(width));
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            float sum = 0;
//            for (int i = -kernelRadius; i <= kernelRadius; i++) {
//                for (int j = -kernelRadius; j <= kernelRadius; j++) {
//                    int neighborX = x + i;
//                    int neighborY = y + j;
//                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
//                        sum += input[neighborY][neighborX] * kernel[(i + kernelRadius) * kernelSize + (j + kernelRadius)];
//                    }
//                }
//            }
//            output[y][x] = sum;
//        }
//    }
//}

//中值滤波
void medianFilter(float* input, float* output, int width, int height, int kernelSize) {
    int kernelRadius = kernelSize / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::vector<float> neighbors;

            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int neighborX = x + i;
                    int neighborY = y + j;
                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
                        neighbors.push_back(input[neighborY * width + neighborX]);
                    }
                }
            }

            std::sort(neighbors.begin(), neighbors.end());
            output[y * width + x] = neighbors[neighbors.size() / 2];
        }
    }
}

//中值滤波
void medianFilter(const std::vector<std::vector<float>>& input,
                  std::vector<std::vector<float>>& output, int kernelSize) {
    int width = input[0].size();
    int height = input.size();
    int kernelRadius = kernelSize / 2; //滤波器核的半径

    output.resize(height, std::vector<float>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::vector<float> neighbors;
            //遍历邻域像素，将这些像素的值放入向量中
            for (int i = -kernelRadius; i <= kernelRadius; i++) {
                for (int j = -kernelRadius; j <= kernelRadius; j++) {
                    int neighborX = x + i;
                    int neighborY = y + j;
                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
                        neighbors.push_back(input[neighborY][neighborX]);
                    }
                }
            }
            //计算中值
            std::sort(neighbors.begin(), neighbors.end());
            output[y][x] = neighbors[neighbors.size() / 2];
        }
    }
}

/*维纳滤波器是一种线性滤波器，它使用最小均方误差 (MMSE) 方法从噪声观测中估计原始信号。
该滤波器对于去除加性高斯白噪声非常有用*/
void wienerFilter(float* input, float* output, int width,
                  int height, float noiseVariance) {
    float signalVariance = 1.0; // assume the signal variance is 1.0

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float signalEstimate = input[y * width + x];
            float noiseEstimate = noiseVariance;
            float wienerCoefficient = signalVariance / (signalVariance + noiseEstimate);
            output[y * width + x] = wienerCoefficient * signalEstimate;
        }
    }
}

void wienerFilter(const std::vector<std::vector<float>>& input,
                  std::vector<std::vector<float>>& output, float noiseVariance) {
    int width = input[0].size();
    int height = input.size();
    float signalVariance = 1.0; // assume the signal variance is 1.0

    output.resize(height, std::vector<float>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float signalEstimate = input[y][x];
            float noiseEstimate = noiseVariance;
            float wienerCoefficient = signalVariance / (signalVariance + noiseEstimate);
            output[y][x] = wienerCoefficient * signalEstimate;
        }
    }
}


//滑动平均法滤波 --TODO:
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output) {
    // 获取行数和列数
    int rows = input.size();
    if (rows == 0) return;
    int cols = input[0].size();

    // 初始化输出矩阵，和输入矩阵大小相同
    output.resize(rows, std::vector<float>(cols, 0));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = 0;
            int count = 0;

            // 遍历周围的 3x3 网格，包括自身
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    // 检查边界条件
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        sum += input[ni][nj];
                        count++;
                    }
                }
            }

            // 计算平均数并存储到输出矩阵中
            output[i][j] = sum / count;
        }
    }
}
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output, int n) {
    // 获取行数和列数
    int rows = input.size();
    if (rows == 0) return;
    int cols = input[0].size();

    // 初始化输出矩阵，和输入矩阵大小相同
    output.resize(rows, std::vector<float>(cols, 0));

    // 计算滤波器窗口半径
    int radius = (n - 1) / 2;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = 0;
            int count = 0;

            // 遍历周围的 n*n 网格，包括自身
            for (int di = -radius; di <= radius; ++di) {
                for (int dj = -radius; dj <= radius; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    // 检查边界条件
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        sum += input[ni][nj];
                        count++;
                    }
                }
            }

            // 计算平均数并存储到输出矩阵中
            output[i][j] = sum / count;
        }
    }
}


// 函数接受一个 std::vector<float> 输入，窗口大小，并返回含有移动平均的 vector
std::vector<float> movmean(const std::vector<float>& x, int window) {
    std::vector<float> result;
    int n = x.size();
    result.resize(n, 0.0f);

    // 初始化滑动窗口元素之和
    float sum = 0.0;
    int start = 0, end = 0;

    // 设定窗口初始位置
    for (end = 0; end < n; ++end) {
        sum += x[end];

        if (end - start + 1 == window) { // 当窗口大小达到设定值
            result[start + (window - 1) / 2] = sum / window; // 将平均值放在窗口的中间位置
            sum -= x[start]; // 移除窗口最前面的元素
            start++; // 窗口滑动
        }
    }

    // 针对开头和结尾位置的不完整窗口进行处理
    for (int i = 0; i < (window - 1) / 2; ++i) {
        result[i] = sum / (i + 1 + (window - 1) / 2);
        result[n - 1 - i] = sum / (i + 1 + (window - 1) / 2);
        sum -= x[start];
        sum -= x[n - 1 - start];
        start++;
    }

    return result;
}




//零相位滤波功能
// 基本的一维滤波器函数（离散卷积）
void filter(const std::vector<float>& b, const std::vector<float>& a,
            const std::vector<float>& x, std::vector<float>& y) {
    int n = x.size();
    int filterOrder = b.size();
    y.resize(n, 0.0);

    for (int i = 0; i < n; ++i) {
        // 计算卷积
        for (int j = 0; j < filterOrder; j++) {
            if (i - j >= 0) { // 检查索引不要越界
                y[i] += b[j] * x[i - j];
            }
        }
    }
}

// 实现零相位滤波功能
void zeroPhaseFilter(const std::vector<float>& x, std::vector<float>& y,
                     const std::vector<float>& b) {
    std::vector<float> temp;
    std::vector<float> reversed_x = x;
    std::reverse(reversed_x.begin(), reversed_x.end());

    // 正向滤波
    filter(b, { 1.0 }, x, temp);

    std::vector<float> reversed_temp = temp;
    std::reverse(reversed_temp.begin(), reversed_temp.end());

    // 反向滤波
    filter(b, { 1.0 }, reversed_temp, y);
    std::reverse(y.begin(), y.end());
}
// 二维零相位滤波器
void zeroPhaseFilter2D(const std::vector<std::vector<float>>& input,
                       std::vector<std::vector<float>>& output,
                       const std::vector<float>& b) {
    int height = input.size();
    int width = input[0].size();

    // 临时存储中间结果
    std::vector<std::vector<float>> temp(height, std::vector<float>(width));

    // 对每一行进行零相位滤波
    for (int i = 0; i < height; ++i) {
        zeroPhaseFilter(input[i], temp[i], b);
    }

    // 转置矩阵
    std::vector<std::vector<float>> transposedTemp(width, std::vector<float>(height));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            transposedTemp[j][i] = temp[i][j];
        }
    }

    // 对每一列（转置后的行）进行零相位滤波
    std::vector<std::vector<float>> transposedOutput(width, std::vector<float>(height));
    for (int j = 0; j < width; ++j) {
        zeroPhaseFilter(transposedTemp[j], transposedOutput[j], b);
    }

    // 再次转置回原来的形状
    output.resize(height, std::vector<float>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            output[i][j] = transposedOutput[j][i];
        }
    }
}



//Savitzky-Golay法滤波
std::vector<float> computeSGCoefficients(int windowSize, int polyOrder) {
    // This is a placeholder. In practice, you'd calculate these or use a function to generate them.
    // Here are example coefficients for a 5-point quadratic fit (polyOrder = 2).
    if (windowSize == 5 && polyOrder == 2) {
        return { -3.0 / 35, 12.0 / 35, 17.0 / 35, 12.0 / 35, -3.0 / 35 };
    }
    else {
        // Return empty for unsupported configurations
        return {};
    }
}
// Savitzky-Golay法a[0] -> 1/35 (-3 x[-2] + 12 x[-1] + 17 x[0] + 12 x[1] - 3 x[2])
void applySavitzkyGolay(const std::vector<std::vector<float>>& input,
                        std::vector<std::vector<float>>& output,
                        int windowSize, int polyOrder) {
    int rows = input.size();
    if (rows == 0) return;
    int cols = input[0].size();
    output.resize(rows, std::vector<float>(cols, 0.0f));

    // Get the coefficients
    std::vector<float> coeffs = computeSGCoefficients(windowSize, polyOrder);
    if (coeffs.empty()) return;  // Handle unsupported window size or order

    int halfWindow = windowSize / 2;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float sum = 0.0;
            for (int k = -halfWindow; k <= halfWindow; ++k) {
                int colIndex = j + k;
                if (colIndex >= 0 && colIndex < cols) {
                    sum += input[i][colIndex] * coeffs[k + halfWindow];
                }
            }
            output[i][j] = sum;
        }
    }
}




// Helper functions
float median(std::vector<float> v) {
    size_t n = v.size() / 2;
    nth_element(v.begin(), v.begin() + n, v.end());
    if (v.size() % 2 == 1)
        return v[n];
    else
        return (v[n - 1] + v[n]) / 2.0;
}

// MAD-based outlier detection and replace with median
void madFilter(const std::vector<std::vector<float>>& input, std::vector<std::vector<float>>& output) {
    int rows = input.size();
    if (rows == 0) return;
    int cols = input[0].size();

    output.resize(rows);
    std::vector<float> colData(rows), absDeviations(rows);

    for (int col = 0; col < cols; ++col) {
        // Extract column into a vector
        for (int row = 0; row < rows; ++row) {
            colData[row] = input[row][col];
            output[row].resize(cols);
        }

        float colMedian = median(colData);
        // Compute absolute deviations from median
        for (int i = 0; i < rows; ++i) {
            absDeviations[i] = fabs(colData[i] - colMedian);
        }

        float mad = median(absDeviations);
        float threshold = 3 * mad;

        // Filter outliers
        for (int i = 0; i < rows; ++i) {
            if (absDeviations[i] > threshold) {
                output[i][col] = colMedian; // Replace outlier with column median
            }
            else {
                output[i][col] = input[i][col];
            }
        }
    }
}





// 将 const std::vector<std::vector<float>>& data 转换为 std::vector<std::vector<double>>
std::vector<std::vector<double>> convertData(const std::vector<std::vector<float>>& data) {
    std::vector<std::vector<double>> data_double(data.size(), std::vector<double>(data[0].size()));
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[i].size(); ++j) {
            data_double[i][j] = static_cast<double>(data[i][j]);
        }
    }
    return data_double;
}

// 假设有一个 Matrix<double> 类型的对象 matrix_rec
std::vector<std::vector<float>> ConvertMatrixTo2DVector(const Matrix<double>& matrix_rec) {
    size_t num_rows = matrix_rec.GetNumRows();
    size_t num_cols = matrix_rec.GetNumColumns();
    std::vector<std::vector<float>> val(num_rows, std::vector<float>(num_cols));

    for (size_t i = 0; i < num_cols; ++i) {
        for (size_t j = 0; j < num_rows; ++j) {
            val[j][i] = float(matrix_rec(j, i));
        }
    }

    return val;
}

//小波3
void wavelet3(const std::vector<std::vector<float>>& data,
              std::vector<std::vector<float>>& outData)
{
    std::cout << "data row:" << data.size() << " col:" << data.at(0).size() << std::endl;
    std::ofstream fileOri2Ddata("wletOri2Ddata.txt");
    if (!fileOri2Ddata) {
        std::cerr << "Error opening fileOri2Ddata file!" << std::endl;
        return;
    }


    const size_t num_rows = static_cast<size_t>(data.size());
    const size_t num_cols = static_cast<size_t>(data.at(0).size());
    // Number of decomposition levels
    const size_t num_levels = static_cast<size_t>(4);

    std::vector<double> data1d;
    for (size_t i = 0; i <  num_cols; ++i) { //考虑到矩阵是按列存储的！！
        for (size_t j = 0; j < num_rows; ++j) {
            data1d.push_back(static_cast<double>(data.at(j).at(i)));
            fileOri2Ddata << data.at(j).at(i) << " ";
        }
        fileOri2Ddata << std::endl;
    }
    fileOri2Ddata.close();

    std::cout << "data1d.size:" << data1d.size()
              << " num_rows:" << num_rows << " num_cols:" << num_cols
              << " " << num_rows * num_cols << std::endl;

    Matrix<double> matrix(  num_rows, num_cols, data1d); //反过来

    //std::cout << "  TEST " << std::endl;
    //Matrix<double> matrix(60, 60);
    //matrix.Rand(0);

    /*测试-保存数据
    std::ofstream fileOri2D("wletOri2D.txt");
    if (!fileOri2D) {
        std::cerr << "Error opening fileOri2D file!" << std::endl;
        return;
    }
    for (size_t i = 0; i < matrix.GetNumColumns(); ++i) {
        for (size_t j = 0; j < matrix.GetNumRows(); ++j) {
            //std::cout << matrix(j, i) << " " << std::endl;
            fileOri2D << matrix(j, i) << " ";
        }
        fileOri2D << std::endl;
    }
    fileOri2D.close();
    */

    const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    const std::vector<double> Lo_D = { inv_sqrt2, inv_sqrt2 };
    const std::vector<double> Hi_D = { -inv_sqrt2, inv_sqrt2 };
    const std::vector<double> Lo_R = { inv_sqrt2, inv_sqrt2 };
    const std::vector<double> Hi_R = { inv_sqrt2, -inv_sqrt2 };

    // Instantiate a Wavelet object with Haar filters
    const Wavelet<double> haar(Lo_D, Hi_D, Lo_R, Hi_R);


    // Example of 2D decomposition and reconstruction
    Decomposition2D<double> dec2D = haar.Wavedec(matrix, num_levels);

    // 应用阈值去噪
    // 设定二维去噪的阈值
    double threshold2D = 0.2;

    // 对每个子带应用阈值去噪
    for (int level = 0; level < dec2D.NumLevels(); ++level) {
        for (int subband = 0; subband < 3; ++subband) {
            Matrix<double> detail = dec2D.GetDetcoef(static_cast<WaveledSubbdand>(subband), level);

            // 软阈值处理
            for (auto& coeff : detail.GetData()) {
                coeff = (fabs(coeff) > threshold2D) ? coeff - copysign(threshold2D, coeff) : 0.0;
            }

            dec2D.SetDetcoef(detail, static_cast<WaveledSubbdand>(subband), level);
        }
    }

    const Matrix<double> matrix_rec = haar.Waverec(dec2D, matrix.Size());
    const double norm2D = Norm((matrix - matrix_rec).GetData());

    outData = ConvertMatrixTo2DVector(matrix_rec);

    /* 测试-写出
    std::ofstream fileOut2D("wletOut2D.txt");
    if (!fileOut2D) {
        std::cerr << "Error opening fileOut2D file!" << std::endl;
        return;
    }

    // 遍历矩阵并写入文本文件
    for (size_t i = 0; i < matrix_rec.GetNumColumns(); ++i) {
        for (size_t j = 0; j < matrix_rec.GetNumRows(); ++j) {
            double tpVal = matrix_rec(j, i) - matrix(j, i);
            //if(tpVal != 0)
            //std::cout << " " << tpVal << " matrix:" << matrix(j, i) << " matrix_rec:" << matrix_rec(j, i)
            //    << std::endl;;
            fileOut2D << matrix_rec(j, i) << " ";
        }
        fileOut2D << std::endl;
    }
    fileOut2D.close();
    */

    std::cout << "<完成>Error (l2-norm) of 2D reconstruction: " << norm2D << std::endl;

}

void normalize(std::vector<std::vector<float>>& data) {
    // 将二维向量拉平到一维向量
    std::vector<float> flat_data;
    for (const auto& vec : data) {
        flat_data.insert(flat_data.end(), vec.begin(), vec.end());
    }

    // 找到最小值和最大值
    float min_val = *min_element(flat_data.begin(), flat_data.end());
    float max_val = *max_element(flat_data.begin(), flat_data.end());

    // 归一化所有元素
    for (auto& vec : data) {
        for (float& val : vec) {
            val = (val - min_val) / (max_val - min_val);
        }
    }
}

//基于阈值赋0
void thresholdFilter(std::vector<std::vector<float>>& data ,
                     float threshold1, float threshold2) { //threshold 上阈值
    // 遍历图像像素
    for (int y = 0; y < data.size(); y++) {
        for (int x = 0; x < data[y].size(); x++) {
            // 如果像素值大于阈值，则设置为 0
            if (data[y][x] > threshold1 || data[y][x] < threshold2 ) {
                data[y][x] = 0.0f;
            }
        }
    }
}
