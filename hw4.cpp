/*
NOTE: This program was written in the context of a sophmore level data structures and algorithms
    assignment.  There is a little jank and a little slop that can be improved, but it works very well 
    as-is within the assignment parameters, and received a perfect grade.
*/

// When compiling, unless you want it to run for 5-10 minutes, compile with -O9 flag

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

using namespace std;

// simple struct that holds coordinate data for nodes in the graph
struct Point {
    double x;
    double y;

    Point() { ; }
    Point(int x, int y) { this->x = x; this->y = y; }
};

// struct used when writing the .ppm image data to file
struct Pixel {
  unsigned char r, g, b;
};

// holds the image and problem data defined in the config.txt file
struct Config {
    public:
        string fileName;
        // The only purpose of the char * is to work more seamlessly
        // with the fread and fwrite functions
        char *fn;
        Point start;
        Point end;
        Point dean;
        Point lin;
        Point perez;
        Point reeder;
        int pThreshold;
        int minStep;
        int maxStep;
        int maxAngle;
        int minDistThresh;
};

// in order to properly select the neighbors to visit next, each node holds the following information:
// - the current coordinate within the graph.
// - the previous coordinate from whence you arrived.  This is used to calculate the distance information
//   required by the assignment parameters.
// - a short int that determines which passengers have been picked up along the way (another assignment parameter)
// comparison operators are overloaded to terminate comparisons as efficiently as possible when judging which
// nodes should be visited next
struct Node {
    Point current;
    Point prev;
    short passengers {0};

    Node() { ; }
    Node(int x, int y) { current.x = x; current.y = y; }
    Node(Point a, Point b) { current = a; prev = b; }
    Node(Point a, Point b, int p) { current = a; prev = b; passengers = p;}

    bool operator<(const Node &b) const {
        if (passengers == b.passengers) {
            if (prev.y == b.prev.y) {
                if (prev.x == b.prev.x) {
                    if (current.y == b.current.y) {
                        return current.x < b.current.x;
                    }
                    return current.y < b.current.y;
                }
                return prev.x < b.prev.x;
            }
            return prev.y < b.prev.y;
        }
        return passengers < b.passengers;
    }

    bool operator==(const Node &b) const {
        if (passengers == b.passengers) {
            if (prev.y == b.prev.y) {
                if (prev.x == b.prev.x) {
                    if (current.y == b.current.y) {
                        return current.x == b.current.x;
                    }
                    return false;
                }
                return false;
            }
            return false;
        }
        return false;
    }
};

// globals and definitions
Node pickups[6];
#define START pickups[0]
#define DEAN pickups[1]
#define LIN pickups[2]
#define PEREZ pickups[3]
#define REEDER pickups[4]
#define END pickups[5]

Pixel orange = {245,102,0};

const int inf = 999999;
Config imageData;
Pixel *image;
map<Node, double> dist;
map<Node, Node> pred;
int width, height;


typedef pair<double,Node> pin;

// distance calc between two points.  NOTE: Point coordinates are doubles, no need to typecast
double getDistance(Point &a, Point &b) {
    double d = ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));

    return sqrt(d);
}

// note: a is prev, b is current, c is potential nbr
// everything is already a double, no need to typecast
double dotProduct(Point &a, Point &b, Point &c) {
    Point u(b.x - a.x, b.y - a.y);
    Point v(c.x - b.x, c.y - b.y);

    double uDist = sqrt((u.x * u.x) + (u.y * u.y));
    double vDist = sqrt((v.x * v.x) + (v.y * v.y));

    double dotProd = (u.x * v.x + u.y * v.y) 
        / (uDist * vDist);

    return dotProd;
}
// note: a is prev, b is current, c is potential neighbor
bool checkAngle(Point &a, Point &b, Point &c) {

    return dotProduct(a, b, c) >= cos(imageData.maxAngle * M_PI / 180);
}

// checks to make sure pixel threshold is met
bool isBlack(int index) {
    if (image[index].r + image[index].g + image[index].b <= imageData.pThreshold) return true;
    return false;
}

vector<Node> getNeighbors(Node &src) {
    vector<Node> nbrs;
    // variables to set the range of the search box.  This should keep things in bounds.
    // typecast as ints since we are working with doubles
    int max_x = min(width - 1, (int)src.current.x + imageData.maxStep);
    int min_x = max(0, (int)src.current.x - imageData.maxStep);
    int max_y = min(height - 1, (int)src.current.y + imageData.maxStep);
    int min_y = max(0, (int)src.current.y - imageData.maxStep);

    // begin searching box for neighbors
    for (int i = min_y; i <= max_y; i++) {
        for (int j = min_x; j <= max_x; j++) {
            // first pass - does pixel pass threshold?
            if (isBlack(i * width + j)) {

                Node nbr(Point(j, i), src.current, src.passengers);
                if (getDistance(nbr.current, DEAN.current) <= imageData.minDistThresh) nbr.passengers|=1;
                if (getDistance(nbr.current, LIN.current) <= imageData.minDistThresh) nbr.passengers|=2;
                if (getDistance(nbr.current, PEREZ.current) <= imageData.minDistThresh) nbr.passengers|=4;
                if (getDistance(nbr.current, REEDER.current) <= imageData.minDistThresh) nbr.passengers|=4;

                double d = getDistance(src.current, nbr.current);
                // 2nd pass - distance is checked to make sure neighbor is in bounds.
                // also check to see if you are at the very first node, and disregard angles if you are
                if (src == START) {
                    if (d >= imageData.minStep && d <= imageData.maxStep) {
                        // if passes, node is added to neighbors
                        nbrs.push_back(nbr);
                    }
                }
                // for all subsequent nodes
                else {
                    if ((d >= imageData.minStep && d <= imageData.maxStep) && checkAngle(pred[src].current, src.current, nbr.current)) {
                        nbrs.push_back(nbr);
                    }
                }
            }
        }
    }
    return nbrs;
}

