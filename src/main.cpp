#include <flecs.h>

struct map{
    double x;
    double y;
    double z;
};

int main()
{
    flecs::world ecs;
    
   auto system = ecs.system<map>()
        .kind(flecs::OnSet)
        .each([](flecs::entity e, map & m) {
            printf("x: %f, y: %f, z: %f", m.x, m.y, m.z);
        });
    ecs.entity()
        .set<map>({1, 2, 3});
    ecs.progress();
    system.run();
    return 0;
}