#include <vector>
#include <iostream>
#include <cuda.h>
#include <chrono>

#define THREAD_COUNT 1024

using namespace std;

void readGraph(int &vertexCount, int &edgeCount, int &maxDegree, int **adjList, int **adjListPointers)
{
    int u, v;
    cin >> vertexCount >> edgeCount;

    vector<int> *adj = new vector<int>[vertexCount];
    for (int i = 0; i < edgeCount; i++)
    {
        cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    *adjList = new int[edgeCount * 2 + 1];
    *adjListPointers = new int[vertexCount + 1];
    int pos = 0;
    for (int i = 0; i < vertexCount; i++)
    {
        (*adjListPointers)[i] = pos;
        for (int j = 0; j < adj[i].size(); j++)
        {
            (*adjList)[pos++] = adj[i][j];
        }
    }
    (*adjListPointers)[vertexCount] = pos;

    maxDegree = INT_MIN;
    for (int i = 0; i < vertexCount; i++)
    {
        maxDegree = max(maxDegree, (int)adj[i].size());
    }

    delete[] adj;

    return;
}

__global__
void assignColors(int vertexCount, int *adjList, int *adjListPointers, int *colors, bool *conflicts, int maxDegree)
{
    int vertex = blockIdx.x * blockDim.x + threadIdx.x;
    if (vertex >= vertexCount || !conflicts[vertex])
    {
        return;
    }

    int maxColors = maxDegree + 1;

    int *forbidden = new int[maxColors/32 + 1];
    if (forbidden == NULL)
    {
        printf("Memory allocation failed\n");
        return;
    }
    memset(forbidden, 0, sizeof(int) * (maxColors/32 + 1));

    for (int i = adjListPointers[vertex]; i < adjListPointers[vertex + 1]; i++)
    {
        int neighbor = adjList[i];
        int ind = colors[neighbor] % 32;
        forbidden[colors[neighbor] / 32] |= (1 << ind);
    }

    for (int c = 1; c < maxColors; c++)
    {
        int ind = c % 32;
        if ((forbidden[c / 32] & (1 << ind)) == 0)
        {
            colors[vertex] = c;
            break;
        }
    }

    delete[] forbidden;
}

void assignColorsWrapper(int vertexCount, int *adjList, int *adjListPointers, int *colors, bool *conflicts, int maxDegree)
{
    assignColors<<<(vertexCount + THREAD_COUNT - 1) / THREAD_COUNT, THREAD_COUNT>>>(vertexCount, adjList, adjListPointers, colors, conflicts, maxDegree);
    cudaDeviceSynchronize();
}

__global__
void detectConflicts(int vertexCount, int *adjList, int *adjListPointers, int *colors, bool *conflicts, bool *conflictExists)
{
    int vertex = blockIdx.x * blockDim.x + threadIdx.x;
    if (vertex >= vertexCount)
    {
        return;
    }

    conflicts[vertex] = false;

    for (int i = adjListPointers[vertex]; i < adjListPointers[vertex + 1]; i++)
    {
        int neighbor = adjList[i];
        if (colors[vertex] == colors[neighbor] && neighbor < vertex)
        {
            conflicts[vertex] = true;
            *conflictExists = true;
        }
    }
}

bool detectConflictsWrapper(int vertexCount, int *adjList, int *adjListPointers, int *colors, bool *conflicts)
{
    bool *device_conflictExists;
    cudaMalloc(&device_conflictExists, sizeof(bool));
    cudaMemset(device_conflictExists, 0, sizeof(bool));

    detectConflicts<<<(vertexCount + THREAD_COUNT - 1) / THREAD_COUNT, THREAD_COUNT>>>(vertexCount, adjList, adjListPointers, colors, conflicts, device_conflictExists);
    cudaDeviceSynchronize();

    bool host_conflictExists = false;
    cudaMemcpy(&host_conflictExists, device_conflictExists, sizeof(bool), cudaMemcpyDeviceToHost);
    cudaFree(device_conflictExists);

    return host_conflictExists;
}

int *graphColoring(int vertexCount, int *adjList, int *adjListPointers, int maxDegree)
{
    bool *host_conflicts = new bool[vertexCount];
    bool *device_conflicts;
    int *host_colors = new int[vertexCount];
    int *device_colors;

    memset(host_colors, 0, sizeof(int) * vertexCount);
    memset(host_conflicts, true, sizeof(bool) * vertexCount);

    cudaMalloc((void**)&device_colors, sizeof(int) * vertexCount);
    cudaMemcpy(device_colors, host_colors, sizeof(int) * vertexCount, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&device_conflicts, sizeof(bool) * vertexCount);
    cudaMemcpy(device_conflicts, host_conflicts, sizeof(bool) * vertexCount, cudaMemcpyHostToDevice);

    do {
        assignColorsWrapper(vertexCount, adjList, adjListPointers, device_colors, device_conflicts, maxDegree);
    } while (detectConflictsWrapper(vertexCount, adjList, adjListPointers, device_colors, device_conflicts));

    cudaMemcpy(host_colors, device_colors, sizeof(int) * vertexCount, cudaMemcpyDeviceToHost);

    delete[] host_conflicts;
    cudaFree(device_colors);
    cudaFree(device_conflicts);

    return host_colors;
}

int main(int argc, char *argv[])
{
    using chrono::high_resolution_clock;
	using chrono::duration;

    int vertexCount, edgeCount, maxDegree;
    int *adjList = NULL, *adjListPointers = NULL;
    int *device_adjList, *device_adjListPointers;
    string in_file_name = argv[1];
    string out_file_name = argv[2];

    auto readGraphStart = high_resolution_clock::now();

    freopen(in_file_name.c_str(), "r", stdin);
    readGraph(vertexCount, edgeCount, maxDegree, &adjList, &adjListPointers);

    auto parallelStart = high_resolution_clock::now();

    cudaMalloc(&device_adjList, sizeof(int) * (edgeCount * 2 + 1));
    cudaMemcpy(device_adjList, adjList, sizeof(int) * (edgeCount * 2 + 1), cudaMemcpyHostToDevice);
    cudaMalloc(&device_adjListPointers, sizeof(int) * (vertexCount + 1));
    cudaMemcpy(device_adjListPointers, adjListPointers, sizeof(int) * (vertexCount + 1), cudaMemcpyHostToDevice);

    int *coloring = graphColoring(vertexCount, device_adjList, device_adjListPointers, maxDegree);

    auto parallelEnd = high_resolution_clock::now();

    cout << endl;

    int numColorsUsed = 0;
    for (int i = 0; i < vertexCount; i++)
    {
        numColorsUsed = max(numColorsUsed, coloring[i]);
    }
    cout << "Number of colors used: " << numColorsUsed << endl;

    int numConflicts = 0;
    for (int u = 0; u < vertexCount; u++)
    {
        for (int i = adjListPointers[u]; i < adjListPointers[u + 1]; i++)
        {
            int v = adjList[i];
            if (coloring[u] == coloring[v] && u <= v)
            {
                cout << "Conflict found between " << u << " and " << v << endl;
                numConflicts++;
            }
        }
    }
    cout << "Number of conflicts: " << numConflicts << endl;
    cout << endl;

    duration<double, milli> parallelTime = parallelEnd - parallelStart;
    duration<double, milli> totalTime = parallelEnd - readGraphStart;

    cout << "Parallel time: " << parallelTime.count() << " ms" << endl;
    cout << "Total time: " << totalTime.count() << " ms" << endl;

    freopen(out_file_name.c_str(), "w", stdout);
    for (int i = 0; i < vertexCount; i++)
    {
        cout << coloring[i] << endl;
    }

    delete[] coloring, adjList, adjListPointers;
    cudaFree(device_adjList);
    cudaFree(device_adjListPointers);

    return 0;
}
