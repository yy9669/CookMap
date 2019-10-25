#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "Part.hpp"
#include "StoryMode.hpp"

using namespace std;

/** Map
 *
 *  e for empty, g for ground, * for goal, 1234 for ingredients
 *
 *  File structure:
 *  width height (int)
 *  start_x start_y (float, float, from bottom left)
 *  npc_num (int)
 *  npc0_type npc0_x npc0_y (int, float, float, from bottom left)
 *  npc1_type npc1_x npc1_y
 *
 *  Map example:
 *  10 5
 *  0.0 2.0
 *  1
 *  0 6.0 2.0
 *  eeeeeeeee*
 *  eee2eeeegg
 *  ee1geeeggg
 *  gggggggggg
 *  gggggggggg
 *
 */

/** load map file and setup member variables of StoryMode
 *
 *  filename: which file to open
 *  mode: pointer to StoryMode
 *
 *  return true if success or false otherwise
 */
bool load_file(const string& filename, StoryMode* mode) {
    ifstream f(filename);

    if (!f.is_open()) {
        cerr << "Error: could NOT open file " << filename << endl;
        return false;
    }

    // reserve for space
    int width, height;
    f >> width >> height;
    if (height != 16) {
        cerr << "Error: map height must be 16" << endl;
        return false;
    }
    mode->parts.clear();
    mode->parts.resize(height);
    for (int i = 0; i < height; ++i) {
        mode->parts[i].resize(width);
    }

    // chef start position
    float start_x, start_y;
    f >> start_x >> start_y;
    mode->chef.position.x = start_x;
    mode->chef.position.y = start_y;

    // npc definition
    int npc_num, npc_type;
    float npc_x, npc_y;
    f >> npc_num;
    mode->npcs.clear();
    mode->npcs.resize(npc_num);
    for (int i = 0; i < npc_num; ++i) {
        f >> npc_type >> npc_x >> npc_y;
        mode->npcs[i] = new Npc(npc_type, npc_x, npc_y);
    }

    // fill in the parts map
    string s;
    for (int y = 0; y < height; ++y) {
        f >> s;
        if (s.size() != width) {
            cerr << "Error: wrong map format!" << endl;
            return false;
        }
        for (int x = 0; x < width; ++x) {
            auto &part = mode->parts[y][x];
            switch (s[x]) {
                case 'e':
                    part = new Empty();
                    break;
                case 'g':
                    part = new Ground();
                    break;
                case '*':
                    part = new Goal();
                    break;
                case '1': case '2': case '3': case '4':
                    part = new Ingredient(s[x] - '1');
                    break;
            }
        }
    }

    f.close();
    return true;
}
