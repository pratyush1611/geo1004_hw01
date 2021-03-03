#ifndef VoxelGrid_h
#define VoxelGrid_h

#include "Bbox.h"
#include "Point.h"
#include "Rows.h"

struct VoxelGrid {
  std::vector<unsigned int> voxels;
  unsigned int max_x, max_y, max_z, size;
  Bbox domain;
  float voxel_size;

  VoxelGrid(Bbox _domain, float _voxel_size) {
    domain = _domain;
    voxel_size = _voxel_size;
    max_x = int(domain.size_x() / voxel_size) + 1;
    max_y = int(domain.size_y() / voxel_size) + 1;
    max_z = int(domain.size_z() / voxel_size) + 1;
    size = max_x * max_y * max_z;
    voxels.reserve(size);
    for (unsigned int i = 0; i < size; ++i) voxels.push_back(0);
  }

  int _coords_to_i(const int& x, const int& y, const int& z) const {
      assert(x >= 0 && x < max_x);
      assert(y >= 0 && y < max_y);
      assert(z >= 0 && z < max_z);
      return voxels[x + y * max_x + z * max_x * max_y];
  }
  
  unsigned int &operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) {
      return voxels[_coords_to_i(x,y,z)];
  }
  
  unsigned int operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) const {
      return voxels[_coords_to_i(x, y, z)];
  }

  unsigned int &operator()(const Rows &rows) {
      return voxels[_coords_to_i(rows.x, rows.y, rows.z)];
  }

  unsigned int operator()(const Rows &rows) const {
      return voxels[_coords_to_i(rows.x, rows.y, rows.z)];
  }

  Point center(int x, int y, int z) {
      return Point(
          domain.min.x + (x + 1 / 2) * voxel_size,
          domain.min.y + (y + 1 / 2) * voxel_size,
          domain.min.z + (z + 1 / 2) * voxel_size
      );
  }

};

#endif /* VoxelGrid_h */
