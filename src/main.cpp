#include <random>
// #include "render/lumal/macros.hpp"

#define ATRACE(_S)
import std;
import ecm;
import lum;
import glm;
import glfw;
import input;

using namespace std;
using namespace glm;

enum class GameAction : char {
    MOVE_CAMERA_FORWARD,
    MOVE_CAMERA_BACKWARD,
    MOVE_CAMERA_LEFT,
    MOVE_CAMERA_RIGHT,
    TURN_CAMERA_LEFT,
    TURN_CAMERA_RIGHT,
    INCREASE_ZOOM,
    DECREASE_ZOOM,
    SHOOT,
    MOVE_TANK_FORWARD,
    MOVE_TANK_BACKWARD,
    TURN_TANK_LEFT,
    TURN_TANK_RIGHT,

    // TODO: i guess?..
    SET_BLOCK_1,
    SET_BLOCK_2,
    SET_BLOCK_3,
    SET_BLOCK_4,
    SET_BLOCK_5,
    SET_BLOCK_6,
    SET_BLOCK_7,
    SET_BLOCK_8,
    SET_BLOCK_9,
    SET_BLOCK_0,
    SET_BLOCK_F1,
    SET_BLOCK_F2,
    SET_BLOCK_F3,
    SET_BLOCK_F4,
    SET_BLOCK_F5,

    LAST_ACTION // MAKE SURE THIS IS THE LAST ITEM
};

constexpr GameAction operator+(GameAction lhs, int rhs) {
    using underlying = std::underlying_type_t<GameAction>;
    return static_cast<GameAction>(static_cast<underlying>(lhs) + rhs);
}
constexpr GameAction operator-(GameAction lhs, int rhs) {
    using underlying = std::underlying_type_t<GameAction>;
    return static_cast<GameAction>(static_cast<underlying>(lhs) - rhs);
}

constexpr GameAction& operator++(GameAction& action) {
    using underlying = std::underlying_type_t<GameAction>;
    action = static_cast<GameAction>(static_cast<underlying>(action) + 1);
    return action;
}
constexpr GameAction& operator--(GameAction& action) {
    using underlying = std::underlying_type_t<GameAction>;
    action = static_cast<GameAction>(static_cast<underlying>(action) - 1);
    return action;
}

class Input : public Lum::InputHandler<GameAction> {
  public:
    Input() {};
    //  handler;
};

vec3 rnVec3(float minValue, float maxValue);
void printFPS();
void setup_input(Input& input, Lum::Renderer& render);
void process_animations(Lum::Renderer& lum);
void process_physics(Lum::Renderer& lum);
void cleanup(Lum::Renderer& lum);
quat find_quat(vec3 v1, vec3 v2);

// for simplicity lets stick to this
Lum::MeshModel tank_body = {};
Lum::MeshTransform tank_body_trans = {};
Lum::MeshModel tank_head = {};
Lum::MeshTransform tank_head_trans = {};
float physical_body_height;
float interpolated_body_height;
vec3 tank_direction_forward, tank_direction_right;

// Legs are controlled with Lum::ECS. Each leg is an entity, they have components, and animation
// systems, defined below

// defining ECS components
typedef struct {
    vec2 pos;
} physical_leg_point;
typedef struct {
    vec2 pos;
} interpolated_leg_point;
typedef struct {
    vec2 pos;
} target_leg_point;
typedef struct {
    vec3 pos;
} leg_center;
typedef struct {
    vec3 pos;
} leg_joint_shift;
typedef struct {
    vec3 pos;
} leg_point;
typedef struct {
    quat rot;
} rotation_mul;

// defining ECS functions

