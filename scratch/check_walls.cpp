#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

struct Box { float cx, cy, cz, sx, sy, sz; };

int main() {
    std::vector<Box> visBoxes;
    std::ifstream vFile("bin/Debug/assets/level_boxes.txt");
    int count = 0;
    vFile >> count;
    for(int i=0; i<count; ++i) {
        int t; float cx, cy, cz, sx, sy, sz, rot;
        vFile >> t >> cx >> cy >> cz >> sx >> sy >> sz >> rot;
        if(t == 0 || t == 1) visBoxes.push_back({cx, cy, cz, sx, sy, sz});
    }

    std::vector<Box> physBoxes;
    std::ifstream pFile("bin/Debug/assets/maze_physics.txt");
    pFile >> count;
    for(int i=0; i<count; ++i) {
        float minX, minY, minZ, maxX, maxY, maxZ;
        pFile >> minX >> minY >> minZ >> maxX >> maxY >> maxZ;
        float cx = minX + (maxX - minX)/2.0f;
        float cz = minZ + (maxZ - minZ)/2.0f;
        physBoxes.push_back({cx, 0, cz, maxX-minX, maxY-minY, maxZ-minZ});
    }

    int missing = 0;
    for(const auto& vb : visBoxes) {
        bool found = false;
        for(const auto& pb : physBoxes) {
            // physics boxes are tiny, they might not center perfectly on the big visual box
            // wait, visual box is also 0.6x1.62x0.6! so centers should match!
            if(std::abs(vb.cx - pb.cx) < 0.3f && std::abs(vb.cz - pb.cz) < 0.3f) {
                found = true;
                break;
            }
        }
        if(!found) {
            std::cout << "Visual wall missing physics: " << vb.cx << ", " << vb.cz << "\n";
            missing++;
        }
    }
    std::cout << "Total missing: " << missing << "\n";
    return 0;
}
