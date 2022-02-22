#ifndef _UIANIMATION_H
#define _UIANIMATION_H
#include <stack>
#include <vector>
#include <iostream>
#include <queue>
#include <chrono>
#include <cmath>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace UI::Animation
{
    class AnimatableProperty
    {
    public:
        AnimatableProperty()
        {
            this->value = 0;
        }
        int value;

        int get();
        void set(int value);
    };

    class Animation
    {
    public:
        Animation()
        {
            this->finished = false;
        };
        virtual ~Animation(){};

        AnimatableProperty *property;
        int start;
        int end;

        int64_t length;
        int64_t start_time;
        int64_t end_time;

        bool finished;

        virtual void prep()
        {
            auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            start_time = millisec_since_epoch;
            end_time = millisec_since_epoch + length;
        }

        double easeInSine(double t)
        {
            return sin(1.5707963 * t);
        }

        double easeInOutCubic(double t)
        {
            return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
        }

        double easeInOutSine(double t)
        {
            return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
        }

        double lerp(double a, double b, double t)
        {
            return a + (t * (b - a));
        }

        virtual void tick(int64_t now)
        {
            if (!finished)
            {
                std::cout << "start: " << this->start << " | now: " << this->property->value << " | " << this->end << "\n";
                if (now >= this->end_time)
                {
                    this->property->value = end;
                    this->finished = true;
                    return;
                }

                int64_t delta = now - start_time;
                if (delta != 0)
                {
                    double dt = (double)delta / (double)length;
                    double prog = easeInOutSine(dt);
                    this->property->value = (int)(lerp((double)this->start, (double)this->end, prog));
                }
            }
        };
    };

    class AnimationGroup : public Animation
    {
    public:
        AnimationGroup()
        {
            this->finished = false;
        };
        std::vector<Animation *> animations;

        virtual void prep()
        {
            for (Animation *animation : animations)
            {
                animation->prep();
            }
        }

        virtual void tick(int64_t now)
        {
            bool fin = true;

            for (Animation *animation : animations)
            {
                animation->tick(now);
                if (!animation->finished)
                {
                    fin = false;
                }
            }

            this->finished = fin;
        };
    };

    class AnimationCore
    {
        static std::vector<Animation *> animations;

    public:
        static void add_animations(std::vector<Animation *> animations_to_add)
        {
            for (Animation *animation : animations_to_add)
            {
                animation->prep();
                animations.push_back(animation);
            }
        }

        static void tick()
        {
            auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            // std::cout << "Size: " << animations.size() << "\n";
            for (auto it = animations.begin(); it != animations.end();)
            {
                Animation *animation = *it;
                if (animation->finished)
                {
                    std::cout << "Erase\n";
                    it = animations.erase(it);
                }
                else
                {
                    animation->tick(millisec_since_epoch);
                    it++;
                }
            }
        }
    };

    class Transaction
    {
        static std::stack<Transaction *> transactions;
        std::vector<Animation *> animations;
        int64_t duration;

    public:
        static void begin();
        static void commit();
        static void flush();

        static void add_animation_for_property(AnimatableProperty *property, int start, int end);

        static void set_duration(int64_t duration);
    };
}

#endif