// just sequance of functions to actually animate.
// in real world you would balance their cache usage (e.g. divide/group to load as mglm::any
// data/functions as possible while staying in L1 cache)
void calculateLegJointsAndRotations(
    leg_point& leg, leg_center& center, Lum::MeshTransform& trans, rotation_mul& mul) {
    leg.pos = tank_body_trans.shift + tank_body_trans.rot * center.pos;
    trans.rot = mul.rot * tank_body_trans.rot;
}
void calculateAndUpdateLegPoints(
    leg_point& leg, target_leg_point& target, physical_leg_point& physical, leg_center& center) {
    vec3 joint_pos = leg.pos;
    target.pos = vec2(joint_pos) + vec2(tank_direction_right) +
        vec2(tank_direction_forward) * 3.0f * (float(rand()) / float(RAND_MAX));

    if (distance(physical.pos, target.pos) > 6.0f) {
        physical.pos = target.pos;
    }
}
void interpolateAndCalculateLegRotation(interpolated_leg_point& interpolated,
    physical_leg_point& physical, Lum::MeshTransform& leg_trans, leg_point& leg) {
    interpolated.pos = mix(interpolated.pos, physical.pos, 0.6f);

    vec2 leg_direction = normalize(interpolated.pos - vec2(leg.pos));
    leg_trans.rot *= find_quat(tank_direction_right, vec3(leg_direction, 0));
}
void applyShiftAndDrawLeg(Lum::MeshTransform& leg_trans, leg_point& leg, leg_joint_shift& shift,
    Lum::MeshModel& leg_mesh) {
    vec3 leg_joint_shift = leg_trans.rot * shift.pos;
    leg_trans.shift = leg.pos - leg_joint_shift;
}
Lum::Renderer* lum_ptr;
void drawLeg(Lum::MeshTransform& leg_trans, Lum::MeshModel& leg_mesh) {
    lum_ptr->drawModel(leg_mesh, leg_trans);
}

