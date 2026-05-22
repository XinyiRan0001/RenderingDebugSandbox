#ifndef ROPE_H
#define ROPE_H

#include <vector>

#include "CGL/CGL.h"
#include "mass.h"
#include "spring.h"

namespace CGL {

    class Rope {
    public:

        Rope(std::vector<Mass*>& masses,
            std::vector<Spring*>& springs)
            : masses(masses), springs(springs) {
        }

        Rope(Vector2D start,
            Vector2D end,
            int num_nodes,
            float node_mass,
            float k,
            std::vector<int> pinned_nodes);

        void simulateVerlet(float delta_t, Vector2D gravity);
        void simulateEuler(float delta_t, Vector2D gravity);

        std::vector<Mass*> masses;
        std::vector<Spring*> springs;
    };

}

#endif