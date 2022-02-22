#include "UIAnimation.hpp"

using namespace UI::Animation;

int AnimatableProperty::get()
{
    return this->value;
}

void AnimatableProperty::set(int new_value)
{
    if (this->value != new_value)
    {
        Transaction::add_animation_for_property(this, this->value, new_value);
    }
}

// Init static member variables
std::stack<Transaction *> Transaction::transactions = std::stack<Transaction *>();
std::vector<Animation *> AnimationCore::animations = std::vector<Animation *>();

void Transaction::begin()
{
    // std::cout << "Beginning transaction\n";
    Transaction *transaction = new Transaction;
    transaction->duration = 0;
    transactions.push(transaction);
}

void Transaction::commit()
{
    if (transactions.empty())
    {
        // TODO: Replace with exception
        return;
    }

    Transaction *transaction = transactions.top();
    AnimationGroup *group = new AnimationGroup();

    if (transaction->animations.empty())
    {
        return;
    }

    for (Animation *animation : transaction->animations)
    {
        group->animations.push_back(animation);
    }

    std::vector<Animation *> animations = std::vector<Animation *>();
    animations.push_back(group);
    AnimationCore::add_animations(animations);
}

void Transaction::flush()
{
    // std::cout << "Flushing transactions\n";
    while (!transactions.empty())
    {
        Transaction *transaction = transactions.top();
        transaction->commit();
        transactions.pop();

        delete transaction;
    }
}

void Transaction::set_duration(int64_t duration)
{
    Transaction *transaction = transactions.top();
    transaction->duration = duration;
}

void Transaction::add_animation_for_property(AnimatableProperty *property, int start, int end)
{
    // std::cout << "Starting new animation\n";
    Transaction *transaction = transactions.top();
    Animation *animation = new Animation();
    animation->property = property;
    animation->start = start;
    animation->end = end;
    animation->length = transaction->duration;

    transaction->animations.push_back(animation);
}