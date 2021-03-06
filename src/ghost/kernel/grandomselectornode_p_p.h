#ifndef GRANDOMSELECTORNODE_P_P_H
#define GRANDOMSELECTORNODE_P_P_H

#include "gcompositenode_p_p.h"
#include "grandomselectornode_p.h"

class GRandomSelectorNodePrivate : public GCompositeNodePrivate
{
    Q_DECLARE_PUBLIC(GRandomSelectorNode)

public:
    GRandomSelectorNodePrivate();
    virtual ~GRandomSelectorNodePrivate();

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
    Ghost::RandomMode randomMode;
};

#endif // GRANDOMSELECTORNODE_P_P_H

