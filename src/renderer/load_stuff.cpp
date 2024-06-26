#include "render.hpp" 

using namespace std;
using namespace glm;
 
tuple<int, int> get_block_xy(int N);

#ifdef DISTANCE_FIELD
struct bp_tex {u8 r; u8 g;};
#else
struct bp_tex {u8 r;};
#endif
//block palette on cpu side is expected to be array of pointers to blocks
void Renderer::update_Block_Palette(Block** blockPalette){
    //block palette is in u8, but for easy copying we convert it to u16 cause block palette is R16_UNIT
    VkDeviceSize bufferSize = (sizeof(bp_tex))*16*BLOCK_PALETTE_SIZE_X* 16*BLOCK_PALETTE_SIZE_Y* 16;
    table3d<bp_tex> blockPaletteLinear = {};
        blockPaletteLinear.allocate(16*BLOCK_PALETTE_SIZE_X, 16*BLOCK_PALETTE_SIZE_Y, 16);
        #ifdef DISTANCE_FIELD
        blockPaletteLinear.set({0,0});
        #else
        blockPaletteLinear.set({0});
        #endif
    for(i32 N=0; N<BLOCK_PALETTE_SIZE; N++){
        for(i32 x=0; x<BLOCK_SIZE; x++){
        for(i32 y=0; y<BLOCK_SIZE; y++){
        for(i32 z=0; z<BLOCK_SIZE; z++){
            auto [block_x, block_y] = get_block_xy(N);
            // blockPaletteLinear(x+16*block_x, y+16*block_y, z) = blockPalette[N].voxels[x][y][z];
            if(blockPalette[N] == NULL) {
                blockPaletteLinear(x+16*block_x, y+16*block_y, z).r = 0;
            }
            else {
                blockPaletteLinear(x+16*block_x, y+16*block_y, z).r = (u16) blockPalette[N]->voxels[x][y][z];
            }
    }}}}
    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    void* data = NULL;

    VK_CHECK(vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL));
    VK_CHECK(vmaMapMemory(VMAllocator, stagingAllocation, &data));

assert(data!=NULL);
assert(bufferSize!=0);
assert(blockPaletteLinear.data()!=NULL);
    memcpy(data, blockPaletteLinear.data(), bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        copy_Buffer(stagingBuffer, origin_block_palette[i].image, uvec3(16*BLOCK_PALETTE_SIZE_X, 16*BLOCK_PALETTE_SIZE_Y, 16), VK_IMAGE_LAYOUT_GENERAL);
    }

    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
}

//TODO: do smth with frames in flight
void Renderer::update_Material_Palette(Material* materialPalette){
    VkDeviceSize bufferSize = sizeof(Material)*256;

    VkBufferCreateInfo stagingBufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        stagingBufferInfo.size = bufferSize;
        stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateInfo stagingAllocInfo = {};
        stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VkBuffer stagingBuffer = {};
    VmaAllocation stagingAllocation = {};
    vmaCreateBuffer(VMAllocator, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, NULL);

    void* data;
    vmaMapMemory(VMAllocator, stagingAllocation, &data);
        memcpy(data, materialPalette, bufferSize);
    vmaUnmapMemory(VMAllocator, stagingAllocation);

    for(i32 i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        copy_Buffer(stagingBuffer, material_palette[i].image, uvec3(6,256,1), VK_IMAGE_LAYOUT_GENERAL);
    }

    vmaDestroyBuffer(VMAllocator, stagingBuffer, stagingAllocation);
}

char* readFileBuffer(const char* path, size_t* size) {
    FILE* fp = fopen(path, "rb");
    *size = _filelength(_fileno(fp));
    char* buffer = (char*) malloc(*size);
    fread(buffer, *size, 1, fp);
    fclose(fp);

    return buffer;
}

// namespace ogt { //this library for some reason uses ogt_ in cases when it will never intersect with others BUT DOES NOT WHEN IT FOR SURE WILL
#include <ogt_vox.hpp>
#include <ogt_voxel_meshify.hpp>
// } //so we'll have to use ogt::ogt_func :(

//TODO: alloca
void Renderer::load_mesh(Mesh* mesh, const char* vox_file, bool _make_vertices, bool extrude_palette){
    size_t buffer_size;
    char* buffer = readFileBuffer(vox_file, &buffer_size);
    const ogt::vox_scene* scene = ogt::vox_read_scene((u8*)buffer, buffer_size);
    free(buffer);

    assert(scene->num_models == 1);

    load_mesh(mesh, (Voxel*)scene->models[0]->voxel_data, scene->models[0]->size_x, scene->models[0]->size_y, scene->models[0]->size_z, _make_vertices);
    
    if(extrude_palette and not has_palette){
        has_palette = true;
        for(int i=0; i<MATERIAL_PALETTE_SIZE; i++){   
            mat_palette[i].color = vec4(
                scene->palette.color[i].r / 256.0,
                scene->palette.color[i].g / 256.0,
                scene->palette.color[i].b / 256.0,
                scene->palette.color[i].a / 256.0
            );
            // mat_palette[i].emmit = scene->materials.matl[i].;
            mat_palette[i].emmit = 0;
            float rough;
            switch (scene->materials.matl[i].type) {
                case ogt::matl_type_diffuse: {
                    rough = 0.85;
                    break;
                }
                case ogt::matl_type_emit: {
                    mat_palette[i].emmit = scene->materials.matl[i].emit * (2.0 + scene->materials.matl[i].flux*2.0);
                    rough = 0.5;
                    break;
                }
                case ogt::matl_type_metal: {
                    rough = (scene->materials.matl[i].rough + (1.0-scene->materials.matl[i].metal))/2.0;
                    break;
                }
                default: {
                    rough = 0;
                    break;
                } 
            }   
            mat_palette[i].rough = rough;
        }
    }
    
/*
roughness
IOR
Specular
Metallic

Emmission
Power - radiant flux
Ldr
*/
    ogt::vox_destroy_scene(scene);
}