int main() {
    // input system i designed. You can use, but it is not neccessary for Lum::Renderer
    Input input;
    // ecs system i designed. You can use, but it is not neccessary for Lum::Renderer
    auto anim_manager = Lum::ECManager<Lum::MeshModel, Lum::MeshTransform, leg_point,
        leg_joint_shift, target_leg_point, physical_leg_point, interpolated_leg_point, leg_center,
        rotation_mul>();
    auto anim_system = Lum::ECSystem(anim_manager, calculateLegJointsAndRotations,
        calculateAndUpdateLegPoints, interpolateAndCalculateLegRotation, applyShiftAndDrawLeg);

    ATRACE();
    // creating legs entities. Lum::EntityID is an opaque handle (index. Actually, indirect index)
    Lum::EntityID rf_leg = anim_system.createEntity(anim_manager);
    Lum::EntityID lf_leg = anim_system.createEntity(anim_manager);
    Lum::EntityID rb_leg = anim_system.createEntity(anim_manager);
    Lum::EntityID lb_leg = anim_system.createEntity(anim_manager);
    // setting up constants
    ATRACE();
    anim_system.getEntityComponent<leg_center&>(anim_manager, rf_leg) = { vec3(14.0, 19.5, 6.5) };
    anim_system.getEntityComponent<leg_center&>(anim_manager, lb_leg) = { vec3(3.0, 3.5, 6.5) };
    anim_system.getEntityComponent<leg_center&>(anim_manager, lf_leg) = { vec3(3.0, 19.5, 6.5) };
    anim_system.getEntityComponent<leg_center&>(anim_manager, rb_leg) = { vec3(14.0, 3.5, 6.5) };
    anim_system.getEntityComponent<leg_joint_shift&>(
        anim_manager, rf_leg) = { vec3(10.0, 6.5, 12.5) };
    anim_system.getEntityComponent<leg_joint_shift&>(
        anim_manager, lb_leg) = { vec3(10.0, 6.5, 12.5) };
    anim_system.getEntityComponent<leg_joint_shift&>(
        anim_manager, lf_leg) = { vec3(0.0, 6.5, 12.5) };
    anim_system.getEntityComponent<leg_joint_shift&>(
        anim_manager, rb_leg) = { vec3(0.0, 6.5, 12.5) };
    anim_system.getEntityComponent<rotation_mul&>(
        anim_manager, rf_leg) = { quat(-vec3(0, 0, glm::pi<float>())) };
    anim_system.getEntityComponent<rotation_mul&>(
        anim_manager, lb_leg) = { glm::quat_identity<float, defaultp>() };
    anim_system.getEntityComponent<rotation_mul&>(
        anim_manager, lf_leg) = { quat(-vec3(0, 0, glm::pi<float>())) };
    anim_system.getEntityComponent<rotation_mul&>(
        anim_manager, rb_leg) = { glm::quat_identity<float, defaultp>() };

    ATRACE();
    Lum::Settings settings = {};
    settings.fullscreen = false;
    settings.vsync = false;
    settings.world_size = ivec3(48, 48, 16);
    settings.static_block_palette_size = 15;
    settings.maxParticleCount = 8128;
    Lum::Renderer lum = Lum::Renderer(15, 4096, 64, 64, 64);
    lum_ptr = &lum;

    // ATTENTION: all foliage has to be declared BEFORE init()
    // this restriction just makes everything 100x simpler
    // i might implement dynamic Vulkan resources in future, but it will only hurt perfomance until
    // ~20k foliage meshes and you are also supposed to compile shader to SPIRV yourself (for GLSL,
    // use glslang / shaderc)
    Lum::MeshFoliage grass = lum.loadFoliage("shaders/compiled/grass.vert.spv", 6, 10);

    ATRACE();
    lum.init(settings);
    lum.loadWorld("assets/scene");

    ATRACE();
    // preparation stage. These functions also can be called in runtime
    // If no palette is found, first models defines it
    // this DOES set palette
    tank_body = lum.loadModel("assets/tank_body.vox", /*extract palette if no found = */ true);
    ATRACE();
    tank_body_trans.shift += vec3(13.1, 14.1, 3.1) * 16.0f;
    // this DOES NOT set palette cause already setten.
    tank_head = lum.loadModel("assets/tank_head.vox", /*extract palette if no found = */ true);
    // material palette can also be loaded from alone
    // lum.loadPalette("my_magicavox_filescene_with_palette_i_want_to_extract.vox")
    // good way to handle this would be to have voxel for each material placed in scene to view them
    ATRACE();
    anim_system.getEntityComponent<Lum::MeshModel&>(anim_manager, rf_leg) =
        lum.loadModel("assets/tank_rf_lb_leg.vox");
    anim_system.getEntityComponent<Lum::MeshModel&>(anim_manager, lf_leg) =
        lum.loadModel("assets/tank_lf_rb_leg.vox");
    anim_system.getEntityComponent<Lum::MeshModel&>(anim_manager, rb_leg) =
        lum.loadModel("assets/tank_lf_rb_leg.vox");
    anim_system.getEntityComponent<Lum::MeshModel&>(anim_manager, lb_leg) =
        lum.loadModel("assets/tank_rf_lb_leg.vox");

    ATRACE();
    Lum::MeshLiquid water = lum.loadLiquid(69, 42);
    Lum::MeshVolumetric smoke = lum.loadVolumetric(1, .5, {});

    ATRACE();
    lum.loadBlock(1, "assets/dirt.vox");
    lum.loadBlock(2, "assets/grass.vox");
    lum.loadBlock(3, "assets/grassNdirt.vox");
    lum.loadBlock(4, "assets/stone_dirt.vox");
    lum.loadBlock(5, "assets/bush.vox");
    lum.loadBlock(6, "assets/leaves.vox");
    lum.loadBlock(7, "assets/iron.vox");
    lum.loadBlock(8, "assets/lamp.vox");
    lum.loadBlock(9, "assets/stone_brick.vox");
    lum.loadBlock(10, "assets/stone_brick_cracked.vox");
    lum.loadBlock(11, "assets/stone_pack.vox");
    lum.loadBlock(12, "assets/bark.vox");
    lum.loadBlock(13, "assets/wood.vox");
    lum.loadBlock(14, "assets/planks.vox");
    // total 15 max blocks specified, so loadBlock(15) is illegal

    // literally uploads data to gpu. You can call them in runtime, but atm its not recommended (a
    // lot of overhead curently, can be significantly reduced)
    ATRACE();
    lum.uploadBlockPaletteToGPU();
    lum.uploadMaterialPaletteToGPU();

    lum.waitIdle();

    // callback functions for glfw action input system
    ATRACE();
    setup_input(input, lum);

    while (not lum.should_close) {
        input.pollUpdates();
        glfw::pollEvents();
        lum.should_close |= glfw::windowShouldClose((glfw::GLFWwindow*) lum.getGLFWptr());
        lum.should_close |=
            (glfw::getKey((glfw::GLFWwindow*) lum.getGLFWptr(), glfw::KEY_ESCAPE) == glfw::PRESS);

        process_physics(lum);
        process_animations(lum);
        anim_system.update(anim_manager); // also animations

        lum.startFrame();
        // this *could* be implicit
        lum.drawWorld();
        lum.drawParticles();

        lum.drawModel(tank_body, tank_body_trans);
        lum.drawModel(tank_head, tank_head_trans);
        // lum.drawModel(tank_rf_leg, tank_rf_leg_trans);
        // lum.drawModel(tank_lf_leg, tank_lf_leg_trans);
        // lum.drawModel(tank_rb_leg, tank_rb_leg_trans);
        // lum.drawModel(tank_lb_leg, tank_lb_leg_trans);
        anim_system.updateSpecific(anim_manager, drawLeg);

        // literally procedural grass placement every frame. You probably want to store it as
        // entities in your own structures
        for (int xx = 4; xx < 20; xx++) {
            for (int yy = 4; yy < 20; yy++) {
                if ((xx >= 5) and (xx < 12) and (yy >= 6) and (yy < 16))
                    continue;
                ivec3 pos = ivec3(xx * 16, yy * 16, 16);
                lum.drawFoliageBlock(grass, pos);
            }
        }

        // literally procedural water placement every frame. You probably want to store it as
        // entities in your own structures
        for (int xx = 5; xx < 12; xx++) {
            for (int yy = 6; yy < 16; yy++) {
                ivec3 pos = ivec3(xx * 16, yy * 16, 14);
                lum.drawLiquidBlock(water, pos);
                lum.opaque_members->render.specialRadianceUpdates.push_back(
                    i8vec4(ivec3(xx, yy, 2), 0));
                lum.opaque_members->render.specialRadianceUpdates.push_back(
                    i8vec4(ivec3(xx, yy, 1), 0));
            }
        }

        // literally procedural smoke placement every frame. You probably want to store it as
        // entities in your own structures
        for (int xx = 8; xx < 10; xx++) {
            for (int yy = 10; yy < 13; yy++) {
                ivec3 pos = ivec3(xx * 16, yy * 16, 20);
                lum.drawVolumetricBlock(smoke, pos);
                // render.render.specialRadianceUpdates.push_back(i8vec4(ivec3(xx,yy,2),0));
                // render.render.specialRadianceUpdates.push_back(i8vec4(ivec3(xx,yy,1),0));
            }
        }

        lum.prepareFrame();
        lum.submitFrame();
    }

    // prints GPU profile data
    if (lum.opaque_members->render.lumal.settings.profile) {
        printf(
            "%-22s: %7.3f: %7.3f\n", lum.opaque_members->render.lumal.timestampNames[0], 0.0, 0.0);
        for (int i = 1; i < lum.opaque_members->render.lumal.currentTimestamp; i++) {
            double time_point = double(lum.opaque_members->render.lumal.average_ftimestamps[i] -
                lum.opaque_members->render.lumal.average_ftimestamps[0]);
            double time_diff = double(lum.opaque_members->render.lumal.average_ftimestamps[i] -
                lum.opaque_members->render.lumal.average_ftimestamps[i - 1]);
            printf("%3d %-22s: %7.3f: %7.3f\n", i,
                lum.opaque_members->render.lumal.timestampNames[i], time_point, time_diff);
        }
    }
    cleanup(lum);
}

