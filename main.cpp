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
    // 1/6 * dot(a-d, cross(b-d, c-d)), ditch the 1/6, we need sign only
    Point 
        ad = a - d,
        bd = b - d,
        cd = c - d;
    return ad.dot( bd.cross(cd) );
}

bool is_opposite(const Point& a, const Point& b, const Point& v0, const Point& v1, const Point& v2) 
{
    // signed volume multiple will be negative if the points are on the opposite sides
    return ( 0 > signed_volume(v0, v1, v2, a) * signed_volume(v0, v1, v2, b) );
}


bool intersects(const Point &orig, const Point &dest, const Point &v0, const Point &v1, const Point &v2)
{
    // endpoints of the line are on opposite sides of the triangle
    // and three planes passing through the line and each vertex 
    // of the triangle have the two other vertices on opposite sides
    return (
        is_opposite(orig, dest, v0, v1, v2)
        && is_opposite(v0, v1, orig, dest, v2)
        && is_opposite(v1, v2, orig, dest, v0)
    );
}

bool voxel_intersects(const Point& center, const float& size, const Point& v0, const Point& v1, const Point& v2) 
{
    float half = size / 2.0;
    const Point& c = center;
    const Point 
        xline_orig = Point{ c.x - half, c.y, c.z },
        xline_dest = Point{ c.x + half, c.y, c.z },
        yline_orig = Point{ c.x, c.y - half, c.z },
        yline_dest = Point{ c.x, c.y + half, c.z },
        zline_orig = Point{ c.x, c.y, c.z - half },
        zline_dest = Point{ c.x, c.y, c.z + half };
    
    return (intersects(xline_orig, xline_dest, v0, v1, v2)
         || intersects(yline_orig, yline_dest, v0, v1, v2)
         || intersects(zline_orig, zline_dest, v0, v1, v2) );
}

Point splitter(std::string str)
{
    // split by x y z
    std::istringstream linestream(str);
    std::vector<std::string> split_line {
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

std::string to_obj(Point center, float size, int nth) {
    const auto& c = center;
    const float r = size/2;
    const int o = nth * 8 + 1;

    char buffer[512];
    sprintf(buffer,
        "o %d \n"
        "v %f %f %f \n" "v %f %f %f \n" "v %f %f %f \n" "v %f %f %f \n"
        "v %f %f %f \n" "v %f %f %f \n" "v %f %f %f \n" "v %f %f %f \n"
        "f %d %d %d %d \n" "f %d %d %d %d \n" "f %d %d %d %d \n"
        "f %d %d %d %d \n" "f %d %d %d %d \n" "f %d %d %d %d \n",
        nth,
        c.x - r, c.y - r, c.z - r,
        c.x + r, c.y - r, c.z - r,
        c.x + r, c.y + r, c.z - r,
        c.x - r, c.y + r, c.z - r,
        c.x - r, c.y - r, c.z + r,
        c.x + r, c.y - r, c.z + r,
        c.x + r, c.y + r, c.z + r,
        c.x - r, c.y + r, c.z + r,
        0 + o, 1 + o, 5 + o, 4 + o,
        0 + o, 3 + o, 2 + o, 1 + o,
        0 + o, 4 + o, 7 + o, 3 + o,
        1 + o, 2 + o, 6 + o, 5 + o,
        2 + o, 3 + o, 7 + o, 6 + o,
        4 + o, 5 + o, 6 + o, 7 + o
    );
    return buffer;
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
        std::cout<<"Cant open " << file_in << " :( \n";
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
            unsigned int fx = static_cast<unsigned int>(vert.x - 1);
            unsigned int fy = static_cast<unsigned int>(vert.y - 1);
            unsigned int fz = static_cast<unsigned int>(vert.z - 1);

            std::vector<unsigned int> fac = {fx,fy,fz};
            faces.push_back(fac);
        }
    }
    infile.close();


    // get boundig box of all .obj vertices
    const Bbox bounds = Bbox(vertices);
    
    // create voxels using voxelGrid with number rows in X, Y and Z axis as per voxel size
    VoxelGrid voxels(bounds, voxel_size);
    
    // Voxelise
    std::cout << "Voxelizing the model " << "\n";
    
    for (auto const &triangle: faces)
    {
        const Point&
            v0 = vertices[triangle[0]],
            v1 = vertices[triangle[1]],
            v2 = vertices[triangle[2]];
        
        // get bbox of the face
        const Bbox bbox = Bbox(std::vector<Point>{v0, v1, v2});
        // find voxels in this bbox
        const std::vector<Rows> bbox_rows = voxels.intersection(bbox);
        // check intersections with these voxels
        for (auto rows : bbox_rows) {
            if (voxel_intersects(voxels.center(rows.x, rows.y, rows.z), voxel_size, v0, v1, v2)) {
                voxels(rows.x, rows.y, rows.z) = 1;
            }
        }      
    }

    // Fill model
    std::cout << "Filling the interior " << "\n";

    for(int i= 0; i<voxels.max_x; i++) {
        for(int j=0 ;  j<voxels.max_y; j++) {
            for(int k=voxels.max_z-1; k>=0; k--) {
                if (voxels(i, j, k) == 1) break; // touched a boundary voxel
                else voxels(i, j, k) = 0;
            }
        }
    }

    // Get volume
    std::cout << "\n\t" << "Total volume of the model is: ";
    std::cout << voxels.volume() << " m3 " << "\n\n";

    // Write voxels
    std::cout << "Writing model to " << file_out << "\n";

    std::ofstream file(file_out);
    float size = voxel_size * 0.7;

    int nth = 0;
    for (int x = 0; x < voxels.max_x; x++) {
        for (int y = 0; y < voxels.max_y; y++) {
            for (int z = 0; z < voxels.max_z; z++) {
                if (voxels(x, y, z) == 0) continue; // skip exterior voxels
                file << to_obj(voxels.center(x, y, z), size, nth);
                nth++;
            }
        }
    }
    file.close();

    return 0;
}