void Renderer::load_block(Block** block, const char* vox_file){
    size_t buffer_size;
    char* buffer = readFileBuffer(vox_file, &buffer_size);
    const ogt::vox_scene* scene = ogt::vox_read_scene((u8*)buffer, buffer_size);
    free(buffer);
    assert(scene->num_models == 1);

    if((*block) == NULL) {
        *block = new Block;
    }

    // load_mesh(&(*block)->mesh, (Voxel*)scene->models[0]->voxel_data, scene->models[0]->size_x, scene->models[0]->size_y, scene->models[0]->size_z, true);
    (*block)->mesh.size = ivec3(scene->models[0]->size_x, scene->models[0]->size_y, scene->models[0]->size_z);
    (*block)->mesh.shift = vec3(0);
    (*block)->mesh.rot = quat_identity<float, defaultp>();
    make_vertices(&(*block)->mesh, (Voxel*)scene->models[0]->voxel_data, scene->models[0]->size_x, scene->models[0]->size_y, scene->models[0]->size_z);
    
    for (int x=0; x < scene->models[0]->size_x; x++){
    for (int y=0; y < scene->models[0]->size_y; y++){
    for (int z=0; z < scene->models[0]->size_z; z++){
        (*block)->voxels[x][y][z] = (u16) scene->models[0]->voxel_data[x + y*scene->models[0]->size_x + z*scene->models[0]->size_x*scene->models[0]->size_y];
    }}}
    
    ogt::vox_destroy_scene(scene);
}
void Renderer::free_block(Block** block){
    for(int i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        if(not (*block)->mesh.vertexes.empty()) vmaDestroyBuffer(VMAllocator, (*block)->mesh.vertexes[i].buffer, (*block)->mesh.vertexes[i].alloc);
        if(not (*block)->mesh. indexes.empty()) vmaDestroyBuffer(VMAllocator, (*block)->mesh. indexes[i].buffer, (*block)->mesh. indexes[i].alloc);
    }

    if(not((*block)==NULL)) delete (*block);
}

void Renderer::load_mesh(Mesh* mesh, Voxel* Voxels, int x_size, int y_size, int z_size, bool _make_vertices){
    mesh->size = ivec3(x_size, y_size, z_size);
    if(_make_vertices){
        make_vertices(mesh, Voxels, x_size, y_size, z_size);
    }

    // table3d<Voxel> voxels_extended = {};
    // voxels_extended.allocate(x_size, y_size, z_size);
    // for (int x=0; x < x_size; x++){
    // for (int y=0; y < y_size; y++){
    // for (int z=0; z < z_size; z++){
    //     voxels_extended(x,y,z) = (Voxel) Voxels[x + y*x_size + z*x_size*y_size];
    // }}}

    mesh->voxels = create_RayTrace_VoxelImages(Voxels, mesh->size);
    mesh->shift = vec3(0);
    mesh->rot = quat_identity<float, defaultp>();
    
    // voxels_extended.deallocate();
}
//frees only gpu side stuff, not mesh ptr
void Renderer::free_mesh(Mesh* mesh){
    for(int i=0; i<MAX_FRAMES_IN_FLIGHT; i++){
        if(not mesh->vertexes.empty()) vmaDestroyBuffer(VMAllocator, mesh->vertexes[i].buffer, mesh->vertexes[i].alloc);
        if(not mesh->indexes.empty()) vmaDestroyBuffer(VMAllocator, mesh->indexes[i].buffer, mesh->indexes[i].alloc);
        if(not mesh->voxels.empty()) vmaDestroyImage(VMAllocator, mesh->voxels[i].image, mesh->voxels[i].alloc);
        if(not mesh->voxels.empty()) vkDestroyImageView(device, mesh->voxels[i].view, NULL);
    }
}

void Renderer::make_vertices(Mesh* mesh, Voxel* Voxels, int x_size, int y_size, int z_size){
    ogt::ogt_voxel_meshify_context ctx = {};
    ogt::ogt_mesh* ogt_mesh = ogt::ogt_mesh_from_paletted_voxels_polygon(&ctx, (const u8*)Voxels, x_size, y_size, z_size, NULL);
    ogt::ogt_mesh_remove_duplicate_vertices(&ctx, ogt_mesh);

    mesh->shift = vec3(0);
    mesh->rot = quat_identity<float, defaultp>();
    tie(mesh->vertexes, mesh->indexes) = create_RayGen_VertexBuffers((Vertex*)ogt_mesh->vertices, ogt_mesh->vertex_count, ogt_mesh->indices, ogt_mesh->index_count);
    mesh->icount = ogt_mesh->index_count;
}