// binding keys to some functions
void setup_input(Input& input, Lum::Renderer& render) {
    input.setup((glfw::GLFWwindow*) render.getGLFWptr());

    input.rebindKey(GameAction::MOVE_CAMERA_FORWARD, glfw::KEY_W);
    input.rebindKey(GameAction::MOVE_CAMERA_BACKWARD, glfw::KEY_S);
    input.rebindKey(GameAction::MOVE_CAMERA_LEFT, glfw::KEY_A);
    input.rebindKey(GameAction::MOVE_CAMERA_RIGHT, glfw::KEY_D);
    input.rebindKey(GameAction::TURN_CAMERA_LEFT, glfw::KEY_COMMA);
    input.rebindKey(GameAction::TURN_CAMERA_RIGHT, glfw::KEY_PERIOD);

    input.rebindKey(GameAction::INCREASE_ZOOM, glfw::KEY_PAGE_UP);
    input.rebindKey(GameAction::DECREASE_ZOOM, glfw::KEY_PAGE_DOWN);

    input.rebindKey(GameAction::SHOOT, glfw::KEY_ENTER);
    input.rebindKey(GameAction::MOVE_TANK_FORWARD, glfw::KEY_UP);
    input.rebindKey(GameAction::MOVE_TANK_BACKWARD, glfw::KEY_DOWN);
    input.rebindKey(GameAction::TURN_TANK_LEFT, glfw::KEY_LEFT);
    input.rebindKey(GameAction::TURN_TANK_RIGHT, glfw::KEY_RIGHT);

    input.rebindKey(GameAction::SET_BLOCK_1, glfw::KEY_1);
    input.rebindKey(GameAction::SET_BLOCK_2, glfw::KEY_2);
    input.rebindKey(GameAction::SET_BLOCK_3, glfw::KEY_3);
    input.rebindKey(GameAction::SET_BLOCK_4, glfw::KEY_4);
    input.rebindKey(GameAction::SET_BLOCK_5, glfw::KEY_5);
    input.rebindKey(GameAction::SET_BLOCK_6, glfw::KEY_6);
    input.rebindKey(GameAction::SET_BLOCK_7, glfw::KEY_7);
    input.rebindKey(GameAction::SET_BLOCK_8, glfw::KEY_8);
    input.rebindKey(GameAction::SET_BLOCK_9, glfw::KEY_9);
    input.rebindKey(GameAction::SET_BLOCK_0, glfw::KEY_0);
    input.rebindKey(GameAction::SET_BLOCK_F1, glfw::KEY_F1);
    input.rebindKey(GameAction::SET_BLOCK_F2, glfw::KEY_F2);
    input.rebindKey(GameAction::SET_BLOCK_F3, glfw::KEY_F3);
    input.rebindKey(GameAction::SET_BLOCK_F4, glfw::KEY_F4);
    input.rebindKey(GameAction::SET_BLOCK_F5, glfw::KEY_F5);

    // Continuous is default
    input.setActionType(GameAction::SHOOT, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_1, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_2, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_3, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_4, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_5, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_6, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_7, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_8, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_9, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_0, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_F1, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_F2, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_F3, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_F4, Lum::ActionType::OneShot);
    input.setActionType(GameAction::SET_BLOCK_F5, Lum::ActionType::OneShot);

    // bind action callbacks
    input.setActionCallback(GameAction::MOVE_CAMERA_FORWARD, [&](GameAction action) {
        render.getCamera().cameraPos += render.delta_time *
            dvec3(dvec2(render.getCamera().cameraDir), 0) * 400.5 /
            render.getCamera().pixelsInVoxel;
    });

    input.setActionCallback(GameAction::MOVE_CAMERA_BACKWARD, [&](GameAction action) {
        render.getCamera().cameraPos -= render.delta_time *
            dvec3(dvec2(render.getCamera().cameraDir), 0) * 400.5 /
            render.getCamera().pixelsInVoxel;
    });

    input.setActionCallback(GameAction::MOVE_CAMERA_LEFT, [&](GameAction action) {
        dvec3 camera_direction_to_right =
            glm::dquat(dvec3(0.0, 0.0, glm::pi<double>() / 2.0)) * render.getCamera().cameraDir;
        render.getCamera().cameraPos += render.delta_time *
            dvec3(dvec2(camera_direction_to_right), 0) * 400.5 / render.getCamera().pixelsInVoxel;
    });
    input.setActionCallback(GameAction::MOVE_CAMERA_RIGHT, [&](GameAction action) {
        dvec3 camera_direction_to_right =
            glm::dquat(dvec3(0.0, 0.0, glm::pi<double>() / 2.0)) * render.getCamera().cameraDir;
        render.getCamera().cameraPos -= render.delta_time *
            dvec3(dvec2(camera_direction_to_right), 0) * 400.5 / render.getCamera().pixelsInVoxel;
    });

    input.setActionCallback(GameAction::TURN_CAMERA_LEFT, [&](GameAction action) {
        render.getCamera().cameraDir =
            rotate(glm::identity<dmat4>(), +0.60 * render.delta_time, dvec3(0, 0, 1)) *
            dvec4(render.getCamera().cameraDir, 0);
        render.getCamera().cameraDir = normalize(render.getCamera().cameraDir);
    });
    input.setActionCallback(GameAction::TURN_CAMERA_RIGHT, [&](GameAction action) {
        render.getCamera().cameraDir =
            rotate(glm::identity<dmat4>(), -0.60 * render.delta_time, dvec3(0, 0, 1)) *
            dvec4(render.getCamera().cameraDir, 0);
        render.getCamera().cameraDir = normalize(render.getCamera().cameraDir);
    });

    input.setActionCallback(GameAction::MOVE_TANK_FORWARD, [&](GameAction action) {
        vec3 tank_direction_forward = tank_body_trans.rot * vec3(0, 1, 0);
        float tank_speed = 50.0 * render.delta_time;
        tank_body_trans.shift += tank_direction_forward * tank_speed;
    });
    input.setActionCallback(GameAction::MOVE_TANK_BACKWARD, [&](GameAction action) {
        vec3 tank_direction_forward = tank_body_trans.rot * vec3(0, 1, 0);
        float tank_speed = 50.0 * render.delta_time;
        tank_body_trans.shift -= tank_direction_forward * tank_speed;
    });

    input.setActionCallback(GameAction::TURN_TANK_LEFT, [&](GameAction action) {
        // assert(tank_body);
        quat old_rot = tank_body_trans.rot;
        tank_body_trans.rot *= quat(vec3(0, 0, +0.05f * 50.0f * render.delta_time));
        quat new_rot = tank_body_trans.rot;

        vec3 old_center = old_rot * vec3(tank_body.getSize()) / 2.0f;
        vec3 new_center = new_rot * vec3(tank_body.getSize()) / 2.0f;

        vec3 difference = new_center - old_center;
        tank_body_trans.shift -= difference;

        tank_body_trans.rot = normalize(tank_body_trans.rot);
    });

    input.setActionCallback(GameAction::TURN_TANK_RIGHT, [&](GameAction action) {
        quat old_rot = tank_body_trans.rot;
        tank_body_trans.rot *= quat(vec3(0, 0, -0.05f * 50.0f * render.delta_time));
        quat new_rot = tank_body_trans.rot;

        vec3 old_center = old_rot * vec3(tank_body.getSize()) / 2.0f;
        vec3 new_center = new_rot * vec3(tank_body.getSize()) / 2.0f;

        vec3 difference = new_center - old_center;
        tank_body_trans.shift -= difference;

        tank_body_trans.rot = normalize(tank_body_trans.rot);
    });

    for (int i = 1; i < 10; ++i) {
        input.setActionCallback(
            static_cast<GameAction>(GameAction::SET_BLOCK_1 + (i - 1)), [&, i](GameAction action) {
                ivec3 block_to_set = ivec3(vec3(tank_body_trans.shift) +
                                         tank_body_trans.rot * (vec3(tank_body.getSize()) / 2.0f)) /
                    16;
                block_to_set =
                    clamp(block_to_set, ivec3(0), ivec3(render.getWorldSize()) - ivec3(1));
                render.setWorldBlock(block_to_set.x, block_to_set.y, block_to_set.z, i);
            });
        input.rebindKey(
            static_cast<GameAction>(GameAction::SET_BLOCK_1 + (i - 1)), glfw::KEY_0 + i);
    }

    // for 0 key (special case for air block)
    input.setActionCallback(GameAction::SET_BLOCK_0, [&](GameAction action) {
        ivec3 block_to_set = ivec3(vec3(tank_body_trans.shift) +
                                 tank_body_trans.rot * (vec3(tank_body.getSize()) / 2.0f)) /
            16;
        block_to_set = clamp(block_to_set, ivec3(0), ivec3(render.getWorldSize()) - ivec3(1));
        render.setWorldBlock(block_to_set.x, block_to_set.y, block_to_set.z - 1, 0);
    });
    input.rebindKey(GameAction::SET_BLOCK_0, glfw::KEY_0);

    // For F1-F5 keys (glfw::KEY_F1 to glfw::KEY_F5)
    for (int i = 0; i < 5; ++i) {
        input.setActionCallback(
            static_cast<GameAction>(GameAction::SET_BLOCK_F1 + i), [&, i](GameAction action) {
                ivec3 block_to_set = ivec3(vec3(tank_body_trans.shift) +
                                         tank_body_trans.rot * (vec3(tank_body.getSize()) / 2.0f)) /
                    16;
                block_to_set =
                    clamp(block_to_set, ivec3(0), ivec3(render.getWorldSize()) - ivec3(1));
                render.setWorldBlock(block_to_set.x, block_to_set.y, block_to_set.z, 10 + i);
            });
        input.rebindKey(static_cast<GameAction>(GameAction::SET_BLOCK_F1 + i), glfw::KEY_F1 + i);
    }

    input.setActionCallback(GameAction::SHOOT, [&](GameAction action) {
        Lum::Particle p = {};
        p.lifeTime = 12.0;
        p.pos = tank_head_trans.rot * vec3(16.5, 3, 9.5) + tank_head_trans.shift;
        p.matID = 249;
        p.vel = (rnVec3(0, 1) * 2.f - 1.f) * .5f + tank_head_trans.rot * vec3(0, -1, 0) * 100.f;
        render.addParticle(p);

        p.lifeTime = 2.5;
        p.matID = 19;
        for (int i = 0; i < 30; i++) {
            p.vel = (rnVec3(0, 1) * 2.f - 1.f) * 25.f;
            render.addParticle(p);
        }
    });

    input.setActionCallback(GameAction::INCREASE_ZOOM,
        [&](GameAction action) { render.getCamera().pixelsInVoxel *= 1.0 + render.delta_time; });
    input.setActionCallback(GameAction::DECREASE_ZOOM,
        [&](GameAction action) { render.getCamera().pixelsInVoxel /= 1.0 + render.delta_time; });
}

