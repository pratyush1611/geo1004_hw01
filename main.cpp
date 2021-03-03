// Ondrej Vesely  [5162130]
// Pratyush Kumar [5359252]

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Point.h"
#include "VoxelGrid.h"

float signed_volume(const Point &a, const Point &b, const Point &c, const Point &d)
{
    // 1/6 * dot(a-d, cross(b-d, c-d))
    const Point 
        ad = a - d,
        bd = b - d,
        cd = c - d;
    return 1/6 * ad.dot( bd.cross(cd) );
}

bool is_opposite(const Point& a, const Point& b, const Point& v0, const Point& v1, const Point& v2) 
{
    // signed volume multiple will be negative if the points are on the opposite sides
    return ( 0 >= signed_volume(v0, v1, v2, a) * signed_volume(v0, v1, v2, b) );
}

bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2)
{
    // endpoints of the line are on opposite sides of the triangle
    // three planes passing through the line and each vertex of the triangle
    // have the two other vertices on opposite sides
    return (
        is_opposite(orig, dest, v0, v1, v2)
        && is_opposite(v0, v1, orig, dest, v2)
        && is_opposite(v1, v2, orig, dest, v0)
        && is_opposite(v2, v0, orig, dest, v1)
    );
}

Point splitter(std::string str)
{
    // split by x y z
    std::istringstream linestream(str);
    std::vector<std::string> split_line
            {
                    std::istream_iterator<std::string>{linestream},
                    std::istream_iterator<std::string>{}
            };

    //convert char to float
    if (split_line.size() == 4)
    {
        const float x = std::stof(split_line[1]);
        const float y = std::stof(split_line[2]);
        const float z = std::stof(split_line[3]);
        Point pt = Point{x, y, z};
        return pt;
    }
}


int main(int argc, const char * argv[])
{
    const char *file_in = "bag_bk.obj";
    const char *file_out = "vox.obj";
    float voxel_size = 1.0;

    // Read file
    std::vector<Point> vertices;
    std::vector<std::vector<unsigned int>> faces;

    std::cout << "Reading file: " << file_in << std::endl;
    std::ifstream infile(file_in, std::ifstream::in);
    if(!infile)
    {
        std::cout<<"cant open :(\n";
        return 1;
    }

    std::string str;
    while(std::getline(infile,str))
    {

        if (strnicmp(str.c_str(), "v", strlen("v") ) == 0)
        {
            Point vert = splitter(str);
            vertices.push_back(vert);
        }

        else if (strnicmp(str.c_str(), "f", strlen("v") ) == 0)
        {
            Point vert = splitter(str);
            unsigned int fx = static_cast<unsigned int>(vert.x);
            unsigned int fy = static_cast<unsigned int>(vert.y);
            unsigned int fz = static_cast<unsigned int>(vert.z);

            std::vector<unsigned int> fac = {fx,fy,fz};
            faces.push_back(fac);
        }
    } // end of while
    infile.close(); //close file


    // get boundig box of all .obj vertices
    Bbox bounds = Bbox(vertices);
    
    // create voxels using voxelGrid with number rows in X, Y and Z axis as per voxel size
    VoxelGrid voxels(bounds, voxel_size);

    unsigned int v = voxels(0,0,0);
    std::cout<< v; // access the values in a voxel

    // Voxelise
    for (auto const &triangle: faces)
    {
        // todo
//        auto v1 = vertices[triangle[0]];
//        auto v2 = vertices[triangle[1]];
//        auto v3 = vertices[triangle[2]];
//
//        // vertices of the face, v1 v2 v3
//        // for every face, run all the voxel, or for every voxel run all the faces
//        // triangle is defined by 3 vertices making 2 vectors and a normal (not needed)
//        auto vec1 = v2-v1;
//        auto vec2 = v3-v1;
////        auto norm = vec1.cross(vec2);


    }

    std::cout<<voxels.max_x;

    // Fill model
    // todo

    // Write voxels
    // todo
    for (int i = 0; i < voxels.voxels.size(); i++) {
        if (voxels.voxels[i] == 0) continue; // skip exterior voxels

        


    };



    return 13;
}
