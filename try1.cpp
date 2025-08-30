#include <bits/stdc++.h>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

typedef __int128 int128;

int128 baseToDecimal(const string &num, int base)
{
    int128 res = 0;
    for (char ch : num)
    {
        int val;
        if (ch >= '0' && ch <= '9')
            val = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            val = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            val = ch - 'A' + 10;
        else
            val = 0;
        res = res * base + val;
    }
    return res;
}

void printInt128(int128 x)
{
    if (x == 0)
    {
        cout << 0;
        return;
    }
    string s;
    bool neg = x < 0;
    if (neg)
        x = -x;
    while (x > 0)
    {
        s += '0' + x % 10;
        x /= 10;
    }
    if (neg)
        s += '-';
    reverse(s.begin(), s.end());
    cout << s;
}

int128 lagrangeAtZeroExact(const vector<pair<int128, int128>> &subset)
{
    int128 result = 0;
    int k = subset.size();
    for (int i = 0; i < k; i++)
    {
        int128 xi = subset[i].first;
        int128 yi = subset[i].second;
        int128 num = yi;
        int128 denom = 1;
        for (int j = 0; j < k; j++)
        {
            if (i != j)
            {
                int128 xj = subset[j].first;
                num *= -xj;
                denom *= (xi - xj);
            }
        }
        result += num / denom;
    }
    return result;
}

int128 evaluatePolynomialExact(const vector<pair<int128, int128>> &subset, int128 x)
{
    int128 fx = 0;
    int k = subset.size();
    for (int i = 0; i < k; i++)
    {
        int128 xi = subset[i].first;
        int128 yi = subset[i].second;
        int128 num = yi;
        int128 denom = 1;
        for (int j = 0; j < k; j++)
        {
            if (i != j)
            {
                int128 xj = subset[j].first;
                num *= (x - xj);
                denom *= (xi - xj);
            }
        }
        fx += num / denom;
    }
    return fx;
}

void generateCombinations(int n, int k, vector<vector<int>> &allComb)
{
    vector<int> comb(k);
    for (int i = 0; i < k; i++)
        comb[i] = i;
    while (true)
    {
        allComb.push_back(comb);
        int i;
        for (i = k - 1; i >= 0; i--)
        {
            if (comb[i] != i + n - k)
                break;
        }
        if (i < 0)
            break;
        comb[i]++;
        for (int j = i + 1; j < k; j++)
            comb[j] = comb[j - 1] + 1;
    }
}

void processFile(const string &filename)
{
    cout << "=== Processing " << filename << " ===\n";
    ifstream fin(filename);
    if (!fin.is_open())
    {
        cout << "Cannot open file: " << filename << endl;
        return;
    }

    json j;
    try
    {
        fin >> j;
    }
    catch (...)
    {
        cout << "Error parsing JSON file: " << filename << endl;
        return;
    }

    int n = j["keys"]["n"];
    int k = j["keys"]["k"];

    vector<tuple<int, string, string>> data;
    for (int i = 1; i <= n; i++)
    {
        string idx = to_string(i);
        if (!j.contains(idx))
            continue;

        string base = "0", value = "0";
        if (j[idx].contains("base") && !j[idx]["base"].is_null())
            base = j[idx]["base"].get<string>();
        if (j[idx].contains("value") && !j[idx]["value"].is_null())
            value = j[idx]["value"].get<string>();

        data.push_back({i, base, value});
    }

    vector<pair<int128, int128>> points;
    for (auto &item : data)
    {
        int key;
        string baseStr, valStr;
        tie(key, baseStr, valStr) = item;
        int128 val = baseToDecimal(valStr, stoi(baseStr));
        points.push_back({key, val});
    }

    vector<vector<int>> allComb;
    generateCombinations(n, k, allComb);

    int128 bestC = 0;
    vector<int> bestInvalidKeys;

    for (auto &comb : allComb)
    {
        vector<pair<int128, int128>> subset;
        for (int idx : comb)
            subset.push_back(points[idx]);

        int128 c = lagrangeAtZeroExact(subset);
        vector<int> invalidKeys;
        for (auto &p : points)
        {
            int128 fx = evaluatePolynomialExact(subset, p.first);
            if (fx != p.second)
                invalidKeys.push_back((int)p.first);
        }

        if (bestInvalidKeys.empty() || invalidKeys.size() < bestInvalidKeys.size())
        {
            bestInvalidKeys = invalidKeys;
            bestC = c;
            if (bestInvalidKeys.empty())
                break;
        }
    }

    cout << "Constant term c (f(0)) = ";
    printInt128(bestC);
    cout << "\nInvalid keys: ";
    if (bestInvalidKeys.empty())
        cout << "None";
    else
        for (int key : bestInvalidKeys)
            cout << key << " ";
    cout << "\n";
    cout << "==============================\n\n";
}

int main()
{

    vector<string> files = {"testcase1.json", "testcase2.json"};
    for (const auto &filename : files)
    {
        processFile(filename);
    }
    return 0;
}