void printFPS() {
    static int frame_count = 0;
    static auto last_print = std::chrono::high_resolution_clock::now();
    static auto last_frame = std::chrono::high_resolution_clock::now();
    static double best_mspf = 1;
    static double worst_mspf = 0;

    frame_count++;

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> last_print_elapsed = current_time - last_print;
    std::chrono::duration<double> last_frame_elapsed = current_time - last_frame;

    best_mspf = std::min(best_mspf, last_frame_elapsed.count());
    worst_mspf = std::max(best_mspf, last_frame_elapsed.count());

    if (last_print_elapsed.count() >= 1.0) {
        std::cout << ", " << frame_count;
        std::cout << ", " << 1000.0 * best_mspf;
        std::cout << ", " << 1000.0 * worst_mspf;
        frame_count = 0;
        last_print = current_time;

        best_mspf = 1;
        worst_mspf = 0;
    }
    last_frame = current_time;
}

vec3 rnVec3(float minValue, float maxValue) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(minValue, maxValue);

    float x = dis(gen);
    float y = dis(gen);
    float z = dis(gen);

    return glm::vec3(x, y, z);
}

void cleanup(Lum::Renderer& lum) {
    // save_scene functions are not really supposed to be used, i implemented them for demo
    // this is not api.
    lum.opaque_members->render.save_scene("assets/scene");

    // automatically frees all allocated blocks and meshes
    lum.cleanup();
}

