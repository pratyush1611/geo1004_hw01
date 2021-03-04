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
    // three planes passing through the line and each vertex of the triangle
    // have the two other vertices on opposite sides
    return (
        is_opposite(orig, dest, v0, v1, v2)
        && is_opposite(v0, v1, orig, dest, v2)
        && is_opposite(v1, v2, orig, dest, v0)
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

std::string to_obj(Point center, float size, int nth) {
    auto& c = center;
    float r = size/2;
    int o = nth * 8 + 1;

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

std::vector<std::vector<Point>> OD_maker(Point vox_center , float voxel_size)
{
    Point xline_orig = vox_center - Point {voxel_size/2, 0, 0};
    Point xline_dest = vox_center + Point {voxel_size/2, 0, 0};

    Point yline_orig = vox_center - Point {0, voxel_size/2, 0};
    Point yline_dest = vox_center + Point {0, voxel_size/2, 0};

    Point zline_orig = vox_center - Point {0, 0, voxel_size/2};
    Point zline_dest = vox_center + Point {0, 0, voxel_size/2};

    std::vector<Point> xline{xline_orig, xline_dest};
    std::vector<Point> yline{yline_orig, yline_dest};
    std::vector<Point> zline{zline_orig, zline_dest};
    //return a vector of these origin destinations vectors
    std::vector<std::vector<Point>> returned_output;
    returned_output =  {xline, yline, zline};

    return returned_output;
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
        std::cout<<"Cant open :(\n";
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
    
    std::cout << "Voxelizing the model. \n";
    
    // Voxelise
    for (auto const &triangle: faces)
    {
        // todo
        if(triangle.size() != 3) std::cout << "haha, found ya, " << triangle.size() <<'\n';

        // extract vertex of face and store it in fc_rvtx
        std::vector<Point> fc_vrtx = {vertices[triangle[0] - 1], vertices[triangle[1] - 1], vertices[triangle[2] - 1]};

        // for every face, run all the voxel, or for every voxel run all the faces
        Bbox facebounds(fc_vrtx); //get bbox of the face
        //find voxels in this bbox
        std::vector<Rows> bnd_vox = voxels.intersection(facebounds);
        //check intersections with these voxels
        for(auto row_: bnd_vox)
        {
            //get center coord of the voxel in this iteration
            Point vox_center =  voxels.center(row_.x ,row_.y ,row_.z  );
            //get coord of the 3 lines
            //lets call the line xline, yline, zline
            //and the two ends of this line are orig and dest
            std::vector<std::vector<Point>> OD_pts =  OD_maker( vox_center ,  voxel_size);
            //once you have the orig dest do the test
            if(intersects(OD_pts[0][0], OD_pts[0][1], fc_vrtx[0], fc_vrtx[1], fc_vrtx[2])
            || intersects(OD_pts[1][0], OD_pts[1][1], fc_vrtx[0], fc_vrtx[1], fc_vrtx[2])
            || intersects(OD_pts[2][0], OD_pts[2][1], fc_vrtx[0], fc_vrtx[1], fc_vrtx[2]))
            {
                //voxel is ok
                voxels(row_.x, row_.y, row_.z) = 1;
            }
        }
    }

    // Fill model
    // todo: snowflake method

    for(int i= 0; i<voxels.max_x; i++)
    {
        for(int j=0 ;  j<voxels.max_y; j++)
        {
            int flag=0;
            for(int k=voxels.max_z-1; k>=0 ;k--)
            {
                if( voxels(i,j,k) == 1)//touched a filled voxel
                {
                    flag=1;
                }

                if(flag != 1) //means not hit the boudary yet
                {
                    //update the cell value to exterior
                    voxels(i,j,k) = 0;
                }
            }

        }
    }

    std::cout << "Writing to " << file_out << "\n";

    // Write voxels
    std::ofstream file(file_out);
    float size = voxel_size * 0.8;

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



    return 13;
}
