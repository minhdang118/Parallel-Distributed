#include <iostream>
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>

using namespace std;

int** opt;
thread* threads;

void helper(int n, int* w, int* v, int i, int start, int end)
{
    for (int j = start; j < end; j++)
    {
        if (i == 0)
        {
            opt[i][j] = (j < w[i]) ? 0 : v[i];
        }
        else 
        if(j < w[i])
        {
            opt[i][j] = opt[i-1][j];
        }
        else
        {
            opt[i][j] = std::max(opt[i-1][j], opt[i-1][j-w[i]] + v[i]);
        }
    }
}

int parallel_knapsack(int n, int* w, int* v, int c, int p)
{
    for(int i=0; i<n; i++)
    {
        
        for(int j=0; j<p; j++)
        {
            threads[j] = thread(helper, n, w, v, i, c*j/p, c*(j+1)/p);
        }
        for(int j=0; j<p; j++)
        {
            threads[j].join();
        }
    }

    return opt[n-1][c-1];
}

int serial_knapsack(int n, int* w, int* v, int c)
{
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<c; j++)
        {
            if (i == 0)
            {
                opt[i][j] = (j < w[i]) ? 0 : v[i];
            }
            else if(j < w[i])
            {
                opt[i][j] = opt[i-1][j];
            }
            else
            {
                opt[i][j] = std::max(opt[i-1][j], opt[i-1][j-w[i]] + v[i]);
            }
        }
    }

    return opt[n-1][c-1];
}

int main(int argc, char const *argv[])
{
    using chrono::high_resolution_clock;
	using chrono::duration;

    int n;
    int c;
    int* w;
    int* v;

    ifstream myfile;
    ofstream outfile;
    outfile.open("averages.txt");

    for (int t = 1; t <= 3; t++)
    {
        outfile << "Input file " << t << endl;
        myfile.open("../inputs/" + to_string(t) + ".txt");

        if (myfile.is_open()) 
        {
            myfile >> n;
            myfile >> c;
            w = new int[n];
            v = new int[n];
            for(int i=0; i<n; i++)
            {
                myfile >> w[i];
                myfile >> v[i];
            }
        }
        else
        {
            cout << "Unable to open file";
        }

        auto start_serial = high_resolution_clock::now();

        opt = new int*[n];
        for(int i=0; i<n; i++)
        {
            opt[i] = new int[c];
        }

        outfile << "Serial: ";
        outfile << serial_knapsack(n, w, v, c) << endl;

        auto end_serial = high_resolution_clock::now();

        duration<double, milli> time_serial = end_serial - start_serial;

        outfile << "Serial time is: " << time_serial.count() << " milliseconds." << endl;

        for (int s = 0; s <= 4; s++)
        {
            int p = pow(2, s);
            threads = new thread[p];
            auto start_parallel = high_resolution_clock::now();

            opt = new int*[n];
            for(int i=0; i<n; i++)
            {
                opt[i] = new int[c];
            }

            outfile << "Parallel with " << p << " threads: ";
            outfile << parallel_knapsack(n, w, v, c, p) << endl;

            auto end_parallel = high_resolution_clock::now();

            duration<double, milli> time_parallel = end_parallel - start_parallel;

            outfile << "Parallel time is: " << time_parallel.count() << " milliseconds." << endl;
        }
        

        // auto start_parallel = high_resolution_clock::now();

        // opt = new int*[n];
        // for(int i=0; i<n; i++)
        // {
        //     opt[i] = new int[c];
        // }

        // outfile << "Parallel: ";
        // outfile << parallel_knapsack(n, w, v, c, p) << endl;

        // auto end_parallel = high_resolution_clock::now();

        // duration<double, milli> time_parallel = end_parallel - start_parallel;

        // outfile << "Parallel time is: " << time_parallel.count() << " milliseconds." << endl;
        outfile << endl;

        myfile.close();
    }

    outfile.close();
    

    // myfile.open("../inputs/2.txt");

    // if (myfile.is_open()) 
    // {
    //     myfile >> n;
    //     myfile >> c;
    //     w = new int[n];
    //     v = new int[n];
    //     for(int i=0; i<n; i++)
    //     {
    //         myfile >> w[i];
    //         myfile >> v[i];
    //     }
    // }
    // else
    // {
    //     cout << "Unable to open file";
    // }
    
    // int p = atoi(argv[1]);

    // auto start_serial = high_resolution_clock::now();

    // opt = new int*[n];
    // for(int i=0; i<n; i++)
    // {
    //     opt[i] = new int[c];
    // }

    // cout << "Serial: ";
    // cout << serial_knapsack(n, w, v, c) << endl;

    // auto end_serial = high_resolution_clock::now();

    // duration<double, milli> time_serial = end_serial - start_serial;

    // cout << "Serial time is: " << time_serial.count() << " milliseconds." << endl;

    // auto start_parallel = high_resolution_clock::now();

    // opt = new int*[n];
    // for(int i=0; i<n; i++)
    // {
    //     opt[i] = new int[c];
    // }

    // cout << "Parallel: ";
    // cout << parallel_knapsack(n, w, v, c, p) << endl;

    // auto end_parallel = high_resolution_clock::now();

    // duration<double, milli> time_parallel = end_parallel - start_parallel;

    // cout << "Parallel time is: " << time_parallel.count() << " milliseconds." << endl;

    return 0;
}