int findBlockUnderTank(Lum::Renderer& lum, const ivec2& block_center, int block_under_body) {
    int selected_block = 0;
    for (int i = -1; i <= 1; ++i) {
        if (lum.getWorldBlock(block_center.x, block_center.y, block_under_body + i) != 0) {
            return block_under_body + i;
        }
    }
    for (selected_block = lum.getWorldSize().z - 1; selected_block >= 0; --selected_block) {
        if (lum.getWorldBlock(block_center.x, block_center.y, selected_block) != 0) {
            break;
        }
    }
    return selected_block;
}

void process_physics(Lum::Renderer& lum) {
    vec3 tank_body_center = tank_body_trans.rot * vec3(8.5, 12.5, 0) + tank_body_trans.shift;
    ivec2 tank_body_center_in_blocks = ivec2(tank_body_center) / 16;
    int block_under_body = int(tank_body_center.z) / 16;

    if (glm::any(lessThan(tank_body_center_in_blocks, ivec2(0))))
        return;
    if (glm::any(greaterThanEqual(tank_body_center_in_blocks, ivec2(lum.getWorldSize()))))
        return;

    int selected_block = 0;

    if (lum.getWorldBlock(tank_body_center_in_blocks.x, tank_body_center_in_blocks.y,
            block_under_body + 1) != 0) {
        selected_block = block_under_body + 1;
    } else if (lum.getWorldBlock(tank_body_center_in_blocks.x, tank_body_center_in_blocks.y,
                   block_under_body) != 0) {
        selected_block = block_under_body;
    } else if (lum.getWorldBlock(tank_body_center_in_blocks.x, tank_body_center_in_blocks.y,
                   block_under_body - 1) != 0) {
        selected_block = block_under_body - 1;
    } else {
        for (selected_block = lum.getWorldSize().z - 1; selected_block >= 0; selected_block--) {
            Lum::MeshBlock blockId = lum.getWorldBlock(
                tank_body_center_in_blocks.x, tank_body_center_in_blocks.y, selected_block);

            if (blockId != 0)
                break;
        }
    }

    physical_body_height = float(selected_block) * 16.0 + 16.0;
}

