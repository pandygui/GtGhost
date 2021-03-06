#ifndef GPRIORITYSEQUENCENODE_P_P_H
#define GPRIORITYSEQUENCENODE_P_P_H

#include "gcompositenode_p_p.h"
#include "gprioritysequencenode_p.h"

class GPrioritySequenceNodePrivate : public GCompositeNodePrivate
{
    Q_DECLARE_PUBLIC(GPrioritySequenceNode)

public:
    GPrioritySequenceNodePrivate();
    virtual ~GPrioritySequenceNodePrivate();

private:
    virtual void confirmEvent(GGhostConfirmEvent *event) Q_DECL_FINAL;

public:
    virtual bool reset() Q_DECL_FINAL;
    virtual void execute() Q_DECL_FINAL;
    virtual bool terminate() Q_DECL_FINAL;

private:
    void executeNextChildNode();

private:
    void resortChildNodes();
private:
    Ghost::UpdateMode updateMode;
    GGhostNodeList sortedChildNodes;
};

#endif // GPRIORITYSEQUENCENODE_P_P_H

