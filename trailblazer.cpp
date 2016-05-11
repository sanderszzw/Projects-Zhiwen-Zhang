/*
 * File: trailblazer.cpp
 * ------------
 * Name: Zhiwen Zhang
 * Course: CS 106B (Section: Thursday 4:30pm)
 * Description: Implements graph path searches using DFS, BFS, Dijkstra's algorithm, A* search, and Kruskal's algorithm.
 */

#include "trailblazer.h"
#include "pqueue.h"
#include "queue.h"
#include "hashmap.h"


using namespace std;

/*
 * Method: visitVertex
 * Usage: visitVertex(curr);
 * -------------------------
 * Accepts the current vertex as a reference parameter.
 * Marks the vertex as visited and sets the color on the GUI as green.
 */
void visitVertex(Vertex*& curr) {
    curr->visited = true;
    curr->setColor(GREEN);
}

/*
 * Method: depthFirstSearchHelper
 * Usage: bool foo = depthFirstSearchHelper(graph, curr, end, path);
 * -------------------------
 * *** Helper function for depthFirstSearch
 * Accepts the current vertex and the end vertex as values, and the graph and the path as reference parameters.
 * Returns true if a path between the current vertex and the end vertex is found.
 * Performs recursive backtracking to find a path between the two vertices.
 */
bool depthFirstSearchHelper(BasicGraph& graph, Vertex* curr, Vertex* end, Vector<Vertex*>& path) {
    // Add the current vertex "curr" to path and mark it as visited
    path.add(curr);
    visitVertex(curr);
    if (curr->name == end->name) {
        // Found a path
        return true;
    }
    // For each unvisited neighbor of curr:
    for (Vertex* neighbor: graph.getNeighbors(curr)) {
        if (!neighbor->visited && depthFirstSearchHelper(graph, neighbor, end, path)) {
            // Found a path
            return true;
        }
    }
    // Unable to find a path (Remove the curr vertex from the path)
    path[path.size() - 1]->setColor(GRAY);
    path.remove(path.size() - 1);
    return false;
}

/*
 * Method: depthFirstSearch
 * Usage: Vector = depthFirstSearch(graph, start, end);
 * -------------------------
 * Accepts the start vertex and the end vertex as values, and the graph and the path as reference parameters.
 * Returns a path between the start vertex and the end vertex.
 * Returns a empty vector if no path found.
 */
Vector<Vertex*> depthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    graph.resetData();
    Vector<Vertex*> path;
    depthFirstSearchHelper(graph, start, end, path);
    return path;
}

/*
 * Method: constructPath
 * Usage: constructPath(end, path);
 * -------------------------
 * Accepts a vertex as values, using "previous" pointer to track the path.
 * Returns a path between the start and the end vertex using recusion.
 */
void constructPath(Vertex*& end, Vector<Vertex*>& path) {
    if (end != NULL) {
        constructPath(end->previous, path);
        path.add(end);
    }
}

/*
 * Method: breadthFirstSearch
 * Usage: Vector v = breadthFirstSearch(graph, start, end)
 * -------------------------
 * Accepts the start vertex and the end vertex as values, and the graph as reference parameters.
 * Returns a path between the start vertex and the end vertex.
 * Returns a empty vector if no path found.
 */
Vector<Vertex*> breadthFirstSearch(BasicGraph& graph, Vertex* start, Vertex* end) {
    graph.resetData();
    Vector<Vertex*> path;
    Queue<Vertex*> vq;
    vq.enqueue(start);
    start->setColor(YELLOW);
    start->visited = true;
    // While queue is not empty
    while (!vq.isEmpty()) {
        Vertex* curr = vq.dequeue();
        visitVertex(curr);
        if (curr == end) {
            // A path is found
            constructPath(end, path);
            return path;
        }
        // For each unvisited neighbor of the vertex "curr"
        for (Vertex* neighbor: graph.getNeighbors(curr)) {
            if (!neighbor->visited) {
                neighbor->visited = true;
                neighbor->previous = curr;
                vq.enqueue(neighbor);
                neighbor->setColor(YELLOW);
            }
        }
    }
    return path;
}

/*
 * Method: dijkstrasHelper
 * Usage: Vector v = dijkstrasHelper(graph, start, end, H_or_not)
 * -------------------------
 * Accepts the start vertex and the end vertex as values, and the graph as reference parameters.
 * Accept an int H_or_not as parameters in order to extend this code to be useful in aStar.
 * Returns a path between the start vertex and the end vertex.
 * Returns a empty vector if no path found.
 * H_or_not is assigned to 1 or 0 if using heuristicFunction or not.
 */