quat find_quat(vec3 v1, vec3 v2) {
    quat q;
    vec3 a = cross(v1, v2);
    q = a;
    q.w = sqrt((length(v1) * length(v1)) * (length(v2) * length(v2))) + dot(v1, v2);

    return normalize(q);
}

// some inverse kinematics for body&head
void process_animations(Lum::Renderer& lum) {
    tank_direction_forward = tank_body_trans.rot * vec3(0, 1, 0);
    tank_direction_right = tank_body_trans.rot * vec3(1, 0, 0);

    Lum::Particle p = {};
    p.lifeTime = 10.0 * (float(rand()) / float(RAND_MAX));
    p.pos = tank_head_trans.rot * vec3(17, 42, 27) + tank_head_trans.shift;
    p.vel = (rnVec3(0, 1) * 2.f - 1.f) * 1.1f;
    p.matID = 79;
    lum.addParticle(p);

    float interpolation = glm::clamp(float(lum.delta_time) * 4.20f);
    tank_head_trans.rot = normalize(glm::mix(tank_head_trans.rot,
        quat(vec3(0, 0, glm::pi<float>())) * tank_body_trans.rot, interpolation));
    interpolated_body_height =
        glm::mix(interpolated_body_height, physical_body_height, interpolation);
    tank_body_trans.shift.z = interpolated_body_height;

    vec3 body_head_joint_shift = tank_body_trans.rot * vec3(8.5, 12.5, tank_body.getSize().z);
    vec3 head_joint_shift = tank_head_trans.rot * vec3(33 / 2.0, 63 / 2.0, 0);
    vec3 head_joint = tank_body_trans.shift + body_head_joint_shift;

    tank_head_trans.shift = head_joint - head_joint_shift;
}