// helper function to color the path
void colorNode(Node &node) {
    int max_x = min(width - 1, (int)(node.current.x + 3));
    int min_x = max(0, (int)(node.current.x - 3));
    int max_y = min((height - 1),(int)( node.current.y + 3));
    int min_y = max(0, (int)(node.current.y - 3));

    for (int i = min_y; i <= max_y; i++) {
        for (int j = min_x; j <= max_x; j++) {
            image[i * width + j] = orange;
        }
    }
}

// mark's path without recursion, in case the path is beastly
void markPath(Node source, Node dest) {
    ofstream path("path.txt");
    vector<pair<int, int>> nodeList;

    while (!(source == dest)) {
        // add path nodes to a vector, reverse it, and print the path.  It's lame but it works
        nodeList.push_back(make_pair(dest.current.x, dest.current.y));
        colorNode(dest);
        dest = pred[dest];
    }
    // now add the first node since the loop exited
    nodeList.push_back(make_pair(dest.current.x, dest.current.y));
    colorNode(dest);

    reverse(nodeList.begin(), nodeList.end());
    
    for (auto i : nodeList) {
        path << i.first << " " << i.second << endl;
    }
    path.close();

	return;
}

void dijkstra(Node &source, Node &dest)
{
  dist[source] = 0;
  priority_queue<pin, vector<pin>,
		 greater<pin>> to_visit; 
  to_visit.push(make_pair(0, source));

  while (!to_visit.empty()) {
    Node x = to_visit.top().second;
    to_visit.pop();
    // end condition: when x is within distance threshold of destination, with the right passengers
    if ((x.passengers == 7) && (getDistance(x.current, dest.current) <= imageData.minDistThresh)) { 
        pred[dest] = x;
        dist[dest] = dist[x] + getDistance(x.current, dest.current);

        markPath(source, pred[dest]);
        return;
    }
    for (Node n : getNeighbors(x)) {
        double weight = getDistance(x.current, n.current);
      // only set to inf if node is unvisited.  is this slowing things down?
      if (dist.count(n) == 0) dist[n] = inf;
      if (dist[x] + weight < dist[n]) { 
        dist[n] = dist[x] + weight;
        pred[n] = x;
        to_visit.push(make_pair(dist[n], n));
      }
    }
  }
}

void initializeData(FILE *configFile) {
    imageData.fn = (char *)calloc(sizeof(char), 80);
    fscanf(configFile, "%s", imageData.fn);
    fscanf(configFile, "%le%le", &(imageData.start.x), &(imageData.start.y) );
    fscanf(configFile, "%le%le", &(imageData.end.x), &(imageData.end.y) );
    fscanf(configFile, "%le%le", &(imageData.dean.x), &(imageData.dean.y) );
    fscanf(configFile, "%le%le", &(imageData.lin.x), &(imageData.lin.y) );
    fscanf(configFile, "%le%le", &(imageData.perez.x), &(imageData.perez.y) );
    fscanf(configFile, "%le%le", &(imageData.reeder.x), &(imageData.reeder.y) );
    fscanf(configFile, "%i", &(imageData.pThreshold));
    fscanf(configFile, "%i%i", &(imageData.minStep), &(imageData.maxStep) );
    fscanf(configFile, "%i", &(imageData.maxAngle));
    fscanf(configFile, "%i", &(imageData.minDistThresh));

    return;
    
}

void buildPPM(FILE *img) {
    fscanf(img, "P6\n%d %d\n255%*c", &width, &height);
    image = new Pixel[width * height];

    fread(image, width * height, sizeof(Pixel), img);
    fclose(img);

    return;
}

void writePPM(void) {
    FILE *output = fopen("path.ppm", "w");
    fprintf(output, "P6\n%d %d\n255\n", width, height);
    fwrite(image, width * height, sizeof(Pixel), output);
    fclose(output);

    return;
}

int main(int argc, char *argv[]) {

    // working with C style inputs due to PPM usage
    FILE *configFile = fopen(argv[1], "r");   
    initializeData(configFile);
    FILE *img = fopen(imageData.fn, "r");

    buildPPM(img);

    fclose(configFile);

    // create initial nodes for function calls and pickups
    START = Node(imageData.start.x, imageData.start.y);
    DEAN = Node(imageData.dean.x, imageData.dean.y);
    LIN = Node(imageData.lin.x, imageData.lin.y);
    PEREZ = Node(imageData.perez.x, imageData.perez.y);
    REEDER = Node(imageData.reeder.x, imageData.reeder.y);
    END = Node(imageData.end.x, imageData.end.y);

    dijkstra(START, END); 
    cout << pred[END].current.x << " " << pred[END].current.y << endl;
    cout << dist[END] << endl;
    cout << pred[END].passengers;

    writePPM();
    
    return 0;
}



