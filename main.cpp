// Ondrej Vesely  [5162130]
// Pratyush Kumar [5359252]

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Point.h"
#include "Rows.h"
#include "VoxelGrid.h"

float signed_volume(const Point &a, const Point &b, const Point &c, const Point &d)
{
  // to do
  return 0;
}

bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2)
{
  // to do
  return 0;

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
    const char *file_in = "E:\\TU_Delft\\yr_1\\q3\\GEO1004\\Assignment\\hw01\\hw1\\bag_bk.obj";
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


    // get min and max x,y,z for bounding box
    Point bnd_min = {1000,  1000,  1000};
    Point bnd_max = {-1000, -1000, -1000};

    for(auto i: vertices)
    {
        if(i.x < bnd_min.x) bnd_min.x = i.x;
        if(i.y < bnd_min.y) bnd_min.y = i.y;
        if(i.z < bnd_min.z) bnd_min.z = i.z;

        if(i.x > bnd_max.x) bnd_max.x = i.x;
        if(i.y > bnd_max.y) bnd_max.y = i.y;
        if(i.z > bnd_max.z) bnd_max.z = i.z;
    }

    //compute number of hroi, vert and depth rows needed as per voxel size
    Point no_voxels = {0,0,0};
    //calculate x
    if( std::fmod((bnd_max.x - bnd_min.x),voxel_size) != 0) // means there are residues
    {
        no_voxels.x = int((bnd_max.x - bnd_min.x)/voxel_size) +1;
    }
    else no_voxels.x = int((bnd_max.x - bnd_min.x)/voxel_size) ;

    //calculate y
    if( std::fmod((bnd_max.y - bnd_min.y),voxel_size) != 0) // means there are residues
    {
        no_voxels.y = int((bnd_max.y - bnd_min.y)/voxel_size) +1;
    }
    else no_voxels.y = int((bnd_max.y - bnd_min.y)/voxel_size) ;

    //calculate z
    if( std::fmod((bnd_max.z - bnd_min.z),voxel_size) != 0) // means there are residues
    {
        no_voxels.z = int((bnd_max.z - bnd_min.z)/voxel_size) +1;
    }
    else no_voxels.z = int((bnd_max.z - bnd_min.z)/voxel_size) ;

    // Create grid
    Rows rows(unsigned int (no_voxels.x) , unsigned int (no_voxels.y), unsigned int (no_voxels.z));

    // create voxels using voxelGrid inbuilt
    VoxelGrid voxels(rows.x, rows.y, rows.z);

    unsigned int v = voxels(0,0,0);
    std::cout<< v; // access the values in a voxel


    // Voxelise
    for (auto const &triangle: faces)
    {
        // todo
        auto v1 = vertices[triangle[0]];
        auto v2 = vertices[triangle[1]];
        auto v3 = vertices[triangle[2]];

        // vertices of the face, v1 v2 v3
        // for every face, run all the voxel, or for every voxel run all the faces
        // triangle is defined by 3 vertices making 2 vectors and a normal (not needed)
        auto vec1 = v2-v1;
        auto vec2 = v3-v1;
        auto norm = vec1.cross(vec2);


    }

    std::cout<<voxels.max_x;

    // Fill model
    // todo

    // Write voxels
    // todo

    return 13;
}
