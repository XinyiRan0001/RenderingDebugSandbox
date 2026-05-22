#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        Vector2D step = (end - start) / (num_nodes - 1);

        for (int i = 0; i < num_nodes; i++)
        {
            Vector2D position = start + step * i;
            masses.push_back(new Mass(position, node_mass, false));
        }

        for (int i = 0; i < num_nodes - 1; i++)
        {
            springs.push_back(new Spring(masses[i], masses[i + 1], k));
        }

        for (auto& i : pinned_nodes)
        {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto& s : springs)
        {
            Vector2D ab = s->m2->position - s->m1->position;
            float current_length = ab.norm();
            Vector2D direction = ab / current_length;

            Vector2D force = s->k * (current_length - s->rest_length) * direction;

            s->m1->forces += force;
            s->m2->forces -= force;
        }

        for (auto& m : masses)
        {
            if (!m->pinned)
            {
                m->forces += gravity * m->mass;

                Vector2D acceleration = m->forces / m->mass;

                m->velocity += acceleration * delta_t;
                m->position += m->velocity * delta_t;

                m->velocity *= 0.99;
            }

            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto& s : springs)
        {
            Vector2D ab = s->m2->position - s->m1->position;
            float current_length = ab.norm();

            if (current_length == 0)
                continue;

            Vector2D direction = ab / current_length;
            Vector2D correction = direction * (current_length - s->rest_length);

            if (!s->m1->pinned && !s->m2->pinned)
            {
                s->m1->position += correction * 0.5;
                s->m2->position -= correction * 0.5;
            }
            else if (s->m1->pinned && !s->m2->pinned)
            {
                s->m2->position -= correction;
            }
            else if (!s->m1->pinned && s->m2->pinned)
            {
                s->m1->position += correction;
            }
        }

        for (auto& m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;

                float damping_factor = 0.00005;
                m->position =
                    m->position +
                    (1 - damping_factor) * (m->position - m->last_position) +
                    gravity * delta_t * delta_t;

                m->last_position = temp_position;
            }
        }
    }
}