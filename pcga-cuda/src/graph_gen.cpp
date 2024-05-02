#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    int n, m, u, v;
    string file_name = argv[1];

    cout << "Enter number of vertices: ";
    cin >> n;
    cout << "Enter number of edges: ";
    cin >> m;

    freopen(file_name.c_str(), "w", stdout);
    cout << n << " " << m << endl;

    for (int i = 0; i < m; i++)
    {
        do
        {
            u = rand() % n;
            v = rand() % n;
        } while (u == v);

        cout << u << " " << v << endl;
    }
    return 0;
}

