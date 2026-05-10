#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include "../src/Geometry.h"
#include "../src/PointStream.h"

// Forward declaration of Execute from surfrecon.cpp (it needs to be available)
// However, Execute is a template in surfrecon.cpp. 
// It might be better to just run the executable via shell if we want to test "the method" as an end-user.
// But the user asked to "add test... by the method", which usually implies programmatic.

namespace {

void write_ply(const std::string& filename, const std::vector<std::pair<Point3D<float>, Point3D<float>>>& points) {
    FILE* fp = fopen(filename.c_str(), "w");
    fprintf(fp, "ply\n");
    fprintf(fp, "format ascii 1.0\n");
    fprintf(fp, "element vertex %zu\n", points.size());
    fprintf(fp, "property float x\n");
    fprintf(fp, "property float y\n");
    fprintf(fp, "property float z\n");
    fprintf(fp, "property float nx\n");
    fprintf(fp, "property float ny\n");
    fprintf(fp, "property float nz\n");
    fprintf(fp, "end_header\n");
    for (const auto& p : points) {
        fprintf(fp, "%f %f %f %f %f %f\n", p.first.coords[0], p.first.coords[1], p.first.coords[2], p.second.coords[0], p.second.coords[1], p.second.coords[2]);
    }
    fclose(fp);
}

void test_method_sphere() {
    std::cout << "Testing sphere reconstruction by method..." << std::endl;
    const int numPoints = 1000;
    std::vector<std::pair<Point3D<float>, Point3D<float>>> points(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        float u = (float)rand() / (float)RAND_MAX;
        float v = (float)rand() / (float)RAND_MAX;
        float theta = 2.0f * 3.14159f * u;
        float phi = acos(2.0f * v - 1.0f);
        float x = 0.3f * sin(phi) * cos(theta) + 0.5f;
        float y = 0.3f * sin(phi) * sin(theta) + 0.5f;
        float z = 0.3f * cos(phi) + 0.5f;
        points[i].first = Point3D<float>(x, y, z);
        points[i].second = Point3D<float>(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    }
    write_ply("sphere_in.ply", points);
    
    // We will use the system command to run the built executable as it's the most reliable way 
    // to test "the method" including all CLI parsing and setup.
    int ret = system("./surfrecon --in sphere_in.ply --out sphere_out.ply --depth 6 --verbose");
    assert(ret == 0);
    
    // Check if output exists
    FILE* fp = fopen("sphere_out.ply", "r");
    assert(fp != NULL);
    fclose(fp);
    std::cout << "Sphere reconstruction by method PASSED" << std::endl;
}

void test_method_cube() {
    std::cout << "Testing cube reconstruction by method..." << std::endl;
    std::vector<std::pair<Point3D<float>, Point3D<float>>> points;
    auto add_face = [&](Point3D<float> n, Point3D<float> u, Point3D<float> v) {
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                float fu = (i / 19.0f) - 0.5f;
                float fv = (j / 19.0f) - 0.5f;
                Point3D<float> p = n * 0.3f + u * fu * 0.6f + v * fv * 0.6f + Point3D<float>(0.5f, 0.5f, 0.5f);
                points.push_back({p, n});
            }
        }
    };
    add_face({ 1, 0, 0}, {0, 1, 0}, {0, 0, 1});
    add_face({-1, 0, 0}, {0, 1, 0}, {0, 0, 1});
    add_face({ 0, 1, 0}, {1, 0, 0}, {0, 0, 1});
    add_face({ 0,-1, 0}, {1, 0, 0}, {0, 0, 1});
    add_face({ 0, 0, 1}, {1, 0, 0}, {0, 1, 0});
    add_face({ 0, 0,-1}, {1, 0, 0}, {0, 1, 0});
    
    write_ply("cube_in.ply", points);
    int ret = system("./surfrecon --in cube_in.ply --out cube_out.ply --depth 6 --verbose");
    assert(ret == 0);
    
    FILE* fp = fopen("cube_out.ply", "r");
    assert(fp != NULL);
    fclose(fp);
    std::cout << "Cube reconstruction by method PASSED" << std::endl;
}

}

int test_reconstruction_method() {
    test_method_sphere();
    test_method_cube();
    // Clean up
    remove("sphere_in.ply");
    remove("sphere_out.ply");
    remove("cube_in.ply");
    remove("cube_out.ply");
    return 0;
}
