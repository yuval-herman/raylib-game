#include "tau/tau.h"
#include "construct.c"

struct ConstructTestFixture
{
    b2WorldId world_id;
    RandomState *rng;
    Construct *construct;
};

TEST_F_SETUP(ConstructTestFixture)
{
    tau->world_id = physics_make_world();
    tau->rng = random_make();
    tau->construct = construct_make(tau->world_id);
}

TEST_F_TEARDOWN(ConstructTestFixture)
{
    construct_destroy(tau->construct);
    b2DestroyWorld(tau->world_id);
    random_destroy(tau->rng);
}

TEST_F(ConstructTestFixture, construct_finalize)
{
    CHECK_FALSE(construct_is_finalized(tau->construct), "Construct is finalized prematurely");
    Construct_node node1 = {.pos = {.x = 0, .y = 1}, .radius = 1};
    Construct_node node2 = {.pos = {.x = 0, .y = 1}, .radius = 1};
    Construct_joint joint1 = {.node1_idx = 0, .node2_idx = 1, .is_muscle = true};
    construct_add_node(tau->construct, node1);
    construct_add_node(tau->construct, node2);
    construct_add_joint(tau->construct, joint1);
    construct_finalize(tau->construct, tau->rng);
    CHECK_TRUE(construct_is_finalized(tau->construct), "Construct is not finalized after finalization call");
}