
#include "sandbox.h"

#include <stdio.h>

QUARTZ_ENTRY(Sandbox)

void Sandbox::DumpTexture(const char* resourceRelativePath, const Quartz::Texture& texture)
{
  FILE* outDumpFile;

  char pathBuffer[256];
  sprintf_s(pathBuffer, 256, SANDBOX_RES_DIR "%s", resourceRelativePath);
  errno_t err = fopen_s(&outDumpFile, pathBuffer, "wb");

  if (err)
  {
    QTZ_ERROR("Sandbox failed to create dump file \"{}\"", resourceRelativePath);
    return;
  }

  fwrite(&texture.format,     sizeof(texture.format),     1, outDumpFile);
  fwrite(&texture.usage,      sizeof(texture.usage),      1, outDumpFile);
  fwrite(&texture.filtering,  sizeof(texture.filtering),  1, outDumpFile);
  fwrite(&texture.sampleMode, sizeof(texture.sampleMode), 1, outDumpFile);
  fwrite(&texture.mipLevels,  sizeof(texture.mipLevels),  1, outDumpFile);
  fwrite(&texture.extents,    sizeof(texture.extents),    1, outDumpFile);

  void* data;
  uint64_t size = texture.Dump_Debug(&data);
  fwrite(&size, sizeof(size), 1, outDumpFile);
  fwrite(data, size, 1, outDumpFile);

  fclose(outDumpFile);
}

void Sandbox::DumpMesh(const char* resourceRelativePath, const Quartz::Mesh& mesh)
{
  FILE* outDumpFile;

  char pathBuffer[256];
  sprintf_s(pathBuffer, 256, SANDBOX_RES_DIR "%s", resourceRelativePath);
  errno_t err = fopen_s(&outDumpFile, pathBuffer, "wb");

  if (err)
  {
    QTZ_ERROR("Sandbox failed to create dump file \"{}\"", resourceRelativePath);
    return;
  }

  const Quartz::Vertex* verts;
  const uint32_t* indices;

  uint64_t indexCount;
  uint64_t vertCount;

  mesh.Dump(&vertCount, &verts, &indexCount, &indices);

  fwrite(&vertCount, sizeof(vertCount), 1, outDumpFile);
  fwrite(verts, sizeof(*verts), vertCount, outDumpFile);

  fwrite(&indexCount, sizeof(indexCount), 1, outDumpFile);
  fwrite(indices, sizeof(*indices), indexCount, outDumpFile);

  fclose(outDumpFile);
}