Vector<Vertex*> dijkstrasHelper(BasicGraph& graph, Vertex* start, Vertex* end, int H_or_not) {
    graph.resetData();
    PriorityQueue<Vertex*> pq;
    Edge* edge;
    Vector<Vertex*> path;
    //initialize every node to have cost ∞
    for (Vertex* v: graph.getVertexSet()) {
        v->cost = POSITIVE_INFINITY;
        v->previous = NULL;
    }
    //enqueue v1 with priority 0(dijkstrasAlgorithm) or 0+heuristics(aStar)
    start->cost = 0;
    pq.enqueue(start, 0 + H_or_not*heuristicFunction(start, end));
    start->setColor(YELLOW);
    //while the priority queue is not empty
    while (!pq.isEmpty()) {
        Vertex* curr = pq.dequeue();
        visitVertex(curr);
        if (curr == end) {
            // A path is found
            constructPath(end, path);
            return path;
        }
        // For each unvisited neighbor of the vertex "curr"
        for (Vertex* neighbor : graph.getNeighbors(curr)) {
            if (!neighbor->visited) {
                edge = graph.getEdge(curr, neighbor);
                //cost = curr’s cost + weight of edge from n to v
                double cost = curr->cost + edge->weight;
                //if cost < n’s cost set n’s cost to cost and n’s previous to v
                if (cost < neighbor->cost) {
                    neighbor->previous = curr;
                    neighbor->cost = cost;
                    //if n is in pq, update its priority to be cost(dijkstrasAlgorithm) or cost+heuristics(aStar)
                    if (neighbor->getColor() == YELLOW) {
                        pq.changePriority(neighbor, cost + H_or_not*heuristicFunction(neighbor, end));
                    } else {
                        //else, enqueue n with priority = cost(dijkstrasAlgorithm) or cost+heuristics(aStar)
                        pq.enqueue(neighbor, cost + H_or_not*heuristicFunction(neighbor, end));
                        neighbor->setColor(YELLOW);
                    }
                }
            }
        }
    }
    return path;
}

/*
 * Method: dijkstrasAlgorithm
 * Usage: Vector v = dijkstrasAlgorithm(graph, start, end)
 * -------------------------
 * Accepts the start vertex and the end vertex as values, and the graph as reference parameters.
 * Returns a path between the start vertex and the end vertex.
 * Returns a empty vector if no path found.
 */
Vector<Vertex*> dijkstrasAlgorithm(BasicGraph& graph, Vertex* start, Vertex* end) {
    //H_or_not is assigned to 0 because of not using heuristicFunction
    int H_or_not = 0;
    return dijkstrasHelper(graph, start, end, H_or_not);
}

/*
 * Method: aStar
 * Usage: Vector v = aStar(graph, start, end)
 * -------------------------
 * Accepts the start vertex and the end vertex as values, and the graph as reference parameters.
 * Returns a path between the start vertex and the end vertex.
 * Returns a empty vector if no path found.
 */
Vector<Vertex*> aStar(BasicGraph& graph, Vertex* start, Vertex* end) {
    //H_or_not is assigned to 1 because of using heuristicFunction
    int H_or_not = 1;
    return dijkstrasHelper(graph, start, end, H_or_not);
}

/*
 * Method: kruskal
 * Usage: Set s = aStar(graph)
 * -------------------------
 * Accepts the graph as reference parameters.
 * Returns a set of pointers to edges in the graph such that
 * those edges would connect the graph's vertexes into a minimum spanning tree.
 */
Set<Edge*> kruskal(BasicGraph& graph) {
    graph.resetData();
    Set<Edge*> mst;
    PriorityQueue<Edge*> pq;

    HashMap<Vertex*, Set<Vertex*> > mapCluster;
    HashMap<Vertex*, Set<Vertex*>* > map;

    //put each vertex into a map with the value of a set(put all nodes into their own cluster)
    for (Vertex* v : graph.getVertexSet()) {
        Set<Vertex*> set;
        set.add(v);
        mapCluster[v] = set;
    }
    //build a map from vertex to pointer(pointing to cluster)
    for (Vertex* v : graph.getVertexSet()) {
        map[v] = &mapCluster[v];
    }
    //Place all edges into a priority queue based on their weight(cost)
    for (Edge* edge : graph.getEdgeSet()) {
        pq.enqueue(edge, edge->cost);
    }
    //while priority queue is not empty
    while (!pq.isEmpty()) {
        Edge* edge = pq.dequeue();
        if (map[edge->start] != map[edge->end]) {
            mst.add(edge);
            //make a new set combining two sets(one from the start of the edge and another one from the end)
            Set<Vertex*> combinedSet = *map[edge->start] + *map[edge->end];
            //change the cluster of the "end" side
            *map[edge->end] = combinedSet;
            //make all vertex from "start" cluster in the map point to the address of "end" cluster
            //the idea is that: for vertex that are in the same cluster, they are mapped to the same address
            for(Vertex* v : *map[edge->start]) {
                map[v] = map[edge->end];
            }
        }
    }
    return mst;
}

