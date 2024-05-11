#include <iostream>
#include <cmath>
#include <chrono> // For std::this_thread and std::chrono_literals
#include <thread> // For std::chrono::milliseconds
#include <vector>
#include <map>
#include <functional>
#include <set>
#include <omp.h>

class Circle
{
public:
    // Position
    double px;
    double py;

    // Velocity
    double vx;
    double vy;

    // Acceleration
    double ax;
    double ay;

    // Mass
    double m;

    // Radius
    double r;

    double left;
    double right;

    Circle(double px, double py, double vx, double vy, double ax, double ay, double m, double r)
    {
        this->px = px;
        this->py = py;

        this->vx = vx;
        this->vy = vy;

        this->ax = ax;
        this->ay = ay;

        this->m = m;

        this->r = r;

        this->left = px - r;
        this->right = px + r;
    }

    void handleBoxCollision()
    {
        if((px - r) <= 0 || (px + r >= 1280))
            this->vx = -this->vx;

        if((py - r) <= 0 || (py + r >= 720))
            this->vy = -this->vy;
    }

    void handleParticleCollision();

    void update_frame(double dt)     // dt = 1/FPS
    {
        vx = vx + (this->ax * dt);
        vy = vy + (this->ay * dt);

        px = px + (vx * dt);
        py = py + (vy * dt);

        left = px - r;
        right = px + r;

        handleBoxCollision();
        handleParticleCollision();
    }

    bool operator<(const Circle& obj) const
    { 
        return px < obj.px; 
    }
};

bool compareParticles(Circle* a, Circle* b)
{
    return ((*a).px < (*b).px);
}

bool intersect(Circle* a, Circle* b)
{
    if(a->left <= b->right && a->right >= b->left)
        return true;

    return false;
}

std::vector <Circle*> particles;
std::vector <Circle*> active_list;
std::map <Circle*, Circle*> completed_collisions;

void sweep_and_prune()
{
    sort(particles.begin(), particles.end(), compareParticles);

    std::set <Circle*> x;

    #pragma omp for
    for(int i = 0; i < particles.size() - 1; i++)
    {
        if(intersect(particles[i], particles[i + 1]))
        {
            x.insert(particles[i]);
            x.insert(particles[i + 1]);
        }
    }

    #pragma omp for
    for(auto i : x)
        active_list.push_back(i);

    sort(active_list.begin(), active_list.end(), compareParticles);

    // #pragma omp for
    // for(auto i : active_list)
    // {
    //     std::cout << i->px << " ";
    // }

    // std::cout << std::endl;
}

int main() 
{
    // Create a circle inside the box
    Circle circle1(709, 690, 100, 100, 0, 0, 1, 10);
    Circle circle2(730, 690, 100, 100, 0, 0, 1, 10);
    Circle circle3(735, 690, 100, 100, 0, 0, 1, 10);
    Circle circle4(800, 690, 100, 100, 0, 0, 1, 10);
    Circle circle5(810, 690, 100, 100, 0, 0, 1, 10);
    Circle circle6(900, 690, 100, 100, 0, 0, 1, 10);
    Circle circle7(1000, 690, 100, 100, 0, 0, 1, 10);
    Circle circle8(1200, 690, 100, 100, 0, 0, 1, 100);

    particles.push_back(&circle1);
    particles.push_back(&circle2);
    particles.push_back(&circle3);
    particles.push_back(&circle4);
    particles.push_back(&circle5);
    particles.push_back(&circle6);
    particles.push_back(&circle7);
    particles.push_back(&circle8);

    const double fps = 60.0;
    const double frame_time = 1.0 / fps;

    // Simulate for a few frames
    for (int i = 0; i < 100; ++i) 
    {
        auto start_time = std::chrono::high_resolution_clock::now(); // Start time of the frame

        sweep_and_prune();
        circle1.update_frame(frame_time);
        completed_collisions.clear();
        active_list.clear();
        std::cout << "Position: (" << circle1.px << ", " << circle1.py << ")\n";

        auto end_time = std::chrono::high_resolution_clock::now(); // End time of the frame
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count(); // Elapsed time in microseconds

        // Introduce a delay to achieve the desired frame rate
        if (elapsed_time < frame_time * 1000000) {
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(frame_time * 1000000 - elapsed_time)));
        }
    }

    return 0;
}

void Circle::handleParticleCollision()
{
    for(auto p : active_list)
    {
        int done = 0;

        #pragma omp for
        for(auto itr = completed_collisions.find(this); itr != completed_collisions.end(); itr++)
            if(itr->second == p)
                done = 1;

        if(p != this && done == 0)
        {
            double dist = std::sqrt(std::pow((this->px - p->px), 2) + std::pow((this->py - p->py), 2));

            if(dist <= (this->r + p->r))
            {
                completed_collisions.insert({this, p});
                completed_collisions.insert({p, this});

                this->vx = ((this->m - p->m) * this->vx + 2 * p->m * p->vx) / (this->m + p->m);
                this->vy = ((this->m - p->m) * this->vy + 2 * p->m * p->vy) / (this->m + p->m);

                p->vx = ((p->m - this->m) * p->vx + 2 * this->m * this->vx) / (this->m + p->m);
                p->vy = ((p->m - this->m) * p->vy + 2 * this->m * this->vy) / (this->m + p->m);
            }
        }
    }
}