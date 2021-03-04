#ifndef VoxelGrid_h
#define VoxelGrid_h

#include "Bbox.h"
#include "Point.h"

struct VoxelGrid {
  std::vector<unsigned int> voxels;
  unsigned int max_x, max_y, max_z;
  Bbox domain;
  float voxel_size;

  VoxelGrid(Bbox _domain, float _voxel_size) {
    domain = _domain;
    voxel_size = _voxel_size;

    max_x = int(domain.size_x() / voxel_size) + 2;
    max_y = int(domain.size_y() / voxel_size) + 2;
    max_z = int(domain.size_z() / voxel_size) + 2;

    int size = max_x * max_y * max_z;
    voxels.reserve(size);
    for (unsigned int i = 0; i < size; ++i) voxels.push_back(2);
  }
  
  unsigned int &operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) {
    assert(x >= 0 && x < max_x);
    assert(y >= 0 && y < max_y);
    assert(z >= 0 && z < max_z);
    return voxels[x + y*max_x + z*max_x*max_y];
  }
  
  unsigned int operator()(const unsigned int &x, const unsigned int &y, const unsigned int &z) const {
    assert(x >= 0 && x < max_x);
    assert(y >= 0 && y < max_y);
    assert(z >= 0 && z < max_z);
    return voxels[x + y*max_x + z*max_x*max_y];
  }

  float volume() {
      float half_vox = voxel_size * voxel_size * voxel_size / 2;
      float total = 0;
      for (float val : voxels) {
          total += val * half_vox;
      }
  }

  Point center(int x, int y, int z) {
    return Point(
        domain.min.x + (x + 1 / 2) * voxel_size,
        domain.min.y + (y + 1 / 2) * voxel_size,
        domain.min.z + (z + 1 / 2) * voxel_size
    );
  }

  std::vector<Rows> intersection(Bbox bbox) {
    Bbox join = domain.intersection(bbox);
    int min_row_x = int((join.min.x - domain.min.x) / domain.size_x() * (max_x - 1)) - 1;
    int max_row_x = int((join.max.x - domain.min.x) / domain.size_x() * (max_x - 1)) + 2;
    int min_row_y = int((join.min.y - domain.min.y) / domain.size_y() * (max_y - 1)) - 1;
    int max_row_y = int((join.max.y - domain.min.y) / domain.size_y() * (max_y - 1)) + 2;
    int min_row_z = int((join.min.z - domain.min.z) / domain.size_z() * (max_z - 1)) - 1;
    int max_row_z = int((join.max.z - domain.min.z) / domain.size_z() * (max_z - 1)) + 2;

    std::vector<Rows> rows;
    rows.reserve((max_row_x - min_row_x + 1) * (max_row_y - min_row_y + 1) * (max_row_z - min_row_z + 1));
    for (int x = min_row_x; x < max_row_x; x++) {
        for (int y = min_row_y; y < max_row_y; y++) {
            for (int z = min_row_z; z < max_row_z; z++) {
                if (is_inside(x,y,z)) rows.push_back(Rows(x, y, z));
            }
        }
    }
    return rows;
  }

  bool is_inside(int &x, int &y, int &z) {
    return (
           x >= 0 && x < max_x
        && y >= 0 && y < max_y
        && z >= 0 && z < max_z
    );
  }

  unsigned int& value(const unsigned int& x, const unsigned int& y, const unsigned int& z) {
      return voxels[x + y * max_x + z * max_x * max_y];
  }

  void fill(int x, int y, int z) {
      if (is_inside(x, y, z) && value(x, y, z) != 1 ) {
          voxels[x + y * max_x + z * max_x * max_y] = 0;
          fill(x - 1, y - 0, z - 0);
          fill(x + 1, y - 0, z - 0);
          fill(x - 0, y + 1, z - 0);
          fill(x - 0, y - 1, z - 0);
          fill(x - 0, y - 0, z + 1);
          fill(x - 0, y - 0, z - 1);
      }
  }
};

#endif /* VoxelGrid_h */
