#include <functional>

#include "./include/Game/Decision.h"

Decision::Decision(std::function<bool()> condition, DecisionNode* trueNode, DecisionNode* falseNode) :
condition(condition), trueNode(trueNode), falseNode(falseNode) {}

Decision::~Decision() {}

Action::Action(std::function<void()> action) : action(action) {}

Action::~Action() {}

void Decision::evaluate() {
    if (condition()) {
        trueNode->evaluate();
    } else {
        falseNode->evaluate();
    }
}

void Action::evaluate() {
    action();
}
