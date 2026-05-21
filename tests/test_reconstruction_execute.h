#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include "../src/Geometry.h"
#include "../src/PointStream.h"

namespace {

std::string find_executable(const char* name) {
    const char* build_dir = std::getenv("POISSON_SURFER_BUILD_DIR");
    if (build_dir) {
        std::string path = std::string(build_dir) + "/" + name;
        if (FILE* fp = fopen(path.c_str(), "r")) { fclose(fp); return path; }
    }
    return std::string("./") + name;
}

int run_command(const std::string& cmd, std::string& output) {
    std::string full_cmd = cmd + " 2>&1";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(full_cmd.c_str(), "r"), pclose);
    if (!pipe) return -1;
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe.get())) output += buf;
    return pclose(pipe.get());
}

struct FileCleanup {
    std::vector<std::string> files;
    ~FileCleanup() { for (const auto& f : files) remove(f.c_str()); }
};

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

void test_method_sphere(FileCleanup& cleanup) {
    std::cout << "Testing sphere reconstruction by method..." << std::endl;
    const int numPoints = 1000;
    std::vector<std::pair<Point3D<float>, Point3D<float>>> points(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        float u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float theta = 2.0f * 3.14159f * u;
        float phi = acosf(2.0f * v - 1.0f);
        float x = 0.3f * sin(phi) * cos(theta) + 0.5f;
        float y = 0.3f * sin(phi) * sin(theta) + 0.5f;
        float z = 0.3f * cos(phi) + 0.5f;
        points[i].first = Point3D<float>(x, y, z);
        points[i].second = Point3D<float>(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    }
    write_ply("sphere_in.ply", points);
    cleanup.files.insert(cleanup.files.end(), {"sphere_in.ply", "sphere_out.ply"});
    
    std::string surfrecon = find_executable("surfrecon");
    std::string cmd = surfrecon + " --in sphere_in.ply --out sphere_out.ply --depth 6 --verbose";
    std::string output;
    int ret = run_command(cmd, output);
    if (ret != 0) {
        std::cerr << "Sphere reconstruction FAILED (exit=" << ret << ")\n" << output << std::endl;
        std::abort();
    }
    
    FILE* fp = fopen("sphere_out.ply", "r");
    if (!fp) {
        std::cerr << "Sphere reconstruction FAILED: output file not found\n" << output << std::endl;
        std::abort();
    }
    fclose(fp);
    std::cout << "Sphere reconstruction by method PASSED" << std::endl;
}

void test_method_cube(FileCleanup& cleanup) {
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
    cleanup.files.insert(cleanup.files.end(), {"cube_in.ply", "cube_out.ply"});
    
    std::string surfrecon = find_executable("surfrecon");
    std::string cmd = surfrecon + " --in cube_in.ply --out cube_out.ply --depth 6 --verbose";
    std::string output;
    int ret = run_command(cmd, output);
    if (ret != 0) {
        std::cerr << "Cube reconstruction FAILED (exit=" << ret << ")\n" << output << std::endl;
        std::abort();
    }
    
    FILE* fp = fopen("cube_out.ply", "r");
    if (!fp) {
        std::cerr << "Cube reconstruction FAILED: output file not found\n" << output << std::endl;
        std::abort();
    }
    fclose(fp);
    std::cout << "Cube reconstruction by method PASSED" << std::endl;
}

}

int test_reconstruction_method() {
    FileCleanup cleanup;
    test_method_sphere(cleanup);
    test_method_cube(cleanup);
    std::cout << "All reconstruction method tests PASSED" << std::endl;
    return 0;
}
