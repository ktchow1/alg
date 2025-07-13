
class svm
{
public:
    svm(double lr = 0.001, int epochs = 1000, double C = 1.0)
    {
        learning_rate = lr;
        this->epochs = epochs;
        this->C = C;
    }

    void learn(const std::vector<std::vector<double>>& X, const std::vector<int>& y)
    {
        int n_samples = X.size();
        int n_features = X[0].size();
        weights.assign(n_features, 0.0);
        bias = 0.0;
        for (int epoch = 0; epoch < epochs; ++epoch)
        {
            for (int i = 0; i < n_samples; ++i)
            {
                double linear_output = dot(X[i], weights) + bias;
                int label = y[i] == 0 ? -1 : 1;
                if (label * linear_output < 1.0)
                {
                    for (int j = 0; j < n_features; ++j)
                    {
                        weights[j] += learning_rate * (label * X[i][j] - 2.0 * C * weights[j]);
                    }
                    bias += learning_rate * label;
                }
                else
                {
                    for (int j = 0; j < n_features; ++j)
                    {
                        weights[j] += learning_rate * (-2.0 * C * weights[j]);
                    }
                }
            }
        }
    }

    int inspect(const std::vector<double>& x) const
    {
        double result = dot(x, weights) + bias;
        return result >= 0.0 ? 1 : 0;
    }

private:
    static double dot(const std::vector<double>& a, const std::vector<double>& b)
    {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i)
        {
            sum += a[i] * b[i];
        }
        return sum;
    }

private:
    std::vector<double> weights;
    double bias = 0.0;
    double learning_rate;
    int epochs;
    double C;
};