#include <fstream>
#include <vector>
#include <iostream>
#include <GL/gl.h>

#pragma pack(push, 1)

struct lump_t {
    int offset;
    int length;
};

const int BSP_LUMPS = 15;

struct dheader_t {
    int version;
    lump_t lumps[BSP_LUMPS];
};

struct dvertex_t {
    float point[3];
};

struct dedge_t {
    unsigned short v[2];
};

struct dface_t {
    short plane_num;
    short side;
    int firstedge;
    short numedges;
    short texinfo;
    unsigned char styles[4];
    int lightofs;
};

#pragma pack(pop)

class BSPLevel {
public:
    std::vector<dvertex_t> vertices;
    std::vector<dedge_t> edges;
    std::vector<int> surfedges;
    std::vector<dface_t> faces;

    bool Load(const char* filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        dheader_t header;
        file.read(reinterpret_cast<char*>(&header), sizeof(dheader_t));

        if (header.version != 29 && header.version != 30) {
            file.close();
            return false;
        }

        ReadLump(file, header.lumps[3], vertices);
        ReadLump(file, header.lumps[12], edges);

        lump_t& seLump = header.lumps[13];
        int seCount = seLump.length / sizeof(int);
        surfedges.resize(seCount);
        file.seekg(seLump.offset, std::ios::beg);
        file.read(reinterpret_cast<char*>(&surfedges[0]), seLump.length);

        ReadLump(file, header.lumps[7], faces);

        file.close();
        return true;
    }

    void Render() {
        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < faces.size(); ++i) {
            const dface_t& face = faces[i];

            for (short j = 0; j < face.numedges; ++j) {
                int surfedgeIndex = surfedges[face.firstedge + j];
                int vertexIndex = 0;

                if (surfedgeIndex > 0) {
                    vertexIndex = edges[surfedgeIndex].v[0];
                } else {
                    vertexIndex = edges[-surfedgeIndex].v[1];
                }

                const float* pt = vertices[vertexIndex].point;
                glVertex3f(pt[0] * 0.03f, pt[2] * 0.03f, -pt[1] * 0.03f);
            }
        }
        glEnd();
    }

private:
    template <typename T>
    void ReadLump(std::ifstream& file, const lump_t& lump, std::vector<T>& outData) {
        int count = lump.length / sizeof(T);
        outData.resize(count);
        file.seekg(lump.offset, std::ios::beg);
        file.read(reinterpret_cast<char*>(&outData[0]), lump.length);
    }
};
