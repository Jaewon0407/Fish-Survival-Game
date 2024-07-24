#ifndef DECISION_H
#define DECISION_H

#include <functional>

class DecisionNode {
public:  
    virtual void evaluate() = 0;
};

class Decision : public DecisionNode {
public: 
    std::function<bool()> condition;
    DecisionNode* trueNode; 
    DecisionNode* falseNode;
    Decision(std::function<bool()> condition, DecisionNode* trueNode, DecisionNode* falseNode);
    ~Decision();
    void evaluate() override;
};

class Action : public DecisionNode {
public:
    std::function<void()> action;
    Action(std::function<void()> action);
    ~Action();
    void evaluate() override;
};

#endif