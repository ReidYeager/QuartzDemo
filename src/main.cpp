
#include "sandbox.h"

#include <stdio.h>

QUARTZ_ENTRY(Sandbox)

bool Sandbox::StartDump()
{
  errno_t err = fopen_s(&outDumpFile, "SandboxDump", "w");

  if (err)
  {
    QTZ_ERROR("Sandbox failed to create dump file");
    return false;
  }

  fprintf(outDumpFile, "#include <stdint.h>\n\n");

  return true;
}

void Sandbox::DumpTexture(const char* name, const Quartz::Texture& texture)
{
  fprintf(outDumpFile, "static const inline uint32_t %sWidth = %u;\n", name, texture.extents.width);
  fprintf(outDumpFile, "static const inline uint32_t %sHeight = %u;\n", name, texture.extents.height);
  void* data;
  uint64_t size = texture.Dump_Debug(&data);
  DumpByteArray(name, size, data);
}

void Sandbox::DumpFile(const char* name, const char* path)
{
  FILE* inFile;
  if (fopen_s(&inFile, path, "rb"))
  {
    QTZ_ERROR("Failed to load the file '{}'", path);
    return;
  }

  fseek(inFile, 0, SEEK_END);
  uint64_t inSize = ftell(inFile);
  char* inBuffer = (char*)malloc(inSize);
  rewind(inFile);

  uint64_t readSize = fread(inBuffer, 1, inSize, inFile);
  if (readSize != inSize)
  {
    QTZ_WARNING("Unexpected end of file in '{}'", path);
    inBuffer = (char*)realloc(inBuffer, readSize);
    inSize = readSize;
  }
  fclose(inFile);

  DumpByteArray(name, inSize, inBuffer);
}

void Sandbox::DumpByteArray(const char* name, uint64_t byteCount, void* data)
{
  uint64_t printedCount = 0;
  const uint8_t* bytes = (uint8_t*)data;

  fprintf(outDumpFile, "static const inline uint32_t %sByteCount = %llu;\n", name, byteCount);

  fprintf(outDumpFile, "static const inline uint8_t %sBytes[%llu] = {\n", name, byteCount);

  fprintf(outDumpFile, "%hhu", bytes[0]);
  printf("Dumping \"%s\" : 0%", name);
  for (uint64_t i = 1; i < byteCount; i++)
  {
    if ((i % 16) == 0)
    {
      printf("\rDumping \"%s\" : %5.2f%", name, float(i) / float(byteCount));
      fprintf(outDumpFile, ",\n%hhu", bytes[i]);
    }
    else
    {
      fprintf(outDumpFile, ", %hhu", bytes[i]);
    }
  }
  fprintf(outDumpFile, "};\n");
  printf("\rDumping \"%s\" : Done.\n", name);
}

void Sandbox::DumpText(const char* string)
{
  static uint32_t bufferSize = 65536;
  static char* buffer = (char*)malloc(bufferSize);

  uint32_t count = sprintf_s(buffer, bufferSize, "%s", string);

  fwrite(buffer, count, 1, outDumpFile);
}

void Sandbox::EndDump()
{
  fclose(outDumpFile);
